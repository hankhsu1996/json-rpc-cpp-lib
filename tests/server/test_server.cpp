#include <memory>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "jsonrpc/server/dispatcher.hpp"
#include "jsonrpc/server/server.hpp"
#include "jsonrpc/server/transports/stdio_server_transport.hpp"

using namespace jsonrpc::server;
using namespace jsonrpc::server::transports;

class MockServerTransport : public ServerTransport {
public:
  MockServerTransport() : ServerTransport() {
  }

protected:
  void Listen() override {
    spdlog::info("MockServerTransport listening");
    while (IsRunning()) {
      // Simulate some work
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    spdlog::info("MockServerTransport stopped listening");
  }
};

TEST_CASE("Server initialization", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));
  REQUIRE_NOTHROW(server);
}

TEST_CASE("Server start and stop", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));

  REQUIRE_FALSE(server.IsRunning());

  // Start the server in a separate thread
  std::thread serverThread([&server]() { REQUIRE_NOTHROW(server.Start()); });

  // Simulate some runtime
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(server.IsRunning());

  REQUIRE_NOTHROW(server.Stop());
  if (serverThread.joinable()) {
    serverThread.join();
  }

  REQUIRE_FALSE(server.IsRunning());
}

TEST_CASE("Server method registration", "[server]") {
  auto transport = std::make_unique<MockServerTransport>();
  Server server(std::move(transport));

  MethodCallHandler methodHandler =
      [](const std::optional<nlohmann::json> &) -> nlohmann::json {
    return {{"result", "testMethod"}};
  };

  NotificationHandler notificationHandler =
      [](const std::optional<nlohmann::json> &) {};

  REQUIRE_NOTHROW(server.RegisterMethodCall("testMethod", methodHandler));
  REQUIRE_NOTHROW(
      server.RegisterNotification("testNotification", notificationHandler));
}
