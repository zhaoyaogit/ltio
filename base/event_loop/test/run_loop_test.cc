#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <iostream>
#include <sys/epoll.h>
#include <atomic>
#include <memory>

#include "../fd_event.h"
#include "../event_pump.h"
#include "../timer_event.h"
#include "../msg_event_loop.h"


bool MessageLoop2Test();
bool FdEventTest();
bool TimerEventTest();

int main() {
  //FdEventTest();
  //TimerEventTest();

  MessageLoop2Test();
  return 0;
}

bool MessageLoop2Test() {
#if 0
  {
    base::MessageLoop2 loop;
    sleep(1);
  }

  {
    base::MessageLoop2 loop;
    loop.Start();
    sleep(1);
  }

  {
    base::MessageLoop2 loop;
    loop.Start();
    loop.PostTask(base::NewClosure([&]() {
      LOG(INFO) << "PostTask Worked";
      loop.Pump()->Quit();
    }));
    loop.WaitLoopEnd();
    sleep(1);
  }
  //delay task
  {
    base::MessageLoop2 loop;
    loop.Start();

    LOG(INFO) << "Post A 1 second Task @" << base::time_us();
    loop.PostDelayTask(base::NewClosure([&]() {
      LOG(INFO) << "1 second task Run @" << base::time_us();
    }), 1000);

    LOG(INFO) << "Post A 0 second Task @" << base::time_us();
    loop.PostDelayTask(base::NewClosure([&]() {
      LOG(INFO) << "0 second task Run @" << base::time_us();
    }), 0);

    sleep(2);
    loop.PostTask(base::NewClosure([&]() {
      loop.Pump()->Quit();
    }));

    loop.WaitLoopEnd();
    sleep(1);
  }
#endif

  {
    base::MessageLoop2 loop;
    loop.Start();

    loop.PostTaskAndReply(
      base::NewClosure([&](){
        LOG(INFO) << "PostTaskAndReply task run";
      }), base::NewClosure([]() {
        LOG(INFO) << "PostTaskAndReply reply run";
      }), &loop)

    sleep(2);
    loop.PostTask(base::NewClosure([&]() {
      loop.Pump()->Quit();
    }));
    loop.WaitLoopEnd();
    sleep(1);
  }

}

bool FdEventTest() {
  base::EventPump loop;

  std::atomic<int> flag;
  flag.store(false);

  int fd[2];
  if (0 != pipe2(fd, O_CLOEXEC | O_NONBLOCK)) {
    std::cout << "Create O_NONBLOCK pipe Failed" << std::endl;
    return false;
  }
  int read_fd = fd[0];
  int write_fd = fd[1];
  std::cout << "read fd" << read_fd << " write fd:" << write_fd << std::endl;

  base::FdEvent fd_ev(read_fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP);
  fd_ev.SetDelegate(&loop);
  fd_ev.SetReadCallback([&]() {
    int value = 0;
    int count = read(fd_ev.fd(), (char*)&value, sizeof(int));
    std::cout << "Get " << count << " bytes data, Value:" << value << std::endl;

    fd_ev.DisableReading();
    //loop.RemoveFdEvent(&fd_ev);
    //loop.Quit();
  });

  fd_ev.SetCloseCallback([&]() {
    LOG(INFO) << "fd was be closed, you should do your close action here, reconnection or clean up";
    //case 1; clean up,
    loop.RemoveFdEvent(&fd_ev);
    //case 2; reconnection
  });

  std::thread thread([&](){
    int times = 10;
    sleep(1);
    int count = write(write_fd, &times, sizeof(times));
    std::cout << "write " << count << " bytes data to fd" << std::endl;

    sleep(2);
    count = write(write_fd, &times, sizeof(times));
    std::cout << "write " << count << " bytes data to fd" << std::endl;

  });

  loop.InstallFdEvent(&fd_ev);
  loop.Run();

  if (thread.joinable()) {
    thread.join();
  }
  close(write_fd);
  close(read_fd);
  return 0;
}

bool TimerEventTest() {
  {
    base::TimerTaskQueue q;
    q.TestingPriorityQueue();
  }

  base::EventPump loop;
  int repeat_count = 0;

  {
    base::RefTimerEvent timer = std::make_shared<base::TimerEvent>(5000);
    uint32_t timer_id = loop.ScheduleTimer(timer);
    timer->SetTimerTask(base::NewClosure([&]() {
      LOG(INFO) << "timer1(once) Invoked";
    }));
  }

  {
    base::RefTimerEvent timer2 = std::make_shared<base::TimerEvent>(2000, false);
    uint32_t timer_id2 = loop.ScheduleTimer(timer2);
    timer2->SetTimerTask(base::NewClosure([&]() {
      LOG(INFO) << "timer2(repeated) Invoked";
      if (3 == (++repeat_count)) {
        loop.CancelTimer(timer_id2);
      }
    }));
  }

  {
    base::RefTimerEvent timer3 = std::make_shared<base::TimerEvent>(10000);
    uint32_t timer_id3 = loop.ScheduleTimer(timer3);
    timer3->SetTimerTask(base::NewClosure([&]() {
      LOG(INFO) << "timer3( for quit ) Invoked";
      loop.Quit();
    }));
  }

  loop.Run();
  return true;
}
