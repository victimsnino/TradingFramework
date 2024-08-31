add_library(interaction_telegram_impl
    STATIC
        interaction_telegram_impl.cpp
        interaction_telegram_impl.hpp
)

target_include_directories(interaction_telegram_impl PUBLIC .)

target_link_libraries(interaction_telegram_impl
    PRIVATE
        rpp
        tgbot::tgbot
        utils
        fmt::fmt
    PUBLIC
        interaction_contract
)
