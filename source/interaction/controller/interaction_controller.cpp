#include "interaction_controller.hpp"

#include <rpp/sources/never.hpp>

#include <interaction_telegram.hpp>

namespace interaction::controller
{
    contract::user_events get_users(const data& data, const contract::app_responses& app_responses)
    {
        if (data.telegram_token)
            return telegram::get_users(data.telegram_token.value(), app_responses);

        return rpp::source::never<contract::user_event>();
    }

} // namespace interaction::controller
