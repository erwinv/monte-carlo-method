#pragma once
#include <functional>
#include <future>
#include <type_traits>
#include <utility>
#include <dispatch/dispatch.h>

namespace tasksystem {

template <class F, class... As>
auto async(F &&f, As &&...args)
{
  using Result = std::result_of_t<std::decay_t<F>(std::decay_t<As>...)>;
  using Task = std::packaged_task<Result()>;

  auto task = new Task(
      std::bind(
          [fn = std::forward<F>(f)](As &...args)
          {
            return fn(std::move(args)...);
          },
          std::forward<As>(args)...));

  auto result = task->get_future();

  dispatch_async_f(
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
      task,
      [](void *t)
      {
        auto task = static_cast<Task *>(t);
        (*task)();
        delete task;
      });

  return result;
}

}
