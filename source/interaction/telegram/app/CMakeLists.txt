add_executable(interaction_telegram_mock_app main.cpp)

target_link_libraries(interaction_telegram_mock_app 
    PRIVATE 
        interaction_telegram
        utils
)

