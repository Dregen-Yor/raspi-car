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
  DTO_FIELD(Float64,Dis);     //距离 field
};

/* End DTO code-generation */
#include OATPP_CODEGEN_END(DTO)
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
#include<string>
#include<iostream>
#include <iomanip>
// #include "sgx.h"
extern "C" {
#include "move.c"
#include "BME280.c"
}
std::thread global;
bool kep=1;
void runTrace(){
  while(1){
    delay(5);
    trace();
    if(!kep){
      break;
    }
  }
  stop();
}
class Handler : public oatpp::web::server::HttpRequestHandler {
public:
  /**
   * Handle incoming request and return outgoing response.
   */
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    double Temp[4];
    getAll(Temp);
    // printf("%lf 度 %lf pa %lf h\n",Temp[0],Temp[1],Temp[2]);
    auto message = MessageDto::createShared();
    message->Temp = Temp[0];
    message->Pres = Temp[1];
    message->Hum = Temp[2];
    message->Dis = Temp[3];
    auto jsonObjectMapper = oatpp::json::ObjectMapper::createShared();
    oatpp::String json = jsonObjectMapper->writeToString(message); 
    auto res=ResponseFactory::createResponse(Status::CODE_200, json->c_str());
    res->putHeader("Access-Control-Allow-Origin","*");
    return res;
  }
};
class moveHandler : public oatpp::web::server::HttpRequestHandler {
public:
  /**
   * Handle incoming request and return outgoing response.
   */
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    auto method=request->getQueryParameter("method");
    auto sspeed=request->getQueryParameter("speed");
    double speed=stod(*sspeed);
    std::cout<<*method;
    kep=0;
    if(*method=="ahead"){
      direct(speed);
    }
    else if(*method=="left"){
      left(speed);
    }else if(*method=="right"){
      right(speed);
    }else if(*method=="stop"){
      stop();
    }else if(*method=="trace"){
      kep=1;
      global=std::thread(runTrace);
      global.detach();
    }
    auto res=ResponseFactory::createResponse(Status::CODE_200,method->c_str());
    res->putHeader("Access-Control-Allow-Origin","*");
    return res;
  }
};
void run() {

  /* Create Router for HTTP requests routing */
  auto router = oatpp::web::server::HttpRouter::createShared();

   /* Route GET - "/hello" requests to Handler */
  router->route("GET", "/getBME", std::make_shared<Handler>());
  router->route("GET","/move",std::make_shared<moveHandler>());

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
void CVcap(){
  VideoCapture capture;
  capture.open("http://192.168.35.158:8081/0/stream",CAP_ANY);
  if(capture.isOpened()){
    printf("success");
  }
  else{
    printf("failed");
  }
}
void CVfun(){
  Capture cap;
  cap.open("",CAP_ANY);
}
int main() {

  /* Init oatpp Environment */
  oatpp::Environment::init();
  init_BME();
  init();
  CVcap();

  /* Run App */
  run();

  /* Destroy oatpp Environment */
  oatpp::Environment::destroy();

  return 0;

}
