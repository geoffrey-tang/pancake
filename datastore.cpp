#include "datastore.h"

using namespace std;

DataStore::DataStore() = default;

void DataStore::init_insert(string key, string val) {
	key_value_store[key] = val;
}

vector<string> DataStore::process_read(vector<string> batch) {
	vector<string> server_return;
	for (auto i : batch)
		server_return.push_back(key_value_store.find(i)->second);
	return server_return;
}

void DataStore::process_write(vector<pair<string, string>> batch) {
	for (auto i : batch) {
		key_value_store[i.first] = i.second;
	}
}

void DataStore::print_elements() {
	for (auto i : key_value_store)
	{
		cout << "{" << i.first << ", " << i.second << "}" << endl;
	}
}
