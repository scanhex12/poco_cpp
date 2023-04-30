#include "bot.h"
#include <iostream>
#include <random>
#include <string>

class BotRunner {
public:
    BotRunner() : bot_(TelegramBot()) {
    }

    void Step() {
        bot_.GetUpdates(GetUrl(), bot_.GetMaxUpdateId() + 1, std::nullopt);
        auto message = bot_.GetUpdates(GetUrl(), std::nullopt, 60);
        if (!message) {
            return;
        }
        if (message == "/weather") {
            bot_.SendMessage(GetUrl(), "Winter Is Coming", std::nullopt);
        }
        if (message == "/random") {
            int random_number = rand();
            bot_.SendMessage(GetUrl(), std::to_string(random_number), std::nullopt);
        }
        if (message == "/styleguide") {
            bot_.SendMessage(GetUrl(), "Я не придумал шутки(", std::nullopt);
        }
        if (message == "/stop") {
            is_alive_ = false;
        }
        if (message == "/crash") {
            abort();
        }
    }

    void Run() {
        while (is_alive_) {
            Step();
        }
    }

private:
    std::string GetUrl() const {
        return " http://api.telegram.org/";
    }

    TelegramBot bot_;
    bool is_alive_ = true;
};