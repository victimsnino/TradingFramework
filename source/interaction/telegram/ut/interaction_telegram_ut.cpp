#include <catch2/catch_test_macros.hpp>

#include <rpp/sources/never.hpp>

#include <interaction_telegram_impl.hpp>
#include <tgbot/net/HttpClient.h>
#include <trompeloeil.hpp>
#include <trompeloeil/mock.hpp>

#include <iostream>
#include <thread>

using namespace interaction::telegram;

namespace
{

    struct MockClient final : public trompeloeil::mock_interface<TgBot::HttpClient>
    {
        IMPLEMENT_CONST_MOCK2(makeRequest);

        int getRequestMaxRetries() const override { return 0; }
        int getRequestBackoff() const override { return 0; }
    };

    std::ostream& operator<<(std::ostream& os, const TgBot::Url& url)
    {
        return os << url.protocol << "://www." << url.host << url.path << "?" << url.query << url.fragment;
    }

} // namespace
TEST_CASE("get_users")
{
    MockClient mock{};
    // ALLOW_CALL(mock, makeRequest(trompeloeil::_, trompeloeil::_)).SIDE_EFFECT({std::cout << _1 << std::endl; }).RETURN("");
    // interaction::telegram::impl::get_users("", rpp::source::never<interaction::contract::app_response>(), &mock).subscribe([](const auto&){});

    // std::this_thread::sleep_for(std::chrono::seconds(1));
}
