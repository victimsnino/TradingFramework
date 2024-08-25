#pragma once

#include <stdexcept>
#include <variant>

namespace utils
{

#define ENSURE(expr)                                                     \
    if (!(expr))                                                         \
    {                                                                    \
        throw std::runtime_error("failed to satisfy condition: " #expr); \
    }

    template<typename T, typename Type>
    concept decayed_same_as = std::same_as<std::decay_t<T>, std::decay_t<Type>>;

    template<typename T>
    concept decayed_type = std::same_as<std::decay_t<T>, T>;

    template<class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;


    template<decayed_type T, decayed_type E>
    class expected
    {
    public:
        template<decayed_same_as<T> TT>
            requires (!decayed_same_as<TT, expected<T, E>>)
        expected(TT&& v) // NOLINT
            : m_data(std::forward<TT>(v))
        {
        }
        template<decayed_same_as<E> EE>
            requires (!decayed_same_as<EE, expected<T, E>>)
        expected(EE&& v) // NOLINT
            : m_data(std::forward<EE>(v))
        {
        }

        expected(const expected&)     = default;
        expected(expected&&) noexcept = default;

        operator bool() const { return std::holds_alternative<T>(m_data); }

        const T& get() const
        {
            if (*this)
                return std::get<T>(m_data);

            throw error();
        }

        const E& error() const
        {
            return std::get<E>(m_data);
        }

    private:
        std::variant<T, E> m_data{};
    };
} // namespace utils
