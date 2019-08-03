#pragma once

#include "signature.h"
#include "underlying-graph.h"

namespace io_automaton {
namespace detail {
class IOAutomaton {
public:
  template <SignatureC signature>
  static void CreateExecutionSystem(signature s, UnderlyingGraph graph) {

  }
};

} // namespace detail

using IOAutomaton = detail::IOAutomaton;

// static_assert(std::is_convertible_v<MessagesToSend<int>, MessagesToSend<double>>);

} // namespace io_automaton