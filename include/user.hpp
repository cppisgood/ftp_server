#pragma once

#include "asio/ip/tcp.hpp"
#include "net.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include "server_config.hpp"

class User;
void handle_command(std::shared_ptr<User> user, Message msg);

class User : public std::enable_shared_from_this<User> {
public:
    enum class Login_status_type {
        WAITING_USER,
        WATTING_PASS,
        LOGGED,
    };
    enum class Trans_type {
        A,
        I
    };
    enum class Port_type {
        PORT,
        PASV,
    };
    asio::io_context& context;
    static constexpr char CRLF[] = "\r\n";
    asio::ip::tcp::endpoint data_endpoint;
    std::string username;
    Connection con;
    asio::ip::tcp::socket data_socket;
    std::filesystem::path root_path;
    std::filesystem::path current_path;
    Login_status_type status = Login_status_type::WAITING_USER;
    Port_type port_type = Port_type::PORT;
    Trans_type trans_type = Trans_type::A;
    std::shared_ptr<User> self;
    User(asio::io_context& context_) :
        context(context_),
        con(context_),
        data_socket(context) {
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

    void user_init(std::string root_path_ = "") {
        if (root_path_ != "") {
            root_path = root_path_;
        } else {
            root_path = server_config::default_ftp_path;
        }
        current_path = root_path;
    }

    void login() {
        LOG(__PRETTY_FUNCTION__);
        auto msg = get_message();
        switch (msg.type) {
        case Message_type::USER:
            if (status == Login_status_type::WAITING_USER) {
                if (check_username(msg.body)) {
                    status = Login_status_type::WATTING_PASS;
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
            if (status == Login_status_type::WATTING_PASS) {
                if (check_password(msg.body)) {
                    status = Login_status_type::LOGGED;
                    response(230, string_format("user %s logged in",username.c_str()));
                    async_wait_command();
                    // TODO load user info, such as root directory
                    user_init();
                    return;
                } else {
                    status = Login_status_type::WAITING_USER;
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
        con.async_wait_command([p](Message msg) {
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

    asio::ip::tcp::socket& get_data_socket() {
        LOG(__PRETTY_FUNCTION__);
        switch (port_type) {
        case User::Port_type::PORT:
            response(150, string_format("Connecting to port %d", data_endpoint.port()));
            data_socket = asio::ip::tcp::socket(context, asio::ip::tcp::v4());
            data_socket.set_option(asio::socket_base::reuse_address(true));
            data_socket.bind(*asio::ip::tcp::resolver(context).resolve(server_config::server_ip, server_config::server_data_port));
            data_socket.connect(data_endpoint);
            break;
        case User::Port_type::PASV:
            response(150, string_format("Connecting to port %d", data_endpoint.port()));
            break;
        }
        return data_socket;
    }

    void connect_pasv_data_socket(std::string port) {
        asio::ip::tcp::acceptor ac(context, *asio::ip::tcp::resolver(context).resolve(server_config::server_ip, port));
        data_socket = ac.accept();
    }
};