#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <tools.hpp>
#include <vector>
#include "asio.hpp"
#include "asio/buffer.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/steady_timer.hpp"
#include "tools.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
// using namespace std::chrono_literals;

int main() {

    asio::io_context context;
    asio::error_code ec;

    asio::ip::tcp::endpoint ed(asio::ip::make_address("127.0.0.1"), 44444);
    asio::ip::tcp::socket socket1(context, asio::ip::tcp::v4());
    socket1.set_option(asio::socket_base::reuse_address(true));
    socket1.bind(ed);
    socket1 = asio::ip::tcp::socket(context, asio::ip::tcp::v4());
    // socket1.close();
    // socket1.bind(ed);
    // socket1.bind(ed);

    asio::ip::tcp::endpoint ed2(asio::ip::make_address("127.0.0.1"), 44444);
    asio::ip::tcp::socket socket2(context, asio::ip::tcp::v4());
    asio::socket_base::reuse_address option2(true);
    socket2.set_option(asio::socket_base::reuse_address(true), ec);
    socket2.bind(ed2);

    auto remote_ed1 = *asio::ip::tcp::resolver(context).resolve("127.0.0.1", "1234");
    auto remote_ed2 = *asio::ip::tcp::resolver(context).resolve("127.0.0.1", "1235");

    socket1.connect(remote_ed1);
    socket2.connect(remote_ed2);

    socket1.write_some(asio::buffer("s1"));
    socket2.write_some(asio::buffer("s2"));

    // socket.get_option(option);
    // socket.connect()
    // debug(option.value());

    // asio::ip::tcp::acceptor acceptor(context);
    // asio::socket_base::reuse_address option;
    // acceptor.get_option(option);
    // bool is_set = option.value();
    // debug(is_set);

    context.run();

    // asio::error_code ec;
    // asio::io_context context;
    // asio::ip::tcp::endpoint ed(asio::ip::make_address("127.0.0.1", ec), 6666);
    // asio::ip::tcp::endpoint ed2(asio::ip::make_address("127.0.0.1", ec), 6666);
    // asio::ip::tcp::socket socket(context);
    // asio::ip::tcp::socket socket2(context);
    // socket.open(asio::ip::tcp::v4());
    // socket2.open(asio::ip::tcp::v4());
    // debug(socket.is_open());
    // debug(socket2.is_open());
    // // if (ec) {
    // //     debug("??", ec.message());
    // // }
    // socket.bind(ed, ec);
    // if (ec) {
    //     debug(ec.message());
    // }
    // socket2.bind(ed2, ec);
    // if (ec) {
    //     debug(ec.message());
    // }


    // asio::ip::tcp::endpoint remote_ed(asio::ip::make_address("127.0.0.1", ec), 1234);

    // socket.connect(remote_ed, ec);
    // // if (ec) {
    // //     debug(ec.message());
    // // }
    // socket2.connect(remote_ed, ec);

    // socket.write_some(asio::buffer("1234"));
    // socket2.write_some(asio::buffer("5678"));


    // context.run();
    // debug(socket.local_endpoint().port(), socket2.local_endpoint().port());
    // // socket.bind(ed);

    // // std::array<char, 204800> buf;
    // // socket.connect(ed);
    // // auto len = socket.read_some(asio::buffer(buf), ec);
    // // // debug(len);
    // // LOG(len, std::string(buf.begin(), buf.begin() + len));
    // // std::this_thread::sleep_for(std::chrono::duration(5s));
}