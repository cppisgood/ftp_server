#pragma once
#include "asio.hpp"
#include "asio/buffer.hpp"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <queue>
#include <filesystem>
#include <tools.hpp>

enum class Status {
    WAITING_USER,
    WATTING_PASS,
    LOGGED,
};

enum class Message_type {
    USER,
    PASS,
    SYST,
    QUIT,
    PORT,
    LIST,
};

// enum class Response_type {

// };

// std::string get_response_header(Response_type response_code) {
//     return std::to_string(static_cast<int>(response_code));
// }
std::string get_response_header(int response_code) {
    return std::to_string(response_code);
}

template<typename Type_message>
struct Message {
    Type_message type;
    // std::string header;
    std::string body;

    Message() {}
    Message(Type_message type_) : type(type_) {}
 
    friend Message& operator<<(Message& msg, const std::string& data) {
        msg.body += data;
        return msg;
    }
};


template <typename T>
Message<T> parse(const std::string raw_msg) {
    LOG(__PRETTY_FUNCTION__);
    Message<Message_type> msg;
    std::size_t st = 0;
    while (st < raw_msg.size() && std::isspace(raw_msg[st])) ++st;
    std::size_t i = st;
    while (i < raw_msg.size() && !std::isspace(raw_msg[i])) ++i;
    if (i - st > 4) {
        ERROR(string_format("[error] %s command type too long. raw message: %s", __PRETTY_FUNCTION__, raw_msg.c_str()));
    } else if (i != raw_msg.size()) {
        std::size_t j = strlen(raw_msg.data()) - 1;
        while (j > i && std::isspace(raw_msg[j])) --j;
        if (j > i) {
            msg.body = raw_msg.substr(i + 1, j - i);
        }
    }
    std::string type = raw_msg.substr(st, i - st);
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);
    if (type == "USER") msg.type = Message_type::USER;
    else if (type == "PASS") msg.type = Message_type::PASS;
    else if (type == "SYST") msg.type = Message_type::SYST;
    else if (type == "QUIT") msg.type = Message_type::QUIT;
    else if (type == "LIST") msg.type = Message_type::LIST;
    else if (type == "PORT") msg.type = Message_type::PORT;
    else {
        // TODO
    }
    return msg;
}

// class Connection : public std::enable_shared_from_this<Connection> {
//     std::string buf;
//     asio::ip::tcp::socket socket;
// public:
//     Connection(asio::io_context& context) : socket(context) {
        
//     }

//     template<typename Message_type>
//     Message<Message_type> async_read() {
//         asio::async_read(socket, asio::buffer(buf), [this](const asio::error_code& ec, size_t size) {
//             if (ec) {
//                 std::cerr << "[error] Connection.async_read has received " << size << " bytes with error_code: " << ec << '\n';
//             }
//         });
//         return paser<Message_type>(buf);
//     }

//     void async_write(std::string msg) {
//         asio::async_write(socket, asio::buffer(msg), [](const asio::error_code& ec, size_t size) {
//             if (ec) {
//                 std::cerr << "[error] Connection.async_write has sent " << size << " bytes with error_code: " << ec << '\n';
//             }
//         });
//     }

//     asio::ip::tcp::socket& get_socket() {
//         return socket;
//     }
// };

void do_nothing() {}

class User;
template <typename T>
void handle_command(std::shared_ptr<User> user, Message<T> msg);



class User : public std::enable_shared_from_this<User> {
public:
    std::string buf;
    asio::io_context& context;
    asio::ip::tcp::socket socket;
    asio::ip::tcp::endpoint data_endpoint;
    std::string username;
    std::filesystem::path current_path;
    Status status = Status::WAITING_USER;
    
    User(asio::io_context& context_) :
        context(context_),
        socket(context_) {
        buf.resize(1024);
    }

    // template<typename T>
    // Message<T> async_read() {
        // LOG(__PRETTY_FUNCTION__);
        // std::promise<std::string> pm;
        // std::string buf;
        // asio::async_read(socket, asio::buffer(buf), [](const asio::error_code& ec, size_t size) {
        //     if (ec) {
        //         ERROR(string_format("[error] User.async_read has received %d bytes with error_code: %s\n", size, ec.message()));
        //     }
        //     pm.set_value(buf);
        // });
        // return parse<T>(ret.get());
    // }

    void async_write(const std::string& msg) {
        LOG(__PRETTY_FUNCTION__);
        asio::async_write(socket, asio::buffer(buf), [msg](const asio::error_code& ec, size_t size) {
            if (ec) {
                std::cerr << "[error] Connection.async_write has sent " << size << " bytes with error_code: " << ec << '\n';
            }
            debug("have write", msg);
        });
        debug(msg, "writed", asio::buffer(msg).size(), asio::buffer(msg).data(), "#");
    }
    template<typename T>
    Message<T> read() {
        LOG(__PRETTY_FUNCTION__);
        Message<T> msg;
        std::string buf(128, '\0');
        socket.read_some(asio::buffer(buf));
        return parse<T>(buf);
    }

    void write(const std::string& msg) {
        LOG(__PRETTY_FUNCTION__);
        socket.write_some(asio::buffer(msg));
    }

    asio::ip::tcp::socket& get_socket() {
        return socket;
    }

    bool check_username(const std::string& username) {
        // TODO
        return true;
    }

    bool check_password(std::string password) {
        // TODO
        return true;
    }

    void async_login() {
        auto msg = read<Message_type>();
        LOG(__PRETTY_FUNCTION__);
        switch (msg.type) {
        case Message_type::USER:
            if (status == Status::WAITING_USER) {
                if (check_username(msg.body)) {
                    status = Status::WATTING_PASS;
                    username = msg.body;
                    write(string_format("331 user %s ok, password required.\n", msg.body.c_str()));
                } else {
                    write(string_format("530 user %s not found\n", msg.body.c_str()));
                }
            } else {
                // TODO
            }
            break;
        case Message_type::PASS:
            if (status == Status::WATTING_PASS) {
                if (check_password(msg.body)) {
                    status = Status::LOGGED;
                    write(string_format("230 user %s logged in\n",username.c_str()));
                    async_wait_command();
                    return;
                } else {
                    status = Status::WAITING_USER;
                    write(string_format("530 login authentication failed\n"));
                }
            } else {
                // TODO
            }
            break;
        default:
            do_nothing();
            handle_command(shared_from_this(), msg);
        }
        async_login();
    }

    void async_wait_command() {
        LOG(__PRETTY_FUNCTION__);

        // auto msg = read<Message_type>();
        // handle_command(shared_from_this(), msg);
        // wait_command();

        socket.async_read_some(asio::buffer(buf), [this](const asio::error_code& ec, size_t size) {
            if (ec) {
                ERROR(string_format("[error] %s", __PRETTY_FUNCTION__, ec.message().c_str()));
            }
            auto msg = parse<Message_type>(buf);
            handle_command(shared_from_this(), msg);
            async_wait_command();
        });
    }

    void disconnect() {
        LOG(__PRETTY_FUNCTION__);
        // TODO
    }

};



void handle_syst(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    user->write("215 UNIX Type: L8\n");
}

void handle_port(std::shared_ptr<User> user, Message<Message_type> msg) {
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
    
    user->write(string_format("200 PORT command successful\n"));
    user->data_endpoint = 
        *asio::ip::tcp::resolver(user->context).resolve(string_format("%d.%d.%d.%d", ip_vec[0], ip_vec[1], ip_vec[2], ip_vec[3]),
        std::to_string(port));
}



void handle_list(std::shared_ptr<User> user) {
    LOG(__PRETTY_FUNCTION__);
    // TODO specify the default work dir of server
    std::string work_dir = "/home/zmm/Documents/code/cpp/ftp_server";
    std::system(string_format("ls -l > %s/ls_out.txt", work_dir.c_str()).c_str());
    std::ifstream f(string_format("%s/ls_out.txt", work_dir.c_str()));
    f.seekg(0, std::ios::end);
    std::string buf(f.tellg(), '\0');
    f.seekg(0, std::ios::beg);
    f.read(buf.data(), buf.size());
    f.close();
    // TODO push job to server
    user->write(string_format("150 Connecting to port %d\n", user->data_endpoint.port()));
    asio::ip::tcp::socket data_socket(user->context, *asio::ip::tcp::resolver(user->context).resolve("127.0.0.1", "20"));
    data_socket.connect(user->data_endpoint);
    data_socket.write_some(asio::buffer(buf + '\n'));
    user->write(string_format("226 ls command ojbk\n"));
}


template <typename T>
void handle_command(std::shared_ptr<User> user, Message<T> msg) {
    LOG(__PRETTY_FUNCTION__);
    switch (msg.type) {
    case T::SYST:
        handle_syst(user);
        break;
    case T::PORT:
        handle_port(user, msg);
        break;
    case T::LIST:
        handle_list(user);
        break;
    case T::QUIT:
        user->disconnect();
        break;
    default:
        ERROR((int)msg.type, msg.body);
    }
}



class Server {
    asio::io_context& context;
    asio::ip::tcp::acceptor ac;
public:

    Server(asio::io_context& context_, std::string ip, int port) :
        context(context_),
        ac(context_, *asio::ip::tcp::resolver(context_).resolve(ip, std::to_string(port))) {
    }

    void start() {
        LOG(__PRETTY_FUNCTION__);
        start_accept();
    }

    void start_accept() {
        LOG(__PRETTY_FUNCTION__);
        auto user = std::make_shared<User>(context);
        ac.async_accept(user->get_socket(), [this, user](const asio::error_code& ec) {
            if (ec) {
                ERROR(string_format("[error] %s error_code: %s\n", __PRETTY_FUNCTION__, ec.message().c_str()));
            }
            start_accept();
            user_queue.push(user);
            user->write("220 welcome to ftp server!\n");
            user->async_login();
        });
    }

    void stop() {

    }

    std::queue<std::shared_ptr<User>> user_queue;

};