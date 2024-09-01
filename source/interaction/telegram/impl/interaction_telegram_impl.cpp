#include "interaction_telegram_impl.hpp"

#include <rpp/operators/observe_on.hpp>
#include <rpp/operators/publish.hpp>
#include <rpp/operators/ref_count.hpp>
#include <rpp/operators/subscribe_on.hpp>
#include <rpp/schedulers/new_thread.hpp>
#include <rpp/sources/create.hpp>

#include <fmt/format.h>
#include <tgbot/tgbot.h>
#include <utils.hpp>

namespace
{
    using namespace interaction::contract;

    struct command_info
    {
        std::string command{};
        std::string description{};
    };

    void add_command(const TgBot::Bot& bot, const command_info& command_info)
    {
        auto        commands = bot.getApi().getMyCommands();
        const auto& command  = commands.emplace_back(std::make_shared<TgBot::BotCommand>());
        command->command     = command_info.command;
        command->description = command_info.description;
        bot.getApi().setMyCommands(commands);
    }

    void add_command(TgBot::Bot& bot, const command_info& command_info, const TgBot::EventBroadcaster::MessageListener& handle)
    {
        bot.getEvents().onCommand(command_info.command, handle);

        add_command(bot, command_info);
    }

    template<rpp::constraint::observer TObserver>
    void fill_commands(TgBot::Bot& bot, const TObserver& observer)
    {
        add_command(bot,
                    {.command = "start", .description = " - Start bot"},
                    [&api = bot.getApi()](const TgBot::Message::Ptr& message) {
                        api.sendMessage(message->chat->id,
                                        "Hello! It is TradingFramework! \n"
                                        "You can subscribe to tickers with /subscribe TICKER command. \n"
                                        "Or you can also get some info about instrument by its ticker with /ticker_info TICKER command.");
                    });

        add_command(bot,
                    {.command = "ticker_info", .description = "ticker - Get information by ticker"},
                    [&api = bot.getApi(), &observer](const TgBot::Message::Ptr& message) {
                        constexpr std::string_view prefix = "/ticker_info";

                        std::string_view text = message->text;
                        ENSURE(text.starts_with(prefix));
                        text.remove_prefix(prefix.size() + 1);

                        if (text.find(' ') != std::string::npos)
                        {
                            api.sendMessage(message->chat->id, "You must write only one ticker after /ticker_info command!");
                            return;
                        }
                        observer.on_next(user_event{.user_id = message->chat->id, .payload = get_ticker_info{std::string{text}}});
                    });
    }

    class events_handler
    {
    public:
        events_handler(const TgBot::Bot& bot, int64_t user_id)
            : m_bot(bot)
            , m_user_id(user_id)
        {
        }

        void operator()(const subscribe_to_ticker::response& ticker_response) const
        {
            m_bot.getApi().sendMessage(m_user_id,
                                       fmt::format("[{}] New price - {} at {}",
                                                   ticker_response.ticker,
                                                   ticker_response.price,
                                                   std::chrono::system_clock::to_time_t(ticker_response.time)));
        }

        void operator()(const get_ticker_info::response& ticker_info) const
        {
            m_bot.getApi().sendMessage(m_user_id,
                                       fmt::format("[{}] {}",
                                                   ticker_info.ticker,
                                                   ticker_info.description));
        }

    private:
        const TgBot::Bot& m_bot;
        int64_t           m_user_id{};
    };
} // namespace

namespace interaction::telegram::impl
{
    contract::user_events get_users(const std::string& token, const contract::app_responses& app_responses, const TgBot::HttpClient* http_client)
    {
        return rpp::source::create<contract::user_event>([token, app_responses, http_client](const auto& observer) {
                   auto bot = http_client ? TgBot::Bot{token, *http_client} : TgBot::Bot{token};

                   fill_commands(bot, observer);
                   app_responses.subscribe([&bot](const app_response& app_response) {
                       std::visit(events_handler{bot, app_response.user_id}, app_response.payload);
                   });

                   try
                   {
                       TgBot::TgLongPoll long_poll(bot);
                       while (!observer.is_disposed())
                       {
                           long_poll.start();
                       }
                   }
                   catch (TgBot::TgException&)
                   {
                       observer.on_error(std::current_exception());
                   }
               })
             | rpp::ops::subscribe_on(rpp::schedulers::new_thread{})
             | rpp::ops::publish()
             | rpp::ops::ref_count();
    }
} // namespace interaction::telegram::impl
