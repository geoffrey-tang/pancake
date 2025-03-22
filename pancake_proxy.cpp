#include "Pancake.h"
#include "encryption.h"
#include <fstream>
#include <iostream>
#include <string>
#include <numeric>
#include <time.h>

using namespace std;

vector<string> split_string(string str, char splitter) {
    vector<string> result;
    string current = "";
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == splitter) {
            if (current != "") {
                result.push_back(current);
                current = "";
            }
            continue;
        }
        current += str[i];
    }
    if (current.size() != 0)
        result.push_back(current);
    return result;
}

int main() {
	ifstream file("workloads/workaload10k.txt"); // Change this to run a different workload
	string line;
    vector<string> parse;
    vector<string> items;
    int key_count = 1000;
    vector<double> weights(key_count);
    random_device rd;
    mt19937 gen(rd());
    while (getline(file, line)) {
        parse = split_string(line, ' ');
        items.push_back(parse[2]);
    }
    generate(weights.begin(), weights.end(), gen);
    const double total = accumulate(weights.begin(), weights.end(), 0.0);
    for (double& value : weights) value /= total;
    file.close();

    cout << "Starting..." << endl;
    DataStore data;
    Pancake pancake;
    Distribution distribution(items, weights);
    Encryption encrypt;
    pancake.init(encrypt, data, distribution, 1.0 / distribution.get_items().size());
    
    time_t timer_start;
    time(&timer_start);
    ifstream runfile("workloads/workarun10k.txt"); // Change this to run a different workload
    while (getline(runfile, line)) {
        parse = split_string(line, ' ');
        if (parse[0] == "READ") {
            pancake.access(data, 'R', parse[2], "", 3);
        }
        else if (parse[0] == "UPDATE") {
            string update;
            for (int i = 4; i < parse.size() - 1; i++) {
                update += parse[i];
            }
            update = update.substr(7, update.size());
            pancake.access(data, 'W', parse[2], update, 3);
        }
    }
    runfile.close();
    time_t timer_end;
    time(&timer_end);
    double elapsed = timer_end - timer_start;
    cout << "Pancake took " << elapsed << " seconds to run." << endl;
}