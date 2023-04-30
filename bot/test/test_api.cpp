#include <fake/fake.h>

#include <catch.hpp>
#include <telegram/bot.h>

TEST_CASE("Single getMe") {
    telegram::FakeServer fake{"Single getMe"};
    fake.Start();
    // Your code here. Doing requests to fake.GetUrl();
    TelegramBot bot("123");

    bot.GetMe(fake.GetUrl());
    fake.StopAndCheckExpectations();
}

TEST_CASE("getMe error handling") {
    telegram::FakeServer fake{"getMe error handling"};
    fake.Start();

    // Your code here

    TelegramBot bot("123");
    REQUIRE_THROWS_AS(bot.GetMe(fake.GetUrl()), std::runtime_error);
    REQUIRE_THROWS_AS(bot.GetMe(fake.GetUrl()), std::runtime_error);

    fake.StopAndCheckExpectations();
}

TEST_CASE("Single getUpdates and send messages") {
    telegram::FakeServer fake{"Single getUpdates and send messages"};
    fake.Start();

    // Your code here
    TelegramBot bot("123");
    bot.GetUpdates(fake.GetUrl());
    bot.SendMessage(fake.GetUrl(), "Hi!", std::nullopt);
    bot.SendMessage(fake.GetUrl(), "Reply", 2);
    bot.SendMessage(fake.GetUrl(), "Reply", 2);

    fake.StopAndCheckExpectations();
}

TEST_CASE("Handle getUpdates offset") {
    telegram::FakeServer fake{"Handle getUpdates offset"};
    fake.Start();

    TelegramBot bot("123");

    bot.GetUpdates(fake.GetUrl(), std::nullopt, 5);
    bot.GetUpdates(fake.GetUrl(), bot.GetMaxUpdateId() + 1, 5);
    bot.GetUpdates(fake.GetUrl(), bot.GetMaxUpdateId() + 1, 5);

    fake.StopAndCheckExpectations();
}
