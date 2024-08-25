add_library(interaction_telegram
    STATIC
        interaction_telegram.cpp
        interaction_telegram.hpp
)

target_include_directories(interaction_telegram PUBLIC .)

target_link_libraries(interaction_telegram
    PRIVATE
        rpp
        tgbot::tgbot
        utils
        fmt::fmt
    PUBLIC
        interaction_contract
)

add_subdirectory(app)
