#pragma once

#include <type_traits>
#include <variant>
#include <vector>

namespace io_automaton {
namespace detail {
// Use this template to specify a set of MessageType.
// IOAutomaton needs this to store/dispatch messages.
class BasicInputMessage {};

template <class... M>
class InputMessages : public BasicInputMessage {};

template <class T>
concept bool InputMessagesC =
  requires { std::is_base_of_v<T, BasicInputMessage>; };

class BasicOutputMessage {};
template <class... M>
class OutputMessages : public BasicOutputMessage {};

template <class T>
concept bool OutputMessagesC =
  requires { std::is_base_of_v<T, BasicOutputMessage>; };

class BasicMessageAlphabet {};

template <InputMessagesC InputM, OutputMessagesC OutputM>
class MessageAlphabet : public BasicMessageAlphabet {
public:
  using InputMessages = InputM;
  using OutputMessages = OutputM;
};

template <class T>
concept bool MessageAlphabetC =
  requires { std::is_base_of_v<T, BasicMessageAlphabet>; };

template <class TSet, class TSubset>
struct IsSubsetOf;

template <class T, class... Ts>
struct AnyOf {
  static constexpr bool value = (std::is_same_v<T, Ts> || ...);
};

template <class... T1, class... T2>
struct IsSubsetOf<std::tuple<T1...>, std::tuple<T2...>> {
  static constexpr bool value = (AnyOf<T2, T1...>::value && ...);
};

// Output action should has return type of a specialization of this template, to send messages.
template <class... M>
class MessagesToSend {
  template <class V1>
  static std::variant<M...> VariantConvert(V1 &&v1) {
      return std::visit([](auto &&v) {
          return std::variant<M...>(std::forward<decltype(v)>(v));
      }, std::forward<V1>(v1));
  }

  template <class... X>
  friend class MessagesToSend;

public:
  template <class... T, class = std::enable_if_t<IsSubsetOf<std::tuple<M...>, std::tuple<T...>>::value>>
  MessagesToSend(const MessagesToSend<T...> &rhs) {
    for (auto &[m, seq] : rhs.messages_)
      messages_.emplace_back(VariantConvert(std::move(m)), std::move(seq));
  }

  template <class T, class Seq>
  MessagesToSend(T &&message, Seq &&send_to) {
    Add(std::forward<T>(message), std::forward<Seq>(send_to));
  }

  template <class T, class Seq>
  void Add(T &&message, Seq &&send_to) 
    requires requires {
        { std::vector<int>{std::forward<Seq>(send_to)} };
        { std::variant<M...>(std::forward<T>(message)) };
      } {
    messages_.emplace_back(std::forward<T>(message), std::forward<Seq>(send_to));
  }

private:
  std::vector<std::pair<std::variant<M...>, std::vector<int>>> messages_;
};

template <class M>
struct IsMessagesToSend : std::false_type {};

template <class... M>
struct IsMessagesToSend<MessagesToSend<M...>> : std::true_type {};

} // namespace detail

template <detail::InputMessagesC InputM, detail::OutputMessagesC OutputM>
using MessageAlphabet = detail::MessageAlphabet<InputM, OutputM>;

template <class... M>
using InputMessages = detail::InputMessages<M...>;

template <class... M>
using OutputMessages = detail::OutputMessages<M...>;

template <class... M>
using MessagesToSend = detail::MessagesToSend<M...>;

} // namespace messages
