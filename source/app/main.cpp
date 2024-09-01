
#include <rpp/rpp.hpp>

#include <connector_controller.hpp>
#include <interaction_controller.hpp>
#include <utils.hpp>

#include <variant>

namespace
{
    auto get_visitor(const std::shared_ptr<connector::contract::interface>& connector)
    {
        return [connector](const auto& r) -> rpp::dynamic_observable<interaction::contract::app_response::payload_t> {
            return utils::overloaded{[connector](const interaction::contract::get_ticker_info& ticker_info) {
                                         const auto r = connector->get_ticker_info(ticker_info.ticker);
                                         return rpp::source::just(interaction::contract::app_response::payload_t{interaction::contract::get_ticker_info::response{.ticker = ticker_info.ticker, .description = r ? r.get().description : r.error()}});
                                     },
                                     [connector](const interaction::contract::subscribe_to_ticker& event) {
                                         return connector->get_ticker_events(event.ticker) | rpp::ops::map([ticker = event.ticker](const connector::contract::ticker_event& r) -> interaction::contract::app_response::payload_t {
                                                    return interaction::contract::subscribe_to_ticker::response{.ticker = ticker, .price = r.price, .time = r.time};
                                                });
                                     }}(r);
        };
    }
} // namespace


int main(int argc, char** argv)
{
    ENSURE(argc == 3);
    const std::string                                                   invest_token   = argv[1];
    const std::string                                                   telegram_token = argv[2];
    rpp::subjects::publish_subject<interaction::contract::app_response> responses{};
    const auto                                                          bot_events = interaction::controller::get_users({.telegram_token = telegram_token}, responses.get_observable());
    const auto                                                          connector  = connector::controller::get_connector({.tinkoff_token = invest_token});

    bot_events
        | rpp::ops::group_by([](const interaction::contract::user_event& event) {
              return event.user_id;
          })
        | rpp::ops::flat_map([connector](const rpp::grouped_observable_group_by<std::int64_t, interaction::contract::user_event>& obs) {
              return obs
                   | rpp::ops::flat_map([connector, key = obs.get_key()](const interaction::contract::user_event& event) {
                         return std::visit(get_visitor(connector), event.payload)
                              | rpp::ops::map([key](const auto& r) { return interaction::contract::app_response{.user_id = key, .payload = r}; });
                     });
              //   return obs
              //        | rpp::ops::filter([](const interaction::contract::user_event& event) {
              //              return std::holds_alternative<interaction::contract::get_ticker_info>(event.payload);
              //          })
              //        | rpp::ops::map([](const interaction::contract::user_event& event) {
              //              return std::get<interaction::contract::get_ticker_info>(event.payload);
              //          })
              //        | rpp::ops::map([connector, key = obs.get_key()](const interaction::contract::get_ticker_info& ticker_info) {
              //              const auto r = connector->get_ticker_info(ticker_info.ticker);
              //              return interaction::contract::app_response{.user_id = key, .payload = interaction::contract::get_ticker_info::response{.ticker = ticker_info.ticker, .description = r ? r.get().description : r.error()}};
              //          });
          })
        | rpp::ops::subscribe(responses.get_observer());


    while (!responses.get_observer().is_disposed())
    {
        std::this_thread::yield();
    }

    return 0;
}
