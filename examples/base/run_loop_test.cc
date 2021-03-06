#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <iostream>
#include <sys/epoll.h>
#include <atomic>
#include <memory>

#include "base/message_loop/fd_event.h"
#include "base/message_loop/event_pump.h"
#include "base/message_loop/timer_event.h"
#include "base/message_loop/message_loop.h"
#include "base/message_loop/linux_signal.h"
#include "base/coroutine/coroutine_runner.h"

bool LocationTaskTest() {
  base::MessageLoop loop;

  loop.Start();

  loop.PostTask(NewClosure([](){
    printf("FailureDump throw failed exception");
    throw -1;
  }));

  loop.PostDelayTask(NewClosure([&]() {
    loop.QuitLoop();
  }), 2000);
  loop.WaitLoopEnd();
  return false;
}

void FailureDump(const char* failure, int size) {
  std::string failure_info = std::string(failure, size);
  throw failure_info;
}

int main(int argc, char** argv) {
  LocationTaskTest();
  return 0;
}
