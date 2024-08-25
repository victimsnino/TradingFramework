#include "connector_controller.hpp"

#include <connector_tinkoff.hpp>

namespace connector::controller
{
    std::shared_ptr<contract::interface> get_connector(const data& data)
    {
        if (data.tinkoff_token)
            return tinkoff::get_connector(data.tinkoff_token.value());
        return {};
    }
} // namespace connector::controller
