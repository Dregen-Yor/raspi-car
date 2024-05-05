#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/Environment.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"

/* Begin DTO code-generation */
#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * Message Data-Transfer-Object
 */
class MessageDto : public oatpp::DTO {

  DTO_INIT(MessageDto, DTO /* Extends */)

  DTO_FIELD(Float64, Temp);   // 温度 field
  DTO_FIELD(Float64, Pres);   // 气压 field
  DTO_FIELD(Float64,Hum);      // 湿度 field

};

/* End DTO code-generation */
#include OATPP_CODEGEN_END(DTO)
#include<string>
// #include "sgx.h"
extern "C" {
#include "move.c"
#include "BME280.c"
}
class Handler : public oatpp::web::server::HttpRequestHandler {
public:
  /**
   * Handle incoming request and return outgoing response.
   */
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    double Temp[3];
    getT(Temp);
    printf("%lf 度 %lf pa %lf h",Temp[0],Temp[1],Temp[2]);
    auto message = MessageDto::createShared();
    message->Temp = Temp[0];
    message->Pres = Temp[1];
    message->Hum = Temp[2];
    auto jsonObjectMapper = oatpp::json::ObjectMapper::createShared();
    oatpp::String json = jsonObjectMapper->writeToString(message); 
    return ResponseFactory::createResponse(Status::CODE_200, json->c_str());
  }

};
// class Handler : public oatpp::web::server::HttpRequestHandler {
// private:
//   std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
// public:
//   Handler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper)
//     : m_objectMapper(objectMapper)
//   {}
//   std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    
//     // return ResponseFactory::createResponse(Status::CODE_200,std::to_string(Temp[1]).c_str());
//   }
// };

void run() {

  /* Create Router for HTTP requests routing */
  auto router = oatpp::web::server::HttpRouter::createShared();

   /* Route GET - "/hello" requests to Handler */
  router->route("GET", "/getBME", std::make_shared<Handler>());

  /* Create HTTP connection handler with router */
  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

  /* Create TCP connection provider */
  auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 5800, oatpp::network::Address::IP_4});

  /* Create server which takes provided TCP connections and passes them to HTTP connection handler */
  oatpp::network::Server server(connectionProvider, connectionHandler);

  /* Print info about server port */
  OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

  /* Run server */
  server.run();
}

int main() {

  /* Init oatpp Environment */
  oatpp::Environment::init();
  init_BME();
  init();
  /* Run App */
  run();

  /* Destroy oatpp Environment */
  oatpp::Environment::destroy();

  return 0;

}