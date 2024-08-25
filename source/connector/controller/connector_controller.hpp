#include <connector_contract.hpp>

#include <optional>

namespace connector::controller
{
    struct data
    {
        std::optional<std::string> tinkoff_token{};
    };

    std::shared_ptr<contract::interface> get_connector(const data& data);
} // namespace connector::controller
