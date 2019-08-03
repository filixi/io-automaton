#pragma once

#include <type_traits>

namespace io_automaton {
namespace detail {
class BasicState {};

// Use this template to specify the internal state of each IOAutomaton.
// State is stored sparately comparing to other actor library, such as akka.
//
// Why? Because actions need to share state, and they are divided into three classes: Internal, Output, Input.
// It doesn't make more sense in implmentating process state into neither one of them.
template <class T>
class State : public BasicState {
public:
  const T &GetState() const { return state_; }
  T &GetState() { return state_; }

  using type = T;

private:
  T state_;
};

template <class T>
concept bool StateC =
  requires { std::is_base_of_v<T, BasicState>; };

} // namespace detail

template <class T>
using State = detail::State<T>; 

} // namespace io_automaton
