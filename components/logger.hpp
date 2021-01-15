#pragma once

#include <boost/process/environment.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

class logger final {
public:
    enum class level {
        trace    = spdlog::level::trace,
        debug    = spdlog::level::debug,
        info     = spdlog::level::info,
        warn     = spdlog::level::warn,
        err      = spdlog::level::err,
        critical = spdlog::level::critical,
        off      = spdlog::level::off,
        level_nums
    };

    logger() = default;
    logger(std::shared_ptr<spdlog::async_logger>);
    logger(std::shared_ptr<spdlog::logger>);
    ~logger() = default;

    auto clone() noexcept -> logger;
    auto set_level(level l) -> void;
    auto get_level() const -> logger::level;
    auto context(std::shared_ptr<spdlog::async_logger> logger) noexcept -> void;
    auto get_internal_logger() const -> std::shared_ptr<const spdlog::logger>;
    auto get_internal_logger() -> std::shared_ptr<spdlog::logger>;

    template<typename MSGBuilder>
    auto log(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->log(static_cast<spdlog::level::level_enum>(lvl_), std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto operator<<(MSGBuilder&& msg_builder) noexcept -> logger& {
        log(std::forward<MSGBuilder>(msg_builder));
        return *this;
    }

    template<typename MSGBuilder>
    auto trace(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->trace(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto info(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->info(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto warn(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->warn(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto error(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->error(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto debug(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->debug(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename MSGBuilder>
    auto critical(MSGBuilder&& msg_builder) noexcept -> void {
        logger_->critical(std::forward<MSGBuilder>(msg_builder));
    }

    template<typename S, typename... Args>
    auto trace(const S& format_str, Args&&... args) -> void {
        logger_->trace(fmt::format(format_str, std::forward<Args>(args)...));
    }

    template<typename S, typename... Args>
    auto info(const S& format_str, Args&&... args) -> void {
        logger_->info(fmt::format(format_str, std::forward<Args>(args)...));
    }

    template<typename S, typename... Args>
    auto debug(const S& format_str, Args&&... args) -> void {
        logger_->debug(fmt::format(format_str, std::forward<Args>(args)...));
    }

    template<typename S, typename... Args>
    auto warn(const S& format_str, Args&&... args) -> void {
        logger_->warn(fmt::format(format_str, std::forward<Args>(args)...));
    }

    template<typename S, typename... Args>
    auto error(const S& format_str, Args&&... args) -> void {
        logger_->error(fmt::format(format_str, std::forward<Args>(args)...));
    }

    template<typename S, typename... Args>
    auto critical(const S& format_str, Args&&... args) -> void {
        logger_->critical(fmt::format(format_str, std::forward<Args>(args)...));
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
    level lvl_ = level::info;
};

auto get_logger(const std::string&) -> logger;
auto get_logger() -> logger;
auto initialization_logger(std::shared_ptr<spdlog::logger>) -> void;
auto initialization_logger(const std::string& prefix = "logs/") -> logger;

logger::logger(std::shared_ptr<spdlog::async_logger> logger): logger_(std::move(logger)) { }

logger::logger(std::shared_ptr<spdlog::logger> logger): logger_(std::move(logger)) { }

auto logger::clone() noexcept -> logger {
    return logger_;
}

auto logger::set_level(logger::level l) -> void {
    auto cast = static_cast<spdlog::level::level_enum>(l);
    logger_->set_level(cast);
    this->lvl_ = l;
}
auto logger::get_level() const -> logger::level {
    auto lvl = logger_->level();
    return static_cast<logger::level>(lvl);
}

auto logger::context(std::shared_ptr<spdlog::async_logger> logger) noexcept -> void {
    logger_ = std::move(logger);
}
auto logger::get_internal_logger() const -> std::shared_ptr<const spdlog::logger>{
    return this->logger_;
}
auto logger::get_internal_logger() -> std::shared_ptr<spdlog::logger>{
    return this->logger_;
}

auto initialization_logger(const std::string& prefix) -> logger {
    //prefix == "logs/", no need for slash here
    auto name = fmt::format(prefix + "{0}.txt", boost::this_process::get_id());
    ///spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(name, true);
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>( ///async_logger
        "default", sinks.begin(), sinks.end()       //,
        /*spdlog::thread_pool(),*/
        /*spdlog::async_overflow_policy::block*/);

    spdlog::flush_every(std::chrono::seconds(1)); //todo: hack
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [pid %P tid %t] %v");
    spdlog::set_default_logger(logger); /// spdlog::register_logger(logger);
    return logger;
}

auto get_logger(const std::string& name) -> logger {
    return spdlog::get(name);
}

auto get_logger() -> logger {
    return spdlog::get("default");
}

auto initialization_logger(std::shared_ptr<spdlog::logger> logger) -> void {
    spdlog::register_logger(std::move(logger));
}