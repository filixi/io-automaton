#pragma once

#include "messages.h"

#include <stdexcept>
#include <type_traits>
#include <vector>

namespace io_automaton {
namespace detail {
// Action class shall inherite from this class to know the dispatch context,
// such as its neighbor, sender of the message etc.
class ActionContext {
public:
  virtual ~ActionContext() {}

  const std::vector<int> &GetNeighbors() const {
    return neightbors_;
  }

  int GetSender() const {
    if (class_ != ActionClass::Input)
      throw std::runtime_error("Sender not available in non-input actions.");
    return sender_;
  }

  int GetAssociatedUser() const {
    return associated_user_;
  }

  size_t GetNodeCount() const {
    return node_count_;
  }

  int GetMyIndex() const {
    return self_idx_;
  }

private:
  int sender_;

  std::vector<int> neightbors_;
  int node_count_;

  int associated_user_;

  int self_idx_;

  enum class ActionClass {
    unknown,
    Output,
    Input,
    Internal
  } class_;

};

class BasicActionsSet {};

enum class ActionCategory {
  InputAction,
  OutputAction,
  InternalAction
};

template <class...>
struct InputActions : public BasicActionsSet {
  static constexpr ActionCategory category = ActionCategory::InputAction;
};

template <class...>
struct OutputActions : public BasicActionsSet {
  static constexpr ActionCategory category = ActionCategory::OutputAction;
};

template <class...>
struct InternalActions : public BasicActionsSet {
  static constexpr ActionCategory category = ActionCategory::InternalAction;
};

template <class T>
concept bool ActionSet =
  requires { std::is_base_of_v<T, BasicActionsSet>; };

template <class T>
concept bool InputActionSet =
  requires { std::is_base_of_v<T, BasicActionsSet>; T::category == ActionCategory::InputAction; };

template <class T>
concept bool OutputActionSet =
  requires { std::is_base_of_v<T, BasicActionsSet>; T::category == ActionCategory::OutputAction; };

template <class T>
concept bool InternalActionSet =
  requires { std::is_base_of_v<T, BasicActionsSet>; T::category == ActionCategory::InternalAction; };

template <class A, class S, class M>
struct GetInputActionInputType;

template <class A, class S, class M>
concept bool InputActionInputMessageHlpr =
  requires(const A &a, S &s, const M &m) { { a.Act(s, m, 0) } -> void; };

template <class A, class S, class...>
struct GetInputActionInputTypeImpl;

template <class A, class S, class First, class... Rest>
struct GetInputActionInputTypeImpl<A, S, First, Rest...> {
  using type = std::conditional_t<
    InputActionInputMessageHlpr<A, S, First>,
    First,
    typename GetInputActionInputTypeImpl<A, S, Rest...>::type
  >;
};

template <class A, class S>
struct GetInputActionInputTypeImpl<A, S> {
  using type = void;
};

template <class A, class S, class... M>
struct GetInputActionInputType<A, S, InputMessages<M...>> {
  using type = typename GetInputActionInputTypeImpl<A, S, M...>::type;
  static_assert(!std::is_same_v<type, void>, "Cannot find a valid input message type.");
};

template <class A, class S, class M>
concept bool InputActionActCallByConstConcept =
  requires(const A &a, S &s, const M &m) { { a.Act(s, m, 0) }; };
template <class A, class S, class M>
concept bool InputActionActReturnTypeConcept =
  requires(const A &a, S &s, const M &m) { { a.Act(s, m, 0) } -> void; };

template <class A, class S, InputMessagesC M>
struct CheckInputAction {
  using InputMessageType = typename GetInputActionInputType<A, S, M>::type; 

  // InputAction type must have function member named `Act`,
  // this function member must can be called from a const lvalue of const InputAction,
  // and it must take a lvalue State as argument.
  static_assert(InputActionActCallByConstConcept<A, S, InputMessageType>, "InputAction::Act call failed.");

  static_assert(InputActionActReturnTypeConcept<A, S, InputMessageType>, "InputAction::Act must return void.");

  static constexpr bool CheckResult = 
    InputActionActCallByConstConcept<A, S, InputMessageType> &&
    InputActionActReturnTypeConcept<A, S, InputMessageType>;
};

template <class>
struct CheckInputActionSignature : std::false_type {};

template <class... Actions>
struct CheckInputActionSignature<InputActions<Actions...>> : std::true_type  {

};

template <class A, class S>
concept bool InternalActionActCallByConstConcept =
  requires(const A &a, S &s) { { a.Act(s) }; };
template <class A, class S>
concept bool InternalActionActReturnTypeConcept =
  requires(const A &a, S &s) { { a.Act(s) } -> void; };
template <class A, class S>
concept bool InternalActionPredCallByConstConcept =
  requires(const A &a, S &s) { { a.Pred(s) }; };
template <class A, class S>
concept bool InternalActionPredReturnTypeConcept =
  requires(const A &a, S &s) { { a.Pred(s) } -> bool; };

template <class A, class S>
struct CheckInternalAction {
  // InternalAction type must have function member named `Act`,
  // this function member must can be called from a const lvalue InternalAction,
  // and it must take a lvalue State as argument.
  static_assert(InternalActionActCallByConstConcept<A, S>, "InternalAction::act call failed.");

  static_assert(InternalActionActReturnTypeConcept<A, S>, "InternalAction::Act must return void.");

  // InternalAction type must have function member named `Pred`,
  // this function member must can be called from a const lvalue InternalAction,
  // and it must take a const lvalue State as argument.
  static_assert(InternalActionPredCallByConstConcept<A, S>, "InternalAction::Pred call failed.");

  static_assert(InternalActionPredReturnTypeConcept<A, S>, "InternalAction::Pred must return bool.");

  static constexpr bool CheckResult = 
    InternalActionActCallByConstConcept<A, S> &&
    InternalActionActReturnTypeConcept<A, S> &&
    InternalActionPredCallByConstConcept<A, S> &&
    InternalActionPredReturnTypeConcept<A, S>;
};

template <class A, class S>
concept bool OutputActionActCallByConstConcept =
  requires(const A &a, S &s) { { a.Act(s) }; };
template <class A, class S, class... M>
concept bool OutputActionActReturnTypeConcept =
  requires(const A &a, S &s) { std::is_convertible_v<decltype(a.Act(s)), MessagesToSend<M...>>; };
template <class A, class S>
concept bool OutputActionPredCallByConstConcept =
  requires(const A &a, S &s) { { a.Pred(s) }; };
template <class A, class S>
concept bool OutputActionPredReturnTypeConcept =
  requires(const A &a, S &s) { { a.Pred(s) } -> bool; };

template <class A, class S, class M>
struct CheckOutputAction;

template <class A, class S, class... M>
struct CheckOutputAction<A, S, OutputMessages<M...>> {
  // OutputAction type must have function member named `Act`,
  // this function member must can be called from a const lvalue OutputAction,
  // and it must take a lvalue State as argument.
  static_assert(OutputActionActCallByConstConcept<A, S>, "OutputAction::Act call failed.");

  // OutputAction::Act's return type must can be explicitly converted to std::variant of all output message types.
  static_assert(OutputActionActReturnTypeConcept<A, S, M...> &&
    std::is_convertible_v<decltype(std::declval<const A &>().Act(std::declval<S &>())), MessagesToSend<M...>> // workaround
    , "OutputAction::Act return type check failed.");

  // OutputAction type must have function member named `Pred`,
  // this function member must can be called from a const lvalue OutputAction,
  // and it must take a const lvalue State as argument."
  static_assert(OutputActionPredCallByConstConcept<A, S>, "OutputAction::Pred call failed.");

  static_assert(OutputActionPredReturnTypeConcept<A, S>, "OutputAction::Pred must return bool.");

  static constexpr bool CheckResult = 
    OutputActionActCallByConstConcept<A, S> &&
    OutputActionActReturnTypeConcept<A, S, M...> &&
    OutputActionPredCallByConstConcept<A, S> &&
    OutputActionPredReturnTypeConcept<A, S>;
};

template <class, class, class>
struct CheckInputActionSet;
template <class M, class S, class... A>
struct CheckInputActionSet<M, S, InputActions<A...>> {
  static constexpr bool CheckResult = 
    (CheckInputAction<A, typename S::type, M>::CheckResult && ...);
};

template <class, class, class>
struct CheckOutputActionSet;
template <class M, class S, class... A>
struct CheckOutputActionSet<M, S, OutputActions<A...>> {
  static constexpr bool CheckResult = 
    (CheckOutputAction<A, typename S::type, M>::CheckResult && ...);
};

template <class, class, class>
struct CheckInternalActionSet;
template <class M, class S, class... A>
struct CheckInternalActionSet<M, S, InternalActions<A...>>  {
  static constexpr bool CheckResult = 
    (CheckInternalAction<A, typename S::type>::CheckResult && ...);
};

} // namespace detail

using ActionContext = detail::ActionContext;

template <class... A>
using InputActions = detail::InputActions<A...>;

template <class... A>
using OutputActions = detail::OutputActions<A...>;

template <class... A>
using InternalActions = detail::InternalActions<A...>;

} // namespace io_automaton
