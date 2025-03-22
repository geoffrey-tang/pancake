#pragma once
#include <random>
#include <vector>

using namespace std;

class Distribution {
private:
    discrete_distribution<int> dist; // Distribution
    vector<string> items; // List of items
    vector<double> weights; // List of weights in the same order as items
public:
    Distribution(); // Initialization
    Distribution(vector<string> &i, vector<double> &w); // Initialization
    string choose_sample(); // Samples the distribution
    vector<string> get_items(); // Returns list of items
    vector<double> get_weights(); // Returns list of weights
};
