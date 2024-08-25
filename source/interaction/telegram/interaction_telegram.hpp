#pragma once

#include <interaction_contract.hpp>

namespace interaction::telegram
{
    contract::user_events get_users(const std::string& token, const contract::app_responses& app_responses);
    
} // namespace interaction::telegram