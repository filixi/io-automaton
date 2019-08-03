#include "io-automaton.h"

#include <vector>
#include <stdexcept>
#include <algorithm>

// state
struct State {
  int v;
  std::vector<int> vals;
};

struct MSend {
  int v;
};

struct MDecide {
  int v;
};

// message alphabet
// each output message as a class, along with its associated action
struct Send : io_automaton::ActionContext {
  bool Pred(const State &) const {
    return true;
  }

  io_automaton::MessagesToSend<MSend> Act(State &s) const {
    return {MSend{s.v}, GetNeighbors()};
  }

};

struct Decide : io_automaton::ActionContext {
  bool Pred(const State &s) const {
    if (s.vals.size() != GetNodeCount())
      return false;

    return s.vals[0];
  }

  io_automaton::MessagesToSend<MDecide> Act(State &s) const {
    return {MDecide{s.v}, GetAssociatedUser()};
  }

};

// each input message as a class, along with its associated action
struct Init {
  static void Act(State &s, const Init &i, int) {
    s.v = i.v;
  }

  int v;
};

struct Receive {
	static void Act(State &s, const MSend &i, int source) {
    s.vals.resize(source);
    s.vals[source] = i.v;
  }
};

// internal actions



// generate signature

// execute

int main() {
  using M = io_automaton::MessageAlphabet<
    io_automaton::InputMessages<Init, MSend>,
    io_automaton::OutputMessages<MSend, MDecide>
  >;

  using S = io_automaton::State<State>;

  using Sig = io_automaton::Signature <
    M, S,
    io_automaton::InputActions<Init, Receive>,
    io_automaton::OutputActions<Send, Decide>,
    io_automaton::InternalActions<>
  >;

  int matrix[3][3] = {
    {1,1,1},
    {1,1,1},
    {1,1,1}
  };

  io_automaton::UnderlyingGraph g(matrix);

  // generate IO automaton
  // create execution context
  io_automaton::IOAutomaton::CreateExecutionSystem(Sig(), g);

  return 0;
}
