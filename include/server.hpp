#pragma once
#include "asio.hpp"
#include "net.hpp"
#include "user.hpp"
#include "handler.hpp"
#include <queue>
#include <string>
#include "server_config.hpp"

class Server {
public:
    asio::io_context& context;
    asio::ip::tcp::acceptor ac;

    void init() {
        if (!std::filesystem::exists(server_config::work_path)) {
            std::filesystem::create_directories(server_config::work_path);
        }
        if (!std::filesystem::exists(server_config::default_ftp_path)) {
            std::filesystem::create_directories(server_config::default_ftp_path);
        }
    }

    Server(asio::io_context& context_) :
        context(context_),
        ac(context_, *asio::ip::tcp::resolver(context_).resolve("0.0.0.0", server_config::server_port)) {
        
    }
    Server(asio::io_context& context_, std::string ip, std::string port) :
        context(context_),
        ac(context_, *asio::ip::tcp::resolver(context_).resolve("0.0.0.0", port)) {
        server_config::server_ip = ip;
        server_config::server_port = port;
        
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