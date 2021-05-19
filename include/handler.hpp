#pragma once

#include "tools.hpp"
#include "user.hpp"
#include <algorithm>
#include <filesystem>
#include <ios>
#include <iterator>
#include <memory>
#include <ranges>
#include <fstream>
#include <string>
#include "asio.hpp"
#include "server_config.hpp"


// if file is large than 1MB, split to small part
template<std::size_t max_size = (1 << 20)>
void send_file(asio::ip::tcp::socket& socket, const std::string path) {
    LOG(__PRETTY_FUNCTION__);
    std::ifstream f(path);
    f.seekg(0, std::ios::end);
    std::size_t total_len = f.tellg();
    f.seekg(0, std::ios::beg);
    std::string buf;
    while (total_len > 0) {
        int len = std::min(max_size, total_len);
        buf.resize(len);
        f.read(buf.data(), buf.size());
        total_len -= len;
        socket.write_some(asio::buffer(buf));
    }
    f.close();
    socket.close();
}

template<std::size_t max_size = (1 << 20)>
void receive_file(asio::ip::tcp::socket& socket, const std::string path) {
    LOG(__PRETTY_FUNCTION__);
    std::ofstream f(path);
    asio::error_code ec;
    std::string buf(max_size, '\0');
    while (true) {
        auto len = socket.read_some(asio::buffer(buf), ec);
        if (ec) {
            break;
        }
        buf.resize(len);
        f.write(buf.data(), buf.size());
    }
    
}

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
    user->port_type = User::Port_type::PORT;
    user->data_endpoint = 
        *asio::ip::tcp::resolver(user->context).resolve(string_format("%d.%d.%d.%d", ip_vec[0], ip_vec[1], ip_vec[2], ip_vec[3]),
        std::to_string(port));
}

void handle_list(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    auto work_path = server_config::work_path / user->username;
    std::system(string_format("cd %s && ls -al > %s/tmp_files/ls_out.txt", user->current_path.c_str(), work_path.c_str()).c_str());

    send_file(user->get_data_socket(), string_format("%s/tmp_files/ls_out.txt", work_path.c_str()));
    user->data_socket.close();
    user->response(226, string_format("ls command ojbk"));
}

std::filesystem::path show_path(std::shared_ptr<User> user) {
    return (std::filesystem::path("/") / std::filesystem::relative(user->current_path, user->root_path)).lexically_normal();
}

std::filesystem::path concat_real_path(std::shared_ptr<User> user, std::string path) {
    std::filesystem::path new_path;
    if (path.front() == '/') new_path = user->root_path / path.substr(1);
    else new_path = user->current_path / path;
    return new_path;
}

void handle_cwd(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    auto next_path = concat_real_path(user, msg.body);
    try {
        next_path = std::filesystem::canonical(next_path);
        if (!std::filesystem::is_directory(next_path)) {
            user->response(550, string_format("can't change directory to %s: not a directory"));
        } else {
            auto [end1, end2] = std::mismatch(user->root_path.begin(), user->root_path.end(), next_path.begin(), next_path.end());
            if (end1 == user->root_path.end()) {
                user->current_path = next_path;
                user->response(250, string_format("ok. current directory is %s", show_path(user).c_str()));
            } else if (end2 == next_path.end()) {
                user->current_path = user->root_path;
                user->response(250, string_format("ok. current directory is %s", show_path(user).c_str()));
            } else {
                user->response(550, string_format("can't change directory to %s: no such file or directory", msg.body.c_str()));
            }
        }
    } catch(...) {
        user->response(550, string_format("can't change directory to %s: no such file or directory", msg.body.c_str()));
    } 
}

void handle_quit(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    user->response(221, "bye");
    user->disconnect();
    user->self.reset();
}

void handle_type(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    if (msg.body[0] == 'A') user->trans_type = User::Trans_type::A;
    else user->trans_type = User::Trans_type::I;
    user->response(220, string_format("type is now %s", msg.body.c_str()));
}

void handle_retr(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    auto file_path = concat_real_path(user, msg.body);
    try {
        file_path = std::filesystem::canonical(file_path);
        if (std::filesystem::is_directory(file_path)) {
            user->response(550, string_format("can't open to %s: is not a file", msg.body.c_str()));
        } else {
            send_file(user->get_data_socket(), file_path);
            user->data_socket.close();
            user->response(226, "file successfully transed");
        }
    } catch(...) {
        user->response(550, string_format("can't open to %s: no such file or directory", msg.body.c_str()));
    }
}

void handle_stor(std::shared_ptr<User> user, Message msg) {
    LOG(__PRETTY_FUNCTION__);
    auto file_path = concat_real_path(user, msg.body);
    if (std::filesystem::exists(file_path)) {
        user->response(550, string_format("users %s isn't permitted to overwrite existing files", user->username.c_str()));
    } else {
        receive_file(user->get_data_socket(), file_path);
        user->data_socket.close();
        user->response(226, "file successfully transferred");
    }
}

void handle_pwd(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    user->response(257, show_path(user));
}

void handle_pasv(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    // TODO get port from config
    auto get_port = [] {
        return 6666;
    };
    user->port_type = User::Port_type::PASV;
    auto ip = server_config::server_ip;
    std::transform(ip.begin(), ip.end(), ip.begin(), [](char const& ch) {
        if (ch == '.') return ',';
        return ch;
    });
    auto port = get_port();
    user->response(227, string_format("=%s,%d,%d", ip.c_str(), port / 256, port % 256));
    user->connect_pasv_data_socket(std::to_string(port));
}

void handle_size(std::shared_ptr<User> user, Message msg) {
    auto file_path = concat_real_path(user, msg.body);
    user->response(213, string_format("%d", std::filesystem::file_size(file_path)));
}

void handle_command(std::shared_ptr<User> user, Message msg) {
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
    case Message_type::PWD:
        handle_pwd(user);
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
    case Message_type::STOR:
        handle_stor(user, msg);
        break;
    case Message_type::PASV:
        handle_pasv(user);
        break;
    case Message_type::SIZE:
        handle_size(user, msg);
        break;
    default:
        ERROR((int)msg.type, msg.body);
    }
}