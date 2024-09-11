add_executable(interaction_telegram_ut interaction_telegram_ut.cpp)
target_link_libraries(interaction_telegram_ut
    PRIVATE
        interaction_telegram_impl
        Catch2::Catch2WithMain
        tgbot::tgbot
        trompeloeil::trompeloeil
)

catch_discover_tests(interaction_telegram_ut)
