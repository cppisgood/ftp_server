#pragma once
#include "asio.hpp"
#include "net.hpp"
#include "user.hpp"
#include "handler.hpp"
#include <queue>


class Server {
public:
    asio::io_context& context;
    asio::ip::tcp::acceptor ac;

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
        user->self = user->shared_from_this();
        ac.async_accept(user->get_socket(), [this, user](const asio::error_code& ec) {
            if (ec) {
                ERROR(string_format("[error] error_code: %s\n", ec.message().c_str()));
                return;
            }
            start_accept();
            // user_queue.push(user);
            user->response(220, "welcome to ftp server!");
            user->login();
        });
    }

    void stop() {

    }

    std::queue<std::shared_ptr<User>> user_queue;

};