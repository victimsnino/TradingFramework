#pragma once

#include <connector_contract.hpp>

namespace connector::tinkoff
{
    std::shared_ptr<contract::interface> get_connector(const std::string& token);
} // namespace connector::tinkoff
