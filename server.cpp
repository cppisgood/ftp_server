#include <asio.hpp>
#include "tools.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <server.hpp>

using namespace std::chrono_literals;

// void f(const asio::error_code& ec, asio::steady_timer* t, int* count) {
//     if (*count < 5) {
//         debug((*count)++);
//         // debug(t->expiry());
//         t->expires_at(t->expiry() + asio::chrono::seconds(1s));
//         t->async_wait(std::bind(f, std::placeholders::_1, t, count));
//     }
// }

// struct Printer {
//     asio::steady_timer t0;
//     asio::steady_timer t1;
//     asio::strand<asio::io_context::executor_type> strand;
//     int count = 0;

//     Printer(asio::io_context &context)
//         : t0(context, std::chrono::duration(1s)),
//           t1(context, std::chrono::duration(1s)),
//           strand(asio::make_strand(context)) {
//         t0.async_wait(asio::bind_executor(strand, std::bind(&Printer::f0, this)));
//         t1.async_wait(asio::bind_executor(strand, std::bind(&Printer::f1, this)));
//     }

//     void f0() {
//         if (count < 5) {
//             debug("f0", count++);
//             t0.expires_at(t0.expiry() + asio::chrono::seconds(1s));
//             t0.async_wait(asio::bind_executor(strand, std::bind(&Printer::f0, this)));
//         }
//     }
//     void f1() {
//         if (count < 5) {
//             debug("f1", count++);
//             t1.expires_at(t1.expiry() + asio::chrono::seconds(1s));
//             t1.async_wait(asio::bind_executor(strand, std::bind(&Printer::f1, this)));
//         }
//     }

// };
// #include <>
int main() {
    asio::error_code ec;
    asio::io_context context;

    // Printer p(context);
    // std::thread t([&context] {context.run();});
    // context.run();
    // t.join();
    // asio::steady_timer t(context, asio::chrono::seconds(1s));
    // int count = 0;
    // t.async_wait(std::bind(f, std::placeholders::_1, &t, &count));
    // context.run();
    // std::this_thread::sleep_for(std::chrono::duration(5s));
    // debug("final:", count);
    // asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec),
    // 8089); asio::ip::tcp::socket socket(context); socket.connect(endpoint,
    // ec); if (!ec) {
    //     std::cout << "connected!\n";
    // } else {
    //     std::cout << "failed to connect! error: " << ec.message() <<
    //     std::endl;
    // }
    // return 0;

    // auto get_time = [] {
    //     auto now = std::time(0);
    // };

    // asio::ip::tcp::endpoint ed(asio::ip::tcp::v4(), 21);
    // asio::ip::tcp::acceptor ac(context, ed);
    // ac.listen();
    // while (true) {
    //     // std::string buf = std::to_string(std::time(0));
    //     // std::string buf = "a";
    //     auto socket = ac.accept();
    //     socket.write_some(asio::buffer("a"));
    //     socket.write_some(asio::buffer("b"));
    //     // debug(buf);
    // }
    // asio::ip::tcp::resolver res(context);
    // auto ed = *res.resolve("127.0.0.1", "80");
    Server s(context, "127.0.0.1", 21);
    s.start();
    const int thread_num = std::thread::hardware_concurrency();
    LOG(thread_num);
    std::vector<std::thread> threads(thread_num);
    for (auto& t : threads) t = std::thread([&context]() {context.run();});
    for (auto& t : threads) t.join();
}