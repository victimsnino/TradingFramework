#include "interaction_telegram.hpp"

#include "interaction_telegram_impl.hpp"

namespace interaction::telegram
{
    contract::user_events get_users(const std::string& token, const contract::app_responses& app_responses)
    {
        return impl::get_users(token, app_responses);
    }

} // namespace interaction::telegram
