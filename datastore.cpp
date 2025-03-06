#include "datastore.h"

using namespace std;

DataStore::DataStore() = default;

void DataStore::init_insert(string key, string val) {
	key_value_store[key] = val;
}

vector<string> DataStore::process_get(vector<string> batch) {
	vector<string> server_return;
	for (auto i : batch)
		server_return.push_back(key_value_store[i]);
	return server_return;
}

void DataStore::print_elements() {
	for (auto i : key_value_store)
	{
		cout << "{" << i.first << ", " << i.second << "}" << endl;
	}
}
