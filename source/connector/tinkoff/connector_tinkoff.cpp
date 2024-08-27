#include "connector_tinkoff.hpp"

#include <rpp/operators/combine_latest.hpp>
#include <rpp/operators/finally.hpp>
#include <rpp/operators/flat_map.hpp>
#include <rpp/operators/group_by.hpp>
#include <rpp/operators/publish.hpp>
#include <rpp/operators/ref_count.hpp>
#include <rpp/sources/create.hpp>
#include <rpp/sources/just.hpp>
#include <rpp/sources/never.hpp>

#include <grpc++/create_channel.h>
#include <investapi_proto/instruments.grpc.pb.h>
#include <investapi_proto/marketdata.grpc.pb.h>
#include <rppgrpc/client_reactor.hpp>


constexpr auto URL = "invest-public-api.tinkoff.ru:443";

namespace tf = tinkoff::public_::invest::api::contract::v1;

namespace connector::tinkoff
{
    namespace
    {
        auto subscribe_to_ticker(const std::shared_ptr<tf::MarketDataStreamService::Stub>& market, const std::string& ticker)
        {
            tf::MarketDataServerSideStreamRequest req{};

            auto& price = *req.mutable_subscribe_last_price_request();
            price.set_subscription_action(tf::SubscriptionAction::SUBSCRIPTION_ACTION_SUBSCRIBE);
            price.add_instruments()->set_instrument_id(ticker);

            auto& trades = *req.mutable_subscribe_trades_request();
            trades.set_subscription_action(tf::SubscriptionAction::SUBSCRIPTION_ACTION_SUBSCRIBE);
            trades.add_instruments()->set_instrument_id(ticker);

            auto ctx = std::make_shared<grpc::ClientContext>();

            const auto reactor = new rppgrpc::client_read_reactor<tf::MarketDataResponse>();
            market->async()->MarketDataServerSideStream(ctx.get(), &req, reactor);
            return std::pair{ctx, reactor};
        }
        class authenticator final : public grpc::MetadataCredentialsPlugin
        {
        public:
            authenticator(const grpc::string& token)
                : m_token(token)
            {
            }

            bool        IsBlocking() const override { return false; }
            const char* GetType() const override { return "authorization"; }

            grpc::Status GetMetadata(grpc::string_ref, grpc::string_ref, const grpc::AuthContext&, std::multimap<grpc::string, grpc::string>* metadata) override // NOLINT
            {
                metadata->insert(std::make_pair(GetType(), std::string{"Bearer "} + m_token));
                return grpc::Status::OK;
            }

        private:
            grpc::string m_token;
        };

        class connector_tinkoff final : public connector::contract::interface
        {
        public:
            explicit connector_tinkoff(const std::string& token)
                : m_channel{grpc::CreateChannel(URL, grpc::CompositeChannelCredentials(grpc::SslCredentials(grpc::SslCredentialsOptions()), grpc::MetadataCredentialsFromPlugin(std::make_unique<authenticator>(token))))}
            {
            }

            utils::expected<contract::ticker_info, std::string> get_ticker_info(const std::string& ticker) const override
            {
                tf::InstrumentRequest request{};
                request.set_id_type(tf::InstrumentIdType::INSTRUMENT_ID_TYPE_TICKER);
                request.set_class_code("TQBR");
                request.set_id(ticker);

                tf::ShareResponse   response{};
                grpc::ClientContext ctx{};

                const auto s = m_instruments->ShareBy(&ctx, request, &response);
                if (s.ok())
                    return contract::ticker_info{response.Utf8DebugString()};
                return s.error_message();
            }

            rpp::dynamic_observable<contract::ticker_event> get_ticker_events(const std::string& ticker) const override
            {
                return rpp::source::create<contract::ticker_event>([market = m_market, ticker]<typename TObs>(TObs&& obs) {
                    const auto [ctx, reactor] = subscribe_to_ticker(market, ticker);

                    reactor->get_observable()
                        | rpp::ops::map([](const tf::MarketDataResponse& response) {
                              return contract::ticker_event{.info = response.Utf8DebugString()};
                          })
                        | rpp::ops::finally([ctx = ctx]() noexcept {
                              ctx->TryCancel();
                          })
                        | rpp::ops::publish()
                        | rpp::ops::ref_count()
                        | rpp::ops::subscribe(std::forward<TObs>(obs));

                    reactor->init();
                });
            }

        private:
            std::shared_ptr<grpc::Channel>                     m_channel;
            std::shared_ptr<tf::InstrumentsService::Stub>      m_instruments{tf::InstrumentsService::NewStub(m_channel)};
            std::shared_ptr<tf::MarketDataStreamService::Stub> m_market{tf::MarketDataStreamService::NewStub(m_channel)};
        };
    } // namespace

    std::shared_ptr<contract::interface> get_connector(const std::string& token)
    {
        return std::make_shared<connector_tinkoff>(token);
    }
} // namespace connector::tinkoff
