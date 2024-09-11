#pragma once

#include <interaction_contract.hpp>

namespace TgBot // NOLINT
{
    class HttpClient;
} // namespace TgBot

namespace interaction::telegram::impl
{
    contract::user_events get_users(const std::string& token, const contract::app_responses& app_responses, const TgBot::HttpClient* http_client = {});

} // namespace interaction::telegram::impl
