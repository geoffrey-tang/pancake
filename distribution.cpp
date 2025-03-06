#include "distribution.h"

Distribution::Distribution() = default;

Distribution::Distribution(vector<string> &i, vector<double> &w) {
    discrete_distribution<int> new_dist(w.begin(), w.end());
    dist = new_dist;
    items = i;
    weights = w;
}

vector<string> Distribution::get_items() {
    return items;
}

vector<double> Distribution::get_weights() {
    return weights;
}

string Distribution::choose_sample() {
    random_device rd;
    mt19937 gen(rd());
    return items[dist(gen)];
}