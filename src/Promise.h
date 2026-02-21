#pragma once

#include <coroutine>
#include <exception>
#include <functional>
#include <memory>

class Promise
{
 private:
  struct SharedState
  {
    bool m_isReady{false};
    std::vector<std::function<void()>> m_callbacks;
  };

 private:
  std::shared_ptr<SharedState> state;

 public:
  struct Awaiter
  {
   private:
    std::shared_ptr<SharedState> state;

   public:
    explicit Awaiter(std::shared_ptr<SharedState> state)
      : state(state)
    {
    }

    bool await_ready()
    {
      return state->m_isReady;
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
      state->m_callbacks.push_back([handle]() { handle.resume(); });
    }

    void await_resume() {}
  };

 public:
  Promise()
    : state(std::make_shared<SharedState>())
  {
  }
  Promise(const Promise& other) = default;
  Promise& operator=(const Promise& other) = default;
  Promise(Promise&& other) = default;
  Promise& operator=(Promise&& other) = default;

  bool IsReady() const
  {
    return state->m_isReady;
  }

  void Set()
  {
    // we can only execute a Promise once
    if (state->m_isReady) return;

    state->m_isReady = true;
    for (auto const & cb : state->m_callbacks)
    {
      cb();
    };
  }

  Awaiter operator co_await()
  {
    return Awaiter{state};
  }
};

template <typename... Args>
struct std::coroutine_traits<Promise, Args...>
{
  struct promise_type
  {
    Promise promise;

    Promise get_return_object()
    {
      return promise;
    }

    std::suspend_never initial_suspend() noexcept
    {
      return {};
    }
    std::suspend_never final_suspend() noexcept
    {
      return {};
    }

    void return_void()
    {
      promise.Set();
    }

    void unhandled_exception()
    {
      // Not a proper way of doing this, but for sake of simplicity, let's just end the program
      std::terminate();
    }
  };
};