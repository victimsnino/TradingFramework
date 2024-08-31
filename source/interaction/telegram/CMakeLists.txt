add_library(interaction_telegram
    STATIC
        interaction_telegram.cpp
        interaction_telegram.hpp
)

target_include_directories(interaction_telegram PUBLIC .)

target_link_libraries(interaction_telegram
    PRIVATE
        interaction_telegram_impl
    PUBLIC
        interaction_contract
)

add_subdirectory(impl)
add_tests_subdirectory(ut)
