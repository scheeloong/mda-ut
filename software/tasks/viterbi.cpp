#include "viterbi.h"

#include <assert.h>
#include <math.h>
#include <iostream>

using namespace std;

// define a large number as infinity since we're working in the log-domain to combat numerical underflow
#define INF 1000000000.

// return -infinity if passed in 0
double ln(double x)
{
  if (x < 1/INF) {
    return -INF;
  }
  return log(x);
}

Viterbi::Viterbi(int num_states, int observations) : num_hidden_states(num_states), num_observations(observations), prev_state(0)
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

  // initialize data counts to 0
  for (int i = 0; i < num_states; i++) {
    transition_data.push_back(vector<unsigned>());
    for (int j = 0; j < num_states; j++) {
      transition_data[i].push_back(0);
    }
  }

  for (int i = 0; i < num_observations; i++) {
    emission_data.push_back(vector<unsigned>());
    for (int j = 0; j < num_states; j++) {
      emission_data[i].push_back(0);
    }
  }
}

Viterbi::~Viterbi()
{
  // print out the collected data
  cout << "Viterbi\n=======\nTransition results\n\n";
  for (int i = 0; i < num_hidden_states; i++) {
    for (int j = 0; j < num_hidden_states; j++) {
      cout << transition_data[i][j] << ' ';
    }
    cout << '\n';
  }

  cout << "\nEmission results\n\n";
  for (int i = 0; i < num_observations; i++) {
    for (int j = 0; j < num_hidden_states; j++) {
      cout << emission_data[i][j] << ' ';
    }
    cout << '\n';
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

void Viterbi::set_emission_prob(int observation, double* user_emission_prob)
{
  for (int i = 0; i < num_hidden_states; i++) {
    double prob = user_emission_prob[i];
    emission_prob[observation].push_back(ln(prob));
  }
}

bool Viterbi::check_emission_prob()
{
  for (int i = 0; i < num_hidden_states; i++) {
    double total_prob = 0;
    for (int j = 0; j < num_observations; j++) {
      total_prob += exp(emission_prob[j][i]);
    }
    if (fabs(1 - total_prob) >= 0.01) {
      return false;
    }
  }

  return true;
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
      double trans_prob = prev_state_prob[j] + transition_prob[j][i];
      if (trans_prob > max_trans_prob) {
        max_trans_prob = trans_prob;
      }
    }
    state_prob[i] = max_trans_prob + emission_prob[observation][i];

    // Compute the maximum current state
    if (i == 0) {
      max_state_prob = state_prob[i];
    } else if (state_prob[i] > max_state_prob) {
      state = i;
      max_state_prob = state_prob[i];
    }
  }

  // Update data for transitions and emissions
  transition_data[prev_state][state]++;
  emission_data[observation][state]++;
  prev_state = state;

  return state;
}
