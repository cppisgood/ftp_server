#include <cstddef>
#include <filesystem>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include "tools.hpp"

/*
enum class file_type : signed char {
      none = 0, not_found = -1, regular = 1, directory = 2, symlink = 3,
      block = 4, character = 5, fifo = 6, socket = 7, unknown = 8
  };
*/
std::string get_path_type(std::filesystem::path path) {
    std::string ret = "type=";
    switch (std::filesystem::symlink_status(path).type()) {
    case std::filesystem::file_type::none: ret += "none"; break;
    case std::filesystem::file_type::not_found: ret += "not_found"; break;
    case std::filesystem::file_type::regular: ret += "regular"; break;
    case std::filesystem::file_type::directory: ret += "directory"; break;
    case std::filesystem::file_type::block: ret += "block"; break;
    case std::filesystem::file_type::character: ret += "character"; break;
    case std::filesystem::file_type::fifo: ret += "fifo"; break;
    case std::filesystem::file_type::socket: ret += "socket"; break;
    case std::filesystem::file_type::unknown: ret += "unknown"; break;
    case std::filesystem::file_type::symlink:
        ret += "symlink:";
        ret += std::filesystem::read_symlink(path);
    }
    return ret + ";";
}

std::string get_path_size(std::filesystem::path path) {
    std::string ret = "";
    switch (std::filesystem::status(path).type()) {
    case std::filesystem::file_type::regular: ret += "size=" + std::to_string(std::filesystem::file_size(path)); break;
    case std::filesystem::file_type::directory: ret += "sizd=4096"; break; // TODO
    default: ret += "size=4096";
    }
    return ret + ";";
}

std::string get_path_modify_time(std::filesystem::path path) { 
    auto time = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::file_clock::to_sys(std::filesystem::last_write_time(path)));
    auto raw_time = std::chrono::system_clock::to_time_t(time);
    auto timeinfo = std::localtime(&raw_time);
    std::string buf(14, '\0');
    std::strftime(buf.data(), sizeof(buf), "%Y%m%d%H%M%S", timeinfo);
    return "modify=" + buf + ";";
}

std::string get_path_modify_permission(std::filesystem::path path) {
    auto perm = std::filesystem::status(path).permissions();
    return string_format("UNIX.mode=%04d;", (int)perm);
}