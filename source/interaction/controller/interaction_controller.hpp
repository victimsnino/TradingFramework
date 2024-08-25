#pragma once

#include <interaction_contract.hpp>

#include <optional>

namespace interaction::controller
{
    struct data
    {
        std::optional<std::string> telegram_token{};
    };
    contract::user_events get_users(const data& data, const contract::app_responses& app_responses);

} // namespace interaction::controller
