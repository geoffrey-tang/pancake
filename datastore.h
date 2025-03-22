#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

class DataStore {
private:
	map<string, string> key_value_store; // Key value pair database
public:
	DataStore();
	void init_insert(string key, string val); // Used for adding replicas during initialization
	vector<string> process_read(vector<string> batch); // Generate server response for a read batch
	void process_write(vector<pair<string, string>> batch); // Writes to database from batch
	void print_elements(); // Testing print function
};