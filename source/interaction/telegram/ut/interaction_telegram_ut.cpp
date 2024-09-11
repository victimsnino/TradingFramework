#include <catch2/catch_test_macros.hpp>

#include <rpp/operators/subscribe.hpp>
#include <rpp/rpp.hpp>

#include <interaction_telegram_impl.hpp>
#include <tgbot/net/HttpClient.h>
#include <trompeloeil.hpp>
#include <trompeloeil/mock.hpp>

#include <iostream>
#include <thread>

using namespace interaction::telegram;

namespace trompeloeil
{
    template<>
    struct printer<TgBot::Url>
    {
        static void print(std::ostream& os, const TgBot::Url& url)
        {
            os << url.protocol << "://www." << url.host << url.path << "?" << url.query << url.fragment;
        }
    };

    template<>
    struct printer<std::vector<TgBot::HttpReqArg>>
    {
        static void print(std::ostream& os, const std::vector<TgBot::HttpReqArg>& args)
        {
            os << "{\n";
            for (const auto& arg : args)
            {
                os << "{" << arg.name << " : " << arg.value << "},\n";
            }
            os << "}";
        }
    };
} // namespace trompeloeil

namespace
{
    struct mock_client final : public trompeloeil::mock_interface<TgBot::HttpClient>
    {
        IMPLEMENT_CONST_MOCK2(makeRequest);

        int getRequestMaxRetries() const override { return 0; }
        int getRequestBackoff() const override { return 0; }
    };

    std::string generate_request(const std::string& body = "{}")
    {
        return "{\"ok\": true, \"result\":" + body + "}";
    }

    void wait(const std::unique_ptr<trompeloeil::expectation>& e)
    {
        while (!e->is_satisfied())
        {
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
    }

} // namespace

TEST_CASE("interaction_telegram")
{
    mock_client           mock{};
    trompeloeil::sequence seq;
    ALLOW_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_)).WITH(_1.path == "/bot/getMyCommands").RETURN(generate_request());
    ALLOW_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_)).WITH(_1.path == "/bot/setMyCommands").RETURN(generate_request());
    ALLOW_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_)).WITH(_1.path == "/bot/getUpdates").RETURN(generate_request("[]"));

    SECTION("/start")
    {
        REQUIRE_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_))
            .WITH(_1.path == "/bot/getUpdates")
            .RETURN(generate_request(R"([{"message": {"chat":{"id": 1}, "text": "/start"}}])"))
            .IN_SEQUENCE(seq);

        const auto call = NAMED_REQUIRE_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_))
                              .WITH(_1.path == "/bot/sendMessage"
                                    && std::ranges::find_if(_2, [](const TgBot::HttpReqArg& arg) { return arg.name == "chat_id" && arg.value == "1"; }) != _2.end()
                                    && std::ranges::find_if(_2, [](const TgBot::HttpReqArg& arg) { return arg.name == "text" && arg.value.starts_with("Hello! It is TradingFramework!"); }) != _2.end())
                              .RETURN(generate_request(R"({"chat":{"id": 1}})"))
                              .IN_SEQUENCE(seq);


        const auto d = interaction::telegram::impl::get_users("", rpp::source::never<interaction::contract::app_response>(), &mock) | rpp::ops::subscribe_with_disposable([](const auto&) {});

        wait(call);
        d.dispose();
    }
}
