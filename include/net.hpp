#pragma once

#include "asio.hpp"
#include "tools.hpp"


// enum parse command_switch command_handler
enum class Message_type {
    USER,
    PASS,
    SYST,
    QUIT,
    PORT,
    LIST,
    CWD,
    TYPE,
    RETR,
};
struct Message {
    Message_type type;
    // std::string header;
    std::string body;

    Message() {}
    Message(Message_type type_) : type(type_) {}
 
    friend Message& operator<<(Message& msg, const std::string& data) {
        msg.body += data;
        return msg;
    }
};


Message parse(const std::string raw_msg) {
    LOG(__PRETTY_FUNCTION__);
    Message msg;
    std::size_t st = 0;
    while (st < raw_msg.size() && std::isspace(raw_msg[st])) ++st;
    std::size_t i = st;
    while (i < raw_msg.size() && !std::isspace(raw_msg[i])) ++i;
    if (i - st > 4) {
        ERROR(string_format("[error] command type too long. raw message: %s", raw_msg.c_str()));
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
    else if (type == "CWD") msg.type = Message_type::CWD;
    else if (type == "TYPE") msg.type = Message_type::TYPE;
    else if (type == "RETR") msg.type = Message_type::RETR;
    else {
        ERROR(string_format("[error] %s unrecognised type: %s", type.c_str()));
        // TODO
    }
    print("[parse]");
    print((int)msg.type);
    print(msg.body);
    print(raw_msg);
    print("[parse]");
    return msg;
}

class Connection {
public:
    asio::io_context& context;
    asio::ip::tcp::socket socket;
    std::string buf;
    Connection(asio::io_context& context_) :
        context(context_),
        socket(context_),
        buf(1024, '\0') {
    }

    Message read() {
        LOG(__PRETTY_FUNCTION__);
        Message msg;
        std::string buf(128, '\0');
        socket.read_some(asio::buffer(buf));
        return parse(buf);
    }

    void write(const std::string msg) {
        LOG(__PRETTY_FUNCTION__);
        socket.write_some(asio::buffer(msg));
    }

    asio::ip::tcp::socket& get_socket() {
        return socket;
    }

    template <typename Func>
    void async_wait_command(Func&& f) {
        LOG(__PRETTY_FUNCTION__);
        socket.async_read_some(asio::buffer(buf), [this, f](asio::error_code ec, std::size_t size) {
            if (ec) {
                debug(ec.message());
                return;
            }
            f(parse(buf));
            // TODO more light way ?
            buf = std::string(1024, '\0');
            async_wait_command(f);
        });
    }

};