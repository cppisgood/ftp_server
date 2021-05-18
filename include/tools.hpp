#pragma once
// #include <bits/stdc++.h>
#include <iostream>
#include <memory>
#include <chrono>
// #include "./unp.h"

#define __TERMINAL_COLOR_RED__ "\033[1;31m"
#define __TERMINAL_COLOR_RESET__ "\033[0m"


// template <const char* Color>
// void debug() {
//     std::cerr << std::endl;
// }

template <const char* Color, typename T, typename... Ts>
void _do_output(T t, Ts... ts) {
    std::cerr << Color << t << __TERMINAL_COLOR_RESET__;
    if constexpr (sizeof...(ts) == 0) {
        std::cerr << std::endl;
    } else {
        std::cerr << ' ';
        _do_output<Color>(ts...);
    }
}

template <typename... Ts>
void LOG(Ts... ts) {
    static const char Color_log[] = "\033[1;32m";
    _do_output<Color_log>(ts...);
}
template <typename... Ts>
void ERROR(Ts... ts) {
    static const char Color_error[] = "\033[1;31m";
    _do_output<Color_error>(ts...);
}
template <typename... Ts>
void debug(Ts... ts) {
    static const char Color_debug[] = "\033[1;34m";
    _do_output<Color_debug>(ts...);
}
template <typename... Ts>
void print(Ts... ts) {
    static const char Color_print[] = "";
    _do_output<Color_print>(ts...);
}


template<typename... Args>
std::string string_format( const std::string& format, Args ... args ) {
    int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}


template <typename F, typename... Ts, typename U = std::chrono::milliseconds>
auto timer(F&& f, Ts&&... params) {
    auto start = std::chrono::system_clock::now();
    auto ret = f(std::forward<Ts>(params)...);
    std::cout << std::chrono::duration_cast<U>(std::chrono::system_clock::now() - start).count() << std::endl;
    return ret;
}