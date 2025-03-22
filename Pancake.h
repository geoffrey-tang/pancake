#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <random>
#include "distribution.h"
#include "datastore.h"
#include "encryption.h"

using namespace std;

class Pancake {
private:
	unordered_map<string, double> real_key_freq; // Maps keys to their real frequency
	unordered_map<string, double> key_replica_num; // Maps keys to replica count
	Distribution dist; // Real distribution
	Distribution fake_dist; // Fake distribution
	Encryption encryption; // Encryption engine
	queue<pair<string, string>> query_queue; // Client query queue
	vector<pair<string, string>> sent_batch; // Internal tracking of the read batch being sent
	map<string, pair<string, vector<bool>>> updates; // Update cache; tracks replicas that need updating
	vector<int> client_pos; // Internal tracking of which queries within a batch were client queries
	double alpha; // Alpha threshold; see paper for details
	double delta; // 1/(2N * alpha); determines how often real distribution is selected when generating batches
	string dummy; // Dummy key
public:
	void init(Encryption &e, DataStore& d, Distribution& distro, double A); // Pancake initialization
	void add_replicas(DataStore &d, const string& key, int num); // Initializes datastore
	vector<string> create_secure_read_batch(DataStore& d, string query, int B); // Generates a read batch
	vector<pair<string, string>> create_secure_write_batch(DataStore& d, vector<string> vals); // Generates a write batch
	bool check_update(const string& key, int repl_num); // Accesses the datastore
	void access(DataStore& d, char query_type, string key, string val, int B);
};
