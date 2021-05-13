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

using namespace std::chrono_literals;

int main() {
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::endpoint ed(asio::ip::make_address("127.0.0.1", ec), 6666);
    asio::ip::tcp::socket socket(context);
    std::array<char, 204800> buf;
    socket.connect(ed);
    auto len = socket.read_some(asio::buffer(buf), ec);
    // debug(len);
    debug(len, std::string(buf.begin(), buf.begin() + len));
    std::this_thread::sleep_for(std::chrono::duration(5s));
}