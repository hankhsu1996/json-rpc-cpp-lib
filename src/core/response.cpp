#include <spdlog/spdlog.h>

#include "json_rpc/core/response.h"

namespace json_rpc {

// Private constructor
Response::Response(const Json &response, std::optional<int> id)
    : response_(response) {
  if (id.has_value()) {
    response_["id"] = id.value();
  }
  ValidateResponse();
}

Response Response::SuccessResponse(
    const Json &result, const std::optional<int> &id) {
  Json response = {{"result", result}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return Response(response);
}

Response Response::LibraryErrorResponse(
    const std::string &message, int code, const std::optional<int> &id) {
  return Response(CreateErrorResponse(message, code, id));
}

Response Response::UserErrorResponse(
    const Json &error, const std::optional<int> &id) {
  Response response({{"error", error}}, id);
  response.ValidateResponse();
  return response;
}

void Response::ValidateResponse() const {
  if (!response_.contains("result") && !response_.contains("error")) {
    spdlog::error("Response validation failed: missing 'result' or 'error'");
    throw std::invalid_argument(
        "Response must contain either 'result' or 'error' field.");
  }
  if (response_.contains("error")) {
    const auto &error = response_["error"];
    if (!error.contains("code") || !error.contains("message")) {
      spdlog::error("Response validation failed: missing 'code' or 'message' "
                    "in error object");
      throw std::invalid_argument(
          "Error object must contain 'code' and 'message' fields.");
    }
  }
}

Json Response::ToJson() const {
  return response_;
}

Json Response::CreateErrorResponse(
    const std::string &message, int code, const std::optional<int> &id) {
  Json error = {{"code", code}, {"message", message}};
  Json response = {{"error", error}};
  if (id.has_value()) {
    response["id"] = id.value();
  }
  return response;
}

} // namespace json_rpc
