add_library(connector_controller
    STATIC
        connector_controller.cpp
        connector_controller.hpp
)

target_include_directories(connector_controller PUBLIC .)

target_link_libraries(connector_controller
    PUBLIC
        connector_contract
    PRIVATE
        connector_tinkoff
)

