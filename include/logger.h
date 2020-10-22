#pragma once
#include <iostream>
#include <chrono>
#include <sstream>
#include <optional>

class logger{
    std::stringstream m_ss;

    decltype(std::chrono::steady_clock::now()) m_time_point =
        std::chrono::steady_clock::now();
public:
    size_t buf_len = 1024;
    size_t flush_ms = 10;

    template<class T>
    void log(const T& data) { 
        auto now = std::chrono::steady_clock::now();
        auto delta = now - m_time_point;
        auto delta_ms = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();

        auto& ss = m_ss;
        ss << data;
        if(ss.str().size() >= buf_len ||
            delta_ms >= flush_ms)
        {
            std::cout << ss.str();
            ss.str("");
            m_time_point = now;
        }
    }

    friend std::ostream& operator<<(logger& lgr, std::ostream& str) { 
        auto now = std::chrono::steady_clock::now();
        auto delta = now - lgr.m_time_point;
        auto delta_ms = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();

        auto& ss = lgr.m_ss;
        ss << str.rdbuf();
        if(ss.str().size() >= lgr.buf_len ||
            delta_ms >= lgr.flush_ms)
        {
            std::cout << ss.str();
            ss.str("");
            lgr.m_time_point = now;
        }
        return str;
    }

    static auto& get_logger(){
        static std::optional<logger> lgr;
        if(!lgr.has_value()){
            lgr.emplace();
        }
        return lgr.value();
    }
};