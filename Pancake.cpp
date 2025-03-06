// Pancake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/* compilation: gcc -o client client.c -lssl -lcrypto */

#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <random>
#include <map>
#include "distribution.h"
#include "Pancake.h"
#include "datastore.h"

using namespace std;

void Pancake::init(DataStore &d, Distribution &distro, double A, double D) {
    alpha = A;
    delta = D;
    dist = distro;
    vector<double> fake_weights;
    vector<string> keys = dist.get_items();
    vector<double> weights = dist.get_weights();
    int index = 0;
    int replica_count = 0;
    for (string key : keys) {
        double pi_k = weights[index]; //real distribution
        double r_k = ceil(pi_k / alpha); //number of replicas
        double pi_f = (alpha - (pi_k / r_k)) / ((2 * keys.size() * alpha) - 1); //fake distribution frequency
        real_key_freq[key] = pi_k;
        key_replica_num[key] = r_k;
        fake_weights.push_back(pi_f * r_k);
        add_replicas(d, key, r_k);
        replica_count += r_k;
        index++;
    }
    double dummy_count = 0.0;
    if ((2 * keys.size()) - replica_count >= 0) {
        dummy_count = (2 * keys.size()) - replica_count;
        add_replicas(d, "dummy", dummy_count);
        double dummy_pi_f = (alpha) / ((2 * keys.size() * alpha) - 1);
        key_replica_num["dummy"] = dummy_count;
        fake_weights.push_back(dummy_pi_f * dummy_count);
        keys.push_back("dummy");
    }
    fake_dist = Distribution(keys, fake_weights);
    cout << "Fake" << endl;
    for (auto i : fake_dist.get_weights()) {
        cout << i << " ";
    }
    cout << endl;
}

void Pancake::add_replicas(DataStore &d, const string& key, int num) {
    random_device rd;
    mt19937 gen(rd());
    string value = to_string(gen());
    for (int i = 0; i < num; i++) {
        string replica = key + to_string(i);
        d.init_insert(replica, value);
    }
}

vector<string> Pancake::create_secure_access_batch(DataStore& d, string query, int B) {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution coin(1.0 / (2 * real_key_freq.size() * alpha)); //true = real query, false = fake query
    vector<string> batch;
    query_queue.push(query + to_string(gen() % (int)key_replica_num[query]));
    //cout << query_queue.front() << endl;
    string k;
    for (int i = 0; i < B; i++) {
        if (coin(gen)) {
            //cout << "real flipped; ";
            if (!query_queue.empty()) {
                //cout << "added query" << endl;
                batch.push_back(query_queue.front());
                query_queue.pop();
            }
            else {
                //cout << "added real dist" << endl;
                k = dist.choose_sample();
                batch.push_back(k + to_string(gen() % (int)key_replica_num[k]));
            }
        }
        else {
            //cout << "fake flipped; ";
            k = fake_dist.choose_sample();
            //cout << "added " << k << endl;
            batch.push_back(k + to_string(gen() % (int)key_replica_num[k]));
        }
    }
    return batch;
}

int main() {
    vector<string> items = { "a", "b", "c", "d", "e"};
    vector<double> weights = { 0.25, 1.0/3.0, 1.0/6.0 , 0.125, 0.125};
    DataStore data;
    Pancake pancake;
    Distribution distribution(items, weights);
    cout << "Starting..." << endl;
    pancake.init(data, distribution, 1.0 / distribution.get_items().size(), 1);
    cout << "Replicas" << endl;
    data.print_elements();
    cout << "Real" << endl;
    for (auto i : distribution.get_weights()) {
        cout << i << " ";
    }
    cout << endl << "Batch" << endl;
    vector<string> sent = pancake.create_secure_access_batch(data, "a", 10);
    map<string, int> m;
    for (auto i : sent) {
        ++m[i];
        cout << i << endl;
    }
    for (auto i : m)
        cout << i.first << " appeared " << i.second << " times" << endl;
    vector<string> recv = data.process_get(sent);
    cout << "Return" << endl;
    for (auto i : recv)
        cout << i << endl;
}
