#pragma once
#include <memory>
#include <tgbot/tgbot.h>

class user;
class room;

using user_ptr = std::shared_ptr<user>;
using room_ptr = std::shared_ptr<room>;
using mes_ptr = TgBot::Message::Ptr;