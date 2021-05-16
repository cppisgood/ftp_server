#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "asio.hpp"
#include "asio/buffer.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/steady_timer.hpp"
#include "tools.hpp"

// using namespace std::chrono_literals;

int main() {
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::endpoint ed(asio::ip::make_address("127.0.0.1", ec), 6666);
    asio::ip::tcp::endpoint ed2(asio::ip::make_address("127.0.0.1", ec), 6666);
    asio::ip::tcp::socket socket(context);
    asio::ip::tcp::socket socket2(context);
    socket.open(asio::ip::tcp::v4());
    socket2.open(asio::ip::tcp::v4());
    debug(socket.is_open());
    debug(socket2.is_open());
    // if (ec) {
    //     debug("??", ec.message());
    // }
    socket.bind(ed, ec);
    if (ec) {
        debug(ec.message());
    }
    socket2.bind(ed2, ec);
    if (ec) {
        debug(ec.message());
    }


    asio::ip::tcp::endpoint remote_ed(asio::ip::make_address("127.0.0.1", ec), 1234);

    socket.connect(remote_ed, ec);
    // if (ec) {
    //     debug(ec.message());
    // }
    socket2.connect(remote_ed, ec);

    socket.write_some(asio::buffer("1234"));
    socket2.write_some(asio::buffer("5678"));


    context.run();
    debug(socket.local_endpoint().port(), socket2.local_endpoint().port());
    // socket.bind(ed);

    // std::array<char, 204800> buf;
    // socket.connect(ed);
    // auto len = socket.read_some(asio::buffer(buf), ec);
    // // debug(len);
    // LOG(len, std::string(buf.begin(), buf.begin() + len));
    // std::this_thread::sleep_for(std::chrono::duration(5s));
}