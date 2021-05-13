#include "asio.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <cctype>


enum class Status {
    WAITING_USER,
    WATTING_PASS,
    LOGGED,
};

enum class Message_type {
    USER,
    PASS,
    SYST
};

template<typename Type_message>
struct Message {
    Type_message type;
    std::string header;
    std::string body;
};


template <typename T>
Message<T> parse(const std::string& raw_msg) {
    Message<Message_type> msg;
    auto p = raw_msg.find(' ');
    std::string type = raw_msg.substr(0, p);
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);
    if (type == "USER") msg.type = Message_type::USER;
    else if (type == "PASS") msg.type = Message_type::PASS;
    else if (type == "SYST") msg.type = Message_type::SYST;
    else {
        // TODO
    }
    msg.body = raw_msg.substr(p + 1);
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

class User : public std::enable_shared_from_this<User> {
    std::string buf;
    asio::ip::tcp::socket socket;
    std::string username;
    // std::shared_ptr<Connection> con;
    Status status = Status::WAITING_USER;
public:
    User(asio::io_context context) : socket(context) {}

    template<typename T>
    Message<T> async_read() {
        asio::async_read(socket, asio::buffer(buf), [this](const asio::error_code& ec, size_t size) {
            if (ec) {
                std::cerr << "[error] Connection.async_read has received " << size << " bytes with error_code: " << ec << '\n';
            }
        });
        return parse<T>(buf);
    }

    void async_write(std::string msg) {
        asio::async_write(socket, asio::buffer(msg), [](const asio::error_code& ec, size_t size) {
            if (ec) {
                std::cerr << "[error] Connection.async_write has sent " << size << " bytes with error_code: " << ec << '\n';
            }
        });
    }

    asio::ip::tcp::socket& get_socket() {
        return socket;
    }

    bool check_password(std::string password) {
        // TODO
        return true;
    }

    void async_login() {
        auto msg = async_read<Message_type>();
        switch (msg.type) {
        case Message_type::USER:
            if (status == Status::WAITING_USER) {
                status = Status::WATTING_PASS;
                username = msg.body;
                break;
            } else {
                // TODO
            }
        case Message_type::PASS:
            if (status == Status::WATTING_PASS) {
                if (check_password(msg.body)) {
                    status = Status::LOGGED;
                    wait_command();
                } else {
                    status = Status::WAITING_USER;
                    // TODO respose relogin msg
                }
                break;
            }
        default:
            do_nothing();
            // handle_command(msg);
        }
    }

    void wait_command() {
        auto msg = async_read<Message_type>();

    }

};


class Server {
    asio::ip::tcp::acceptor ac;
public:

    Server(asio::io_context& context, std::string ip, int port) :
        ac(context, *asio::ip::tcp::resolver(context).resolve(ip, std::to_string(port))) {
    }

    void start() {
        start_accept();
    }

    void start_accept() {
        auto user = std::make_shared<User>(ac.get_executor());
        ac.async_accept(user->get_socket(), [this, &user](const asio::error_code& ec) {
            if (ec) {
                std::cerr << "[error] Server.start_accept error_code: " << ec << '\n';
            }
            start_accept();
            user_queue.push(user);
        });
    }

    void stop() {

    }

    std::queue<std::shared_ptr<User>> user_queue;

};