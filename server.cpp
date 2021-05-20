#include <asio.hpp>
#include "tools.hpp"
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
#include <server.hpp>

int main() {
    asio::error_code ec;
    asio::io_context context;
    // Server s(context, "172.22.27.149", 21);
    Server s(context, "10.42.0.1", 21);
    LOG(s.ac.local_endpoint());
    s.start();
    const int thread_num = std::thread::hardware_concurrency();
    LOG(thread_num);
    std::vector<std::thread> threads(thread_num);
    for (auto& t : threads) t = std::thread([&context]() {context.run();});
    for (auto& t : threads) t.join();
    
}