#pragma once

#include <rpp/observables/fwd.hpp>

#include <rpp/observables/dynamic_observable.hpp>

#include <chrono>
#include <string>
#include <variant>

namespace interaction::contract
{
    struct subscribe_to_ticker
    {
        std::string ticker{};
        struct response
        {
            std::string                           ticker{};
            std::chrono::system_clock::time_point time{};
            double                                price{};
        };
    };
    struct get_ticker_info
    {
        std::string ticker{};
        struct response
        {
            std::string ticker{};
            std::string description{};
        };
    };

    namespace details
    {
        template<rpp::constraint::decayed_type... Events>
        struct events_helper_impl
        {
            using event_payload    = std::variant<Events...>;
            using response_payload = std::variant<typename Events::response...>;
        };

        using events_helper = events_helper_impl<subscribe_to_ticker, get_ticker_info>;
    } // namespace details

    struct user_event
    {
        using payload_t = details::events_helper::event_payload;
        int64_t   user_id{};
        payload_t payload;
    };

    struct app_response
    {
        using payload_t = details::events_helper::response_payload;
        int64_t   user_id{};
        payload_t payload;
    };

    using user_events   = rpp::dynamic_observable<user_event>;
    using app_responses = rpp::dynamic_observable<app_response>;
} // namespace interaction::contract
