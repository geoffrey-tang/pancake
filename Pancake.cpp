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
    string value; //= to_string(gen());
    for (int i = 0; i < num; i++) {
        string replica = key + to_string(i);
        value = replica + "_value";
        d.init_insert(replica, value);
    }
}

vector<string> Pancake::create_secure_read_batch(DataStore& d, string query, int B) {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution coin(1.0 / (2 * real_key_freq.size() * alpha)); //true = real query, false = fake query
    client_pos.clear();
    vector<string> batch;
    if (!query.empty()) {
        query_queue.push(make_pair(query, to_string(gen() % (int)key_replica_num[query])));
    }
    //cout << query_queue.front() << endl;
    string k;
    string r;
    for (int i = 0; i < B; i++) {
        if (coin(gen)) {
            //cout << "real flipped; ";
            if (!query_queue.empty()) {
                //cout << "added query" << endl;
                batch.push_back(query_queue.front().first + query_queue.front().second);
                sent_batch.push_back(make_pair(query_queue.front().first, query_queue.front().second));
                client_pos.push_back(i);
                query_queue.pop();
            }
            else {
                //cout << "added real dist" << endl;
                k = dist.choose_sample();
                r = to_string(gen() % (int)key_replica_num[k]);
                batch.push_back(k + r);
                sent_batch.push_back(make_pair(k, r));
            }
        }
        else {
            //cout << "fake flipped; ";
            k = fake_dist.choose_sample();
            //cout << "added " << k << endl;
            r = to_string(gen() % (int)key_replica_num[k]);
            batch.push_back(k + r);
            sent_batch.push_back(make_pair(k, r));
        }
    }
    return batch;
}

bool Pancake::check_update(const string& key, int repl_num) {
    if (updates.count(key) > 0)
        return updates[key].second[repl_num];
    else
        return true;
}

vector<pair<string, string>> Pancake::create_secure_write_batch(DataStore& d, vector<string> vals) {
    vector<pair<string, string>> batch;
    for (int i = 0; i < sent_batch.size(); i++) {
        //cout << sent_batch[i].first + sent_batch[i].second << endl;
        if (!check_update(sent_batch[i].first, stoi(sent_batch[i].second))) {
            batch.push_back(make_pair(sent_batch[i].first + sent_batch[i].second, updates[sent_batch[i].first].first));
        }
        else {
            batch.push_back(make_pair(sent_batch[i].first + sent_batch[i].second, vals[i]));
        }
    }
    for(auto i : client_pos) {
        cout << "READ: {" << batch[i].first << ", " << batch[i].second << "}" << endl;
    }
    sent_batch.clear();
    return batch;
}

void Pancake::access(DataStore &d, char query_type, string key, string val, int B) {
    vector<string> read_batch;
    vector<string> read_return;
    vector<pair<string, string>> write_batch;
    cout << "QUERY: " << query_type << endl;
    if (query_type == 'R') {
        read_batch = create_secure_read_batch(d, key, B);
        read_return = d.process_read(read_batch);
        write_batch = create_secure_write_batch(d, read_return);
        d.process_write(write_batch);
    }
    else if (query_type == 'W') {
        vector<bool> updated(key_replica_num[key], false);
        updates[key] = make_pair(val, updated);
        read_batch = create_secure_read_batch(d, "", B);
        read_return = d.process_read(read_batch);
        write_batch = create_secure_write_batch(d, read_return);
        d.process_write(write_batch);
    }
    /*cout << "READ_BATCH: ";
    for (auto i : read_batch) {
        cout << i << " ";
    }
    cout << endl << "READ_RETURN: ";
    for (auto i : read_return) {
        cout << i << " ";
    }
    cout << endl << "WRITE_BATCH: ";
    for (auto i : write_batch) {
        cout << "{" << i.first << ", " << i.second << "} ";
    }
    cout << endl << "--------------------------------" << endl;;*/
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
    cout << endl << "================================================" << endl;
    pancake.access(data, 'R', "a", "", 5);
    pancake.access(data, 'W', "a", "new value!", 5);
    pancake.access(data, 'R', "a", "", 5);
    pancake.access(data, 'W', "b", "a different value", 5);
    pancake.access(data, 'R', "b", "", 5);
    pancake.access(data, 'R', "c", "", 5);
    /*map<string, int> m;
    for (auto i : sent) {
        ++m[i];
        cout << i << endl;
    }
    for (auto i : m)
        cout << i.first << " appeared " << i.second << " times" << endl;
    vector<string> recv = data.process_read(sent);
    cout << "Return" << endl;
    for (auto i : recv)
        cout << i << endl;*/
    cout << endl << "================================================" << endl;
    data.print_elements();
}
