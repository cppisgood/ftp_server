#define ASIO_STANDALONE
#include <iostream>
// #include "asio/error_code.hpp"
#include "asio.hpp"
// #include "asio/ts/buffer.hpp"
// #include "asio/ts/internet.hpp"
#include <vector>

int main() {
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 8089);
    asio::ip::tcp::socket socket(context);
    socket.connect(endpoint, ec);
    if (!ec) {
        std::cout << "connected!\n";
    } else {
        std::cout << "failed to connect! error: " << ec.message() << std::endl;
    }
    return 0;
}