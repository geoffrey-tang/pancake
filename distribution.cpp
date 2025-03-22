#include "distribution.h"

Distribution::Distribution() = default;

// Initializes a distribution with items and weights
Distribution::Distribution(vector<string> &i, vector<double> &w) {
    discrete_distribution<int> new_dist(w.begin(), w.end());
    dist = new_dist;
    items = i;
    weights = w;
}

// Returns list of items
vector<string> Distribution::get_items() {
    return items;
}

// Returns list of weights
vector<double> Distribution::get_weights() {
    return weights;
}

// Samples distribution
string Distribution::choose_sample() {
    random_device rd;
    mt19937 gen(rd());
    return items[dist(gen)];
}