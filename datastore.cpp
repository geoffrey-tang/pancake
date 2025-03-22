#include "datastore.h"

using namespace std;

DataStore::DataStore() = default;

// Used only for initial replica generation
void DataStore::init_insert(string key, string val) {
	key_value_store[key] = val;
}

// Server response for a read batch
vector<string> DataStore::process_read(vector<string> batch) {
	vector<string> server_return;
	for (auto i : batch) {
		if (key_value_store.find(i) != key_value_store.end()) {
			server_return.push_back(key_value_store.find(i)->second);
		}
	}
	return server_return;
}

// Writes to database from batch
void DataStore::process_write(vector<pair<string, string>> batch) {
	for (auto i : batch) {
		if(!i.second.empty())
			key_value_store[i.first] = i.second;
	}
}

// Testing function to print full database
void DataStore::print_elements() {
	for (auto i : key_value_store)
	{
		cout << "{" << i.first << ", " << i.second << "}" << endl;
	}
}
