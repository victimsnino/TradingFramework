add_library(interaction_controller
    STATIC
        interaction_controller.cpp
        interaction_controller.hpp
)

target_include_directories(interaction_controller PUBLIC .)

target_link_libraries(interaction_controller
    PUBLIC
        interaction_contract
    PRIVATE
        interaction_telegram
)
