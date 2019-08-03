#pragma once

#include "actions.h"
#include "messages.h"
#include "state.h"

#include <type_traits>

namespace io_automaton {
namespace detail {
class BasicSignature {};

template <MessageAlphabetC M, StateC S, InputActionSet In, OutputActionSet Out, InternalActionSet Int>
class Signature : public BasicSignature {
  static_assert(CheckInputActionSet<typename M::InputMessages, S, In>::CheckResult);
  static_assert(CheckOutputActionSet<typename M::OutputMessages, S, Out>::CheckResult);
  static_assert(CheckInternalActionSet<M, S, Int>::CheckResult);
};

template <class T>
concept bool SignatureC =
  requires {
    std::is_base_of_v<T, BasicSignature>;
  };

} // namespace detail

template <detail::MessageAlphabetC M, detail::StateC S, detail::InputActionSet In, detail::OutputActionSet Out, detail::InternalActionSet Int>
using Signature = detail::Signature<M, S, In, Out, Int>;

} // namespace io_automaton
