/*
* This is an experimental algorithm, that came out as a result of studying for a final exam.
* This file contains an implementation of the Viterbi algorithm to 'track' targets in various task control algorithms.
* The main benefit of this algorithm is that it is probabilistic, so it can perform properly if there is some noise in the input,
* and it also allows a formulation of probabilistic error states, so that the algorithm can tell if it is 'lost'. For example, if
* the gate task is confident it has found the gate, but then sees no gate for ten images in a row, it is likely that it has lost the
* gate and should try backtracking or just giving up.
*
* Mathematically, a task that uses Viterbi implementation must construct a Viterbi object with a list of 'hidden' states. These
* correspond to 'regular' states in a state machine. The object must then be initialized with transition probabilities between
* all pairs of states. If no probability is set, it is assumed to be zero. As a simplification, the first state passed in the
* list will be assumed to be the starting state, with a probability of 1. This avoids having to input initial probabilities for each
* hidden state. Finally, to complete the Viterbi algorithm, each state is assigned a probability, given the image input. This interface
* is still in the making.
*
* Since this is a 'tracking' algorithm, the computed hidden state actually has a meaning. The meaning is that each hidden state can
* control the submarine using the actuator output interface. This interface is also exposed for completion.
*
* This class gives a way to express a control task as a Hidden Markov Model and implements a Viterbi algorithm to solve for the hidden
* states. There are learning algorithms that use the Viterbi algorithm to learn the transition probabilities, but manually tuning
* parameters may be easier than generating training runs for the learning algorithm.
*/

#ifndef _VITERBI_H
#define _VITERBI_H

#include <stdlib.h>
#include <vector>

#include "ActuatorOutput.h"

using namespace std;

// define a large number as infinity since we're working in the log-domain to combat numerical underflow
#define INF 1000000000.

class Viterbi {
  public:
    // Initialize state probabilities, given the number of hidden states
    Viterbi(int num_states);

    void set_transition_prob(int state, double * user_transition_prob);
    void update_emission_prob(double * updated_emission_prob);

    int optimal_state(); // compute the optimal state and return it
  private:
    int num_hidden_states;
    vector<double> state_prob;
    vector<vector<double> > transition_prob;
    vector<double> emission_prob;
};

#endif
