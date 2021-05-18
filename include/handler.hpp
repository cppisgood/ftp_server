#pragma once

#include "tools.hpp"
#include "user.hpp"
#include <memory>
#include <ranges>
#include <fstream>



void handle_syst(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    user->response(215, "UNIX Type: L8");
}

void handle_port(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    auto sv = msg.body
        | std::ranges::views::split(',')
        | std::ranges::views::transform([](auto&& rng) {
            return stoi(std::string(&*rng.begin(), std::ranges::distance(rng)));
        });
    std::vector<int> ip_vec(4);
    auto p = sv.begin();
    for (auto& x : ip_vec) x = *p++;
    int p1 = *p++;
    int p2 = *p++;
    int port = p1 * 256 + p2;
    
    user->response(200, string_format("PORT command successful"));
    user->data_endpoint = 
        *asio::ip::tcp::resolver(user->context).resolve(string_format("%d.%d.%d.%d", ip_vec[0], ip_vec[1], ip_vec[2], ip_vec[3]),
        std::to_string(port));
}

void handle_list(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    // TODO specify the default work dir of server
    std::string work_path = "/home/zmm/Documents/code/cpp/ftp_server/";
    std::system(string_format("cd %s && ls -al > %s/tmp_files/ls_out.txt", user->current_path.c_str(), work_path.c_str()).c_str());
    std::ifstream f(string_format("%s/tmp_files/ls_out.txt", work_path.c_str()));
    f.seekg(0, std::ios::end);
    std::string buf(f.tellg(), '\0');
    f.seekg(0, std::ios::beg);
    f.read(buf.data(), buf.size());
    f.close();
    // TODO push job to server
    asio::ip::tcp::socket data_socket(user->context, asio::ip::tcp::v4());
    data_socket.set_option(asio::socket_base::reuse_address(true));
    data_socket.bind(*asio::ip::tcp::resolver(user->context).resolve("127.0.0.1", "20"));
    // TODO PASV support
    data_socket.connect(user->data_endpoint);
    user->response(150, string_format("Connecting to port %d", user->data_endpoint.port()));
    data_socket.write_some(asio::buffer(buf));
    user->response(226, string_format("ls command ojbk"));
}

void handle_cwd(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    auto next_path = std::filesystem::weakly_canonical(user->current_path / msg.body);
    if (std::filesystem::exists(next_path)) {
        debug("Yes");
        user->current_path = next_path;
        user->response(250, string_format("ok. current directory is %s", user->current_path.c_str()));
    } else {
        debug("No");
        user->response(550, string_format("can't change directory to %s: no such directory", next_path.c_str()));
    }
    
}

void handle_quit(std::shared_ptr<User> user) {
    user->response(221, "bye");
    user->disconnect();
    user->self.reset();
}

void handle_type(std::shared_ptr<User> user, Message msg) {
    if (msg.body[0] == 'A') user->trans_type = User::Trans_type::A;
    else user->trans_type = User::Trans_type::I;
    user->response(220, string_format("type is now %s", msg.body.c_str()));
}

void handle_retr(std::shared_ptr<User> user, Message msg) {
    auto file_path = std::filesystem::weakly_canonical(user->current_path / msg.body);
    if (std::filesystem::exists(file_path)) {
        debug("Yes");
        asio::ip::tcp::socket data_socket(user->context, asio::ip::tcp::v4());
        data_socket.set_option(asio::socket_base::reuse_address(true));
        data_socket.bind(*asio::ip::tcp::resolver(user->context).resolve("127.0.0.1", "20"));
        data_socket.connect(user->data_endpoint);

        user->response(150, string_format("Connecting to port %d", user->data_endpoint.port()));

        std::ifstream f(file_path);
        constexpr long long max_size = (1 << 20);
        f.seekg(0, std::ios::end);
        long long total_len = f.tellg();
        f.seekg(0, std::ios::beg);
        // long long r = 0;
        std::string buf;
        while (total_len > 0) {
            int len = std::min(max_size, total_len);
            buf.resize(len);
            f.read(buf.data(), buf.size());
            // debug(r, next_r, total_len, max_size);
            total_len -= len;
            data_socket.write_some(asio::buffer(buf));
        }

        user->response(226, "file successfully transed");
    } else {
        debug("No");
        debug("current", user->current_path);
        debug("msg", msg.body);
        debug(file_path);
        user->response(550, string_format("can't open to %s: no such directory", msg.body.c_str()));
    }
}

void handle_command(std::shared_ptr<User> user, Message msg) {
    debug("handle", user.use_count());
    LOG(__PRETTY_FUNCTION__);
    switch (msg.type) {
    case Message_type::SYST:
        handle_syst(user);
        break;
    case Message_type::PORT:
        handle_port(user, msg);
        break;
    case Message_type::LIST:
        handle_list(user);
        break;
    case Message_type::CWD:
        handle_cwd(user, msg);
        break;
    case Message_type::QUIT:
        handle_quit(user);
        break;
    case Message_type::TYPE:
        handle_type(user, msg);
        break;
    case Message_type::RETR:
        handle_retr(user, msg);
        break;
    default:
        ERROR((int)msg.type, msg.body);
    }
}