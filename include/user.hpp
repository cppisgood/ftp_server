#pragma once

#include "net.hpp"
#include <filesystem>
#include <memory>
#include <string>

class User;
void handle_command(std::shared_ptr<User> user, Message msg);

class User : public std::enable_shared_from_this<User> {
public:
    enum class Status_type {
        WAITING_USER,
        WATTING_PASS,
        LOGGED,
    };
    enum class Trans_type {
        A,
        I
    };
    // std::string buf;
    asio::io_context& context;
    // asio::ip::tcp::socket socket;
    static constexpr char CRLF[] = "\r\n";
    asio::ip::tcp::endpoint data_endpoint;
    std::string username;
    std::filesystem::path current_path {"/home/zmm/Documents/code/cpp/ftp_server"};
    Status_type status = Status_type::WAITING_USER;
    Trans_type trans_type = Trans_type::A;
    std::shared_ptr<User> self;
    Connection con;
    User(asio::io_context& context_) :
        context(context_),
        con(context_) {
    }

    Message get_message() {
        LOG(__PRETTY_FUNCTION__);
        return con.read();
    }

    void response(int code, const std::string& msg) {
        LOG(__PRETTY_FUNCTION__);
        con.write(string_format("%d %s%s", code, msg.c_str(), CRLF));
    }

    asio::ip::tcp::socket& get_socket() {
        return con.get_socket();
    }

    bool check_username(const std::string& username) {
        // TODO
        if (!username.empty()) return true;
        return false;
    }

    bool check_password(std::string password) {
        // TODO
        if (!password.empty()) return true;
        return false;
    }

    void login() {
        LOG(__PRETTY_FUNCTION__);
        auto msg = get_message();
        switch (msg.type) {
        case Message_type::USER:
            if (status == Status_type::WAITING_USER) {
                if (check_username(msg.body)) {
                    status = Status_type::WATTING_PASS;
                    username = msg.body;
                    response(331, string_format("user %s ok, password required.", msg.body.c_str()));
                } else {
                    response(530, string_format("user %s not found", msg.body.c_str()));
                }
            } else {
                // TODO
            }
            break;
        case Message_type::PASS:
            if (status == Status_type::WATTING_PASS) {
                if (check_password(msg.body)) {
                    status = Status_type::LOGGED;
                    response(230, string_format("user %s logged in",username.c_str()));
                    async_wait_command();
                    // TODO load user info, such as root directory
                    return;
                } else {
                    status = Status_type::WAITING_USER;
                    response(530, string_format("login authentication failed"));
                }
            } else {
                // TODO
            }
            break;
        case Message_type::SYST:
            handle_command(shared_from_this(), msg);
            break;
        default:
            response(530, "log first please.");
        }
        login();
    }

    void async_wait_command() {
        LOG(__PRETTY_FUNCTION__);
        auto p = shared_from_this();
        con.async_wait_command([this, p](Message msg) {
            handle_command(p, msg);
        });
    }

    void disconnect() {
        LOG(__PRETTY_FUNCTION__);
        con.socket.cancel();
        con.socket.close();
        // TODO
    }

    ~User() {
        LOG(__PRETTY_FUNCTION__);
        ERROR("BYE");
    }

};