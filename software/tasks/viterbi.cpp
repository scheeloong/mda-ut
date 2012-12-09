#include "viterbi.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

#define DEBUG_VITERBI 1

// return -infinity if passed in 0
double ln(double x)
{
  if (x < 1/INF) {
    return -INF;
  }
  return log(x);
}

Viterbi::Viterbi(int num_states, int observations) : num_hidden_states(num_states), num_observations(observations)
{
  // initialize state probabilities, assuming we start in state 0 with probability 1 => log(1) = 0
  // initialize other state to probability 0 => log(0+) = -infinity
  state_prob.push_back(0);
  for (int i = 1; i < num_states; i++) {
    state_prob.push_back(-INF);
  }

  // initialize emission_prob vector
  for (int i = 0; i < num_observations; i++) {
    emission_prob.push_back(vector<double>());
  }
}

void Viterbi::set_transition_prob(int state, double * user_transition_prob)
{
  transition_prob.push_back(vector<double>());

  double total_prob = 0; // check that it sums to 1
  for (int i = 0; i < num_hidden_states; i++) {
    double prob = user_transition_prob[i];
    total_prob += prob;
    transition_prob[state].push_back(ln(prob)); // store in log-domain
  }

  assert(fabs(1 - total_prob) < 0.01);
}

void Viterbi::set_emission_prob(int observation, double* updated_emission_prob)
{
  for (int i = 0; i < num_hidden_states; i++) {
    emission_prob[observation].push_back(ln(updated_emission_prob[i]));
  }
}

int Viterbi::optimal_state(int observation)
{
  vector<double> prev_state_prob = state_prob;
  int state = 0;
  double max_state_prob = 0;

  for (int i = 0; i < num_hidden_states; i++) {
    // Add the best transition from a previous state to the emission probability
    double max_trans_prob = prev_state_prob[0] + transition_prob[0][i];
    for (int j = 1; j < num_hidden_states; j++) {
      double trans_prob = prev_state_prob[i] + transition_prob[j][i];
      if (trans_prob > max_trans_prob) {
        max_trans_prob = trans_prob;
      }
    }
    state_prob[i] = max_trans_prob + emission_prob[observation][i];

#ifdef DEBUG_VITERBI
    printf("%lf ", state_prob[i]);
#endif

    // Compute the maximum current state
    if (i == 0) {
      max_state_prob = state_prob[i];
    } else if (state_prob[i] > max_state_prob) {
      state = i;
      max_state_prob = state_prob[i];
    }
  }

#ifdef DEBUG_VITERBI
  puts("");
#endif

  return state;
}
