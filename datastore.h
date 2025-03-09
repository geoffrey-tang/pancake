#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

class DataStore {
private:
	map<string, string> key_value_store;
public:
	DataStore();
	void init_insert(string key, string val);
	vector<string> process_read(vector<string> batch);
	void process_write(vector<pair<string, string>> batch);
	void print_elements(); // for testing
};