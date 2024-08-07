#include <memory>

#include <jsonrpc/client/client.hpp>
#include <jsonrpc/transport/pipe_transport.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

using namespace jsonrpc::client;
using namespace jsonrpc::transport;
using Json = nlohmann::json;

int main() {
  auto logger = spdlog::basic_logger_mt("client", "logs/client.log", true);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);

  std::string socketPath = "/tmp/calculator_pipe";
  auto transport = std::make_unique<PipeTransport>(socketPath, false);
  Client client(std::move(transport));
  client.Start();

  Json addRes = client.SendMethodCall("add", Json({{"a", 10}, {"b", 5}}));
  spdlog::info("Add result: {}", addRes.dump());

  Json divRes = client.SendMethodCall("divide", Json({{"a", 10}, {"b", 0}}));
  spdlog::info("Divide result: {}", divRes.dump());

  client.SendNotification("stop");

  client.Stop();
  return 0;
}
