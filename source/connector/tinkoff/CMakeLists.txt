add_library(connector_tinkoff
    STATIC
        connector_tinkoff.cpp
        connector_tinkoff.hpp
)
target_include_directories(connector_tinkoff PUBLIC .)

target_link_libraries(connector_tinkoff
    PRIVATE
        investapi_proto
        rppgrpc
    PUBLIC
        utils
        rpp
        connector_contract
)
