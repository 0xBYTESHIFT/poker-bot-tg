#include <iostream>
#include "poker_bot.h"


int main(int argc, char* argv[]) {
    if(argc != 2){
        throw std::runtime_error("provide telegram token!");
    }
    const std::string token(argv[1]);
    poker_bot bot(token);

    try {
        bot.start();
    } catch (TgBot::TgException& e) {
        printf("error tg api: %s\n", e.what());
    } catch (std::exception& e){
        printf("error stl: %s\n", e.what());
    } catch (...){
        printf("we have some bad error here\n");
    }
    return 0;
}