#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include "json_rpc/transports/framed_stdio_server_transport.hpp"

namespace json_rpc {

void FramedStdioServerTransport::Listen() {
  spdlog::info("FramedStdioServerTransport listening");

  if (!dispatcher_) {
    spdlog::error("Dispatcher is not set.");
    return;
  }

  while (IsRunning()) {
    try {
      std::string content = ReceiveMessage();
      std::optional<std::string> response =
          dispatcher_->DispatchRequest(content);
      if (response.has_value()) {
        spdlog::debug("FramedStdioServerTransport dispatching response: {}",
            response.value());
        SendMessage(response.value());
      }
    } catch (const std::runtime_error &e) {
      spdlog::error(
          "Runtime error in FramedStdioServerTransport: {}", e.what());
      break; // Exit loop on error
    }
  }
}

} // namespace json_rpc
