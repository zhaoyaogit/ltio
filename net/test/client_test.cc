#include <glog/logging.h>

#include <vector>
#include "../tcp_channel.h"
#include "../socket_utils.h"
#include "../service_acceptor.h"
#include "../protocol/proto_service.h"
#include "../protocol/line/line_message.h"
#include "../protocol/http/http_request.h"
#include "../protocol/http/http_response.h"
#include "../protocol/proto_service_factory.h"
#include "../inet_address.h"
#include "clients/client_connector.h"
#include "clients/client_router.h"
#include "dispatcher/coro_dispatcher.h"
#include "base/coroutine/coroutine_runner.h"
#include "base/closure/closure_task.h"
#include "net/dispatcher/coro_dispatcher.h"
#include "net/protocol/raw/raw_message.h"
#include "net/protocol/raw/raw_proto_service.h"

base::MessageLoop loop;
base::MessageLoop wloop;

net::ClientRouter* http_router = NULL;
net::ClientRouter* raw_router = NULL;

bool SendRequest(int sequence_id) {
  net::RefHttpRequest request = std::make_shared<net::HttpRequest>();
  request->SetKeepAlive(true);
  request->SetRequestURL("/");
  request->InsertHeader("Accept", "*/*");
  request->InsertHeader("Host", "127.0.0.1");
  request->InsertHeader("T", std::to_string(sequence_id));
  //request->InsertHeader("Host", "g.test.amnetapi.com");
  request->InsertHeader("User-Agent", "curl/7.58.0");

  LOG(INFO) << "call send [" << sequence_id << "]st request";
  net::HttpResponse* response = http_router->SendRecieve(request);
  if (response && !response->IsKeepAlive()) {
    LOG(ERROR) << "recieve a closed response from server" << response->MessageDebug();
  }
  return response != NULL && request->MessageFailInfo() == net::FailInfo::kNothing;
}

void SendRawRequest(int sequence_id) {
  net::RefRawMessage raw_request = net::RawMessage::CreateRequest();
  std::string content("this is a raw request");
  raw_request->SetContent(content);
  raw_request->SetCode(12);
  raw_request->SetMethod(12);

  auto response = raw_router->SendRecieve(raw_request);
  if (response) {
    LOG(ERROR) << "Get RawResponse:\n" << response->MessageDebug();
  }
}
int g_count = 0;

void HttpClientBenchMark(int grp, int count) {
  LOG(ERROR) << "@"<< base::CoroRunner::Runner().CurrentCoroutineId()
             << " group:" << grp << " start, count:" << count;

  int success_count = 0;
  for (int i=0; i < count; i++) {
    if (SendRequest(g_count++)) {success_count++;}
  }

  LOG(ERROR) << "@"<< base::CoroRunner::Runner().CurrentCoroutineId()
             << " group:" << grp << " end, count/success:" << count << "/" << success_count;
}

// usage
int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);  // 初始化 gflags
  google::SetStderrLogging(google::GLOG_ERROR);

  loop.SetLoopName("clientloop");
  wloop.SetLoopName("workloop");

  net::CoroWlDispatcher* dispatcher_ = new net::CoroWlDispatcher(true);

  loop.Start();
  wloop.Start();
  {
    net::InetAddress http_server_addr("127.0.0.1", 80);
    http_router = new net::ClientRouter(&loop, http_server_addr);
    net::RouterConf router_config;
    router_config.protocol = "http";
    router_config.connections = 2;
    router_config.recon_interal = 100;
    router_config.message_timeout = 5000;
    http_router->SetupRouter(router_config);
    http_router->SetWorkLoadTransfer(dispatcher_);

    http_router->StartRouter();
  }

  sleep(2);

  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
    co_go &loop << std::bind(HttpClientBenchMark, 0, 1000);
  }

  loop.WaitLoopEnd();

  delete http_router;
  delete raw_router;
  return 0;
}
