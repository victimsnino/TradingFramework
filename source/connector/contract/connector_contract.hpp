#pragma once

#include <rpp/observables/fwd.hpp>

#include <utils.hpp>

#include <string>

namespace connector::contract
{
    struct ticker_info
    {
        std::string description{};
    };

    struct ticker_event
    {
        std::string info{};
        // std::string ticker{};
        // std::chrono::system_clock::time_point time{};
        // double                                price{};
    };

    struct interface
    {
        virtual ~interface() = default;

        virtual utils::expected<ticker_info, std::string> get_ticker_info(const std::string& ticker) const   = 0;
        virtual rpp::dynamic_observable<ticker_event>     get_ticker_events(const std::string& ticker) const = 0;
    };
} // namespace connector::contract
