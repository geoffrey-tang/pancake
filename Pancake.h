#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include <random>
#include "distribution.h"
#include "datastore.h"

using namespace std;

class Pancake {
private:
	unordered_map<string, double> real_key_freq;
	unordered_map<string, double> key_replica_num;
	Distribution dist;
	Distribution fake_dist;
	queue<string> query_queue;
	double alpha;
	double delta;
public:
	void init(DataStore &d, Distribution &distro, double alpha, double delta);
	void add_replicas(DataStore &d, const string& key, int num);
	vector<string> create_secure_access_batch(DataStore& d, string query, int B);
};
