add_executable(trading_framework main.cpp)

target_link_libraries(trading_framework
    PRIVATE
        rpp
        rppgrpc
        utils
        interaction_controller
        connector_controller
)


