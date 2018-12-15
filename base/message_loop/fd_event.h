#ifndef FD_EVENT_H
#define FD_EVENT_H

#include <memory>

#include <functional>
#include <sys/epoll.h>
#include <sys/poll.h>
#include "base/base_micro.h"
#include "base/queue/double_linked_list.h"

namespace base {
/* A Event Holder and Owner represend a filedescriptor, 
 * Create With a fd and take it owner  close it when it's gone*/
class FdEvent : public EnableDoubleLinked<FdEvent> {
public:
  /* interface notify poller update polling events*/
  class FdEventWatcher {
  public:
    virtual ~FdEventWatcher() {}
    virtual void OnEventChanged(FdEvent* fd_event) = 0;
  };
  class EventHandler { //fd owner should implement those
  public:
    virtual void HandleRead(FdEvent* fd_event) = 0;
    virtual void HandleWrite(FdEvent* fd_event) = 0;
    virtual void HandleError(FdEvent* fd_event) = 0;
    virtual void HandleClose(FdEvent* fd_event) = 0;
  };

  typedef std::function<void()> EventCallback;
  static std::shared_ptr<FdEvent> Create(int fd, uint32_t event);

  FdEvent(int fd, uint32_t events);
  ~FdEvent();
  
  void SetFdWatcher(FdEventWatcher *d);
  FdEventWatcher* EventWatcher() {return watcher_;}

  void HandleEvent();
  void ResetCallback();
  void SetReadCallback(const EventCallback &cb);
  void SetWriteCallback(const EventCallback &cb);
  void SetErrorCallback(const EventCallback &cb);
  void SetCloseCallback(const EventCallback &cb);

  //the event we take care about
  uint32_t MonitorEvents() const;
  void SetRcvEvents(uint32_t ev);

  void Update();

  void EnableReading();
  inline bool IsReadEnable() {return events_ & EPOLLIN; }

  void EnableWriting();
  inline bool IsWriteEnable() {return events_ & EPOLLOUT; }

  void DisableAll() { events_ = 0; Update();}
  void DisableReading() { events_ &= ~EPOLLIN; Update();}
  void DisableWriting() { events_ &= ~EPOLLOUT; Update();}

  inline int fd() const {return fd_;};
  inline void GiveupOwnerFd() {owner_fd_life_ = false;}

  std::string EventInfo() const;
  std::string RcvEventAsString() const;
  std::string MonitorEventAsString() const;
private:
  std::string events2string(uint32_t event) const;

  const int fd_;
  uint32_t events_;
  uint32_t revents_;
  bool owner_fd_life_;

  EventHandler* handler_ = NULL;
  FdEventWatcher* watcher_ = NULL;

  EventCallback read_callback_;
  EventCallback write_callback_;
  EventCallback error_callback_;
  EventCallback close_callback_;
  DISALLOW_COPY_AND_ASSIGN(FdEvent);
};

typedef std::shared_ptr<FdEvent> RefFdEvent;

} // namespace
#endif // EVENT_H
