#include "components/logger.hpp"
#include "core/bot.h"
#include "poker/bank.h"
#include "poker/bot.h"
#include "poker/card.h"
#include "poker/deck.h"
#include "poker/game.h"

#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>
#include <csignal>
#include <iostream>

void my_signal_handler(int signum) {
    ::signal(signum, SIG_DFL);
    std::cerr << "signal called:" << std::endl << boost::stacktrace::stacktrace() << std::endl;
    std::cerr << "errno:" << ::strerror(errno) << std::endl;
    ::raise(SIGABRT);
}

void setup_handlers() {
    ::signal(SIGSEGV, &my_signal_handler);
    ::signal(SIGABRT, &my_signal_handler);
}

static auto original_terminate_handler {std::get_terminate()};

void terminate_handler() {
    std::cerr << "terminate called:" << std::endl << boost::stacktrace::stacktrace() << std::endl;
    std::cerr << "errno:" << ::strerror(errno) << std::endl;
    original_terminate_handler();
    std::abort();
}

int main(int argc, char* argv[]) {
    setup_handlers();
    std::set_terminate(terminate_handler);

    auto lgr = initialization_logger();
    lgr.set_level(logger::level::info);
    auto internal = lgr.get_internal_logger();
    internal->set_pattern("[%Y-%m-%d %T] [%L] %v");

    //parse options
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "this message");
    desc.add_options()("token", po::value<std::string>(), "token for tg bot");
    desc.add_options()("verbose", po::value<std::uint64_t>(),
                       "level of verbosity.\n"
                       "0 - trace\n"
                       "1 - debug\n"
                       "2 - info\n"
                       "3 - warn\n"
                       "4 - err\n"
                       "5 - critical\n"
                       "6 - off\n");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help")) {
        std::stringstream ss;
        ss << desc;
        std::string mes = ss.str();
        lgr.info(mes);
        return 0;
    }
    if(vm.count("verbose")) {
        int verb = vm["verbose"].as<std::uint64_t>();
        int max  = (int)logger::level::level_nums;
        if(verb >= max) {
            auto mes = fmt::format("max alloved verbocity level:{0}", max);
            lgr.error(mes);
            throw std::runtime_error(mes);
        }
        lgr.set_level(static_cast<logger::level>(verb));
    }

    std::string token;
    if(vm.count("token")) {
        token = vm["token"].as<std::string>();
    } else {
        auto mes = "param [token] is mandatory! see --help";
        lgr.error(mes);
        throw std::runtime_error(mes);
    }

    poker::poker_bot b(token);
    b.start();

    return 0;
}