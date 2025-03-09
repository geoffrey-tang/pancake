#pragma once

#include <unordered_map>
#include <map>
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
	queue<pair<string, string>> query_queue;
	vector<pair<string, string>> sent_batch;
	map<string, pair<string, vector<bool>>> updates;
	vector<int> client_pos;
	double alpha;
	double delta;
public:
	void init(DataStore &d, Distribution &distro, double alpha, double delta);
	void add_replicas(DataStore &d, const string& key, int num);
	vector<string> create_secure_read_batch(DataStore& d, string query, int B);
	vector<pair<string, string>> create_secure_write_batch(DataStore& d, vector<string> vals);
	void access(DataStore &d, char query_type, string key, string val, int B); // 'R' for read, 'W' for write
	bool check_update(const string& key, int repl_num);
};
