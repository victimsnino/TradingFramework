

#include <rpp/operators/as_blocking.hpp>
#include <rpp/operators/subscribe.hpp>
#include <rpp/sources/never.hpp>
#include <rpp/subjects/publish_subject.hpp>

#include <interaction_telegram.hpp>
#include <utils.hpp>


int main(int argc, char** argv)
{
    ENSURE(argc == 2);

    const auto subj       = rpp::subjects::publish_subject<interaction::contract::app_response>();
    const auto bot_events = interaction::telegram::get_users(argv[1], subj.get_observable());
    bot_events | rpp::ops::as_blocking() | rpp::ops::subscribe([&subj](const interaction::contract::user_event& event) {
        std::visit(
            utils::overloaded{
                [&subj, &event](const interaction::contract::get_ticker_info& ticker_info) {
                    subj.get_observer().on_next(interaction::contract::app_response{.user_id = event.user_id,
                                                                                    .payload = interaction::contract::get_ticker_info::response{.description = ticker_info.ticker + " info response"}});
                },
                [](const auto&) {
                }},
            event.payload);
    });

    return 0;
}
