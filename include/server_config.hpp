#pragma once
#include <filesystem>
#include <asio.hpp>
#include <string>

namespace server_config {
    std::filesystem::path work_path("/home/zmm/Documents/code/cpp/ftp_server/work");
    std::filesystem::path default_ftp_path("/home/zmm/Documents/code/cpp/ftp_server/ftp");
    std::string server_ip("127.0.0.1");
    std::string server_data_port("20");
}