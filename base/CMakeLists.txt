SET(BASE_SOURCES
  time/timestamp.cc
  time/time_utils.cc

  utils/sys_error.cc
  utils/rand_util.cc
  utils/rand_util_posix.cc
  utils/ns_convertor.cc
  utils/string/str_utils.cc
  #coroutine
  coroutine/wait_group.cc
  coroutine/coroutine_runner.cc
  #new eventloop
  message_loop/event.cc
  message_loop/fd_event.cc
  message_loop/event_pump.cc
  message_loop/linux_signal.cpp
  message_loop/file_util_linux.cc
  message_loop/io_multiplexer.cc
  message_loop/io_mux_epoll.cc
  #message_loop/timer_task_queue.cc
  message_loop/message_loop.cc
  message_loop/timeout_event.cc
  message_loop/repeating_timer.cc
  #spin lock
  #memory
  memory/spin_lock.cc
  memory/lazy_instance.cc
  # gzip compression utils
  utils/gzip/gzip_utils.cc
  # closure
  closure/location.cc
  closure/closure_task.cc
  )

if (LTIO_USE_ACO_CORO_IMPL)
  list(APPEND BASE_SOURCES coroutine/aco_impl.cc)
else()
  list(APPEND BASE_SOURCES coroutine/coro_impl.cc)
endif()

add_library(ltbase_objs OBJECT ${BASE_SOURCES})
ltio_default_properties(ltbase_objs)
