#pragma once
#include <filesystem>
#include <asio.hpp>
#include <string>

namespace server_config {
    static std::filesystem::path work_path("/home/zmm/Documents/code/cpp/ftp_server/work");
    static std::filesystem::path default_ftp_path("/home/zmm/Documents/code/cpp/ftp_server/ftp");
    static std::string server_ip("127.0.0.1");
    static std::string server_data_port("20");
    static std::string server_port("21");
}