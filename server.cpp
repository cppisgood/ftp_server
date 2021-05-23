#include <asio.hpp>
#include "tools.hpp"
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
#include <server.hpp>

int main(int argc, char* argv[]) {
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::socket socket(context);
    std::string ip;
    std::string port;
    if (argc == 2) {
        ip = argv[1];
    } else if (argc == 3) {
        ip = argv[1];
        port = argv[2];
    } else {
        debug("usage: ftp_server <ip> [port]");
        return 0;
    }
    LOG(ip, port);
    Server s(context, ip, port);
    s.start();
    const int thread_num = std::thread::hardware_concurrency();
    LOG(thread_num);
    std::vector<std::thread> threads(thread_num);
    for (auto& t : threads) t = std::thread([&context]() {context.run();});
    for (auto& t : threads) t.join();
    
}