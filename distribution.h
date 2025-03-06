#pragma once
#include <random>
#include <vector>

using namespace std;

class Distribution {
private:
    discrete_distribution<int> dist;
    vector<string> items;
    vector<double> weights;
public:
    Distribution();
    Distribution(vector<string> &i, vector<double> &w);
    string choose_sample();
    vector<string> get_items();
    vector<double> get_weights();
};
