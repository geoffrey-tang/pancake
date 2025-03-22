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
#include "histogram.h"
#include "encryption.h"

using namespace std;

// Pancake initialization
// Generates replicas and fake distribution
void Pancake::init(Encryption &e, DataStore &d, Distribution &distro, double A) {
    // Setting up variables
    alpha = A;
    dist = distro;
    encryption = e;
    vector<double> fake_weights;
    vector<string> keys = dist.get_items();
    vector<double> weights = dist.get_weights();
    int index = 0;
    int replica_count = 0;

    // Generating fake distribution and replicas
    for (string key : keys) {
        double pi_k = weights[index]; // Get real distribution
        double r_k = ceil(pi_k / alpha); // Get number of replicas to create
        double pi_f = (alpha - (pi_k / r_k)) / ((2 * keys.size() * alpha) - 1); // Generate fake frequency
        // Updating proxy's maps (key: real frequency and key: replica count) and adding replicas
        real_key_freq[key] = pi_k;
        key_replica_num[key] = r_k;
        fake_weights.push_back(pi_f * r_k);
        add_replicas(d, key, r_k);
        replica_count += r_k;
        index++;
    }
    // Generating dummy replicas
    double dummy_count = 0.0;
    dummy = encryption.random_string(16);
    if ((2 * keys.size()) - replica_count >= 0) { // Generate dummy replicas such that total replicas is 2N
        dummy_count = (2 * keys.size()) - replica_count;
        add_replicas(d, dummy, dummy_count);
        double dummy_pi_f = (alpha) / ((2 * keys.size() * alpha) - 1); // Generate dummy key frequency on fake distribution
        key_replica_num[dummy] = dummy_count;
        fake_weights.push_back(dummy_pi_f * dummy_count);
        keys.push_back(dummy);
    }
    fake_dist = Distribution(keys, fake_weights);
    delta = 1 / (2 * real_key_freq.size() * alpha);
}

// Only for iniialization
// Set up datastore with replicas
void Pancake::add_replicas(DataStore &d, const string& key, int num) {
    random_device rd;
    mt19937 gen(rd());
    string value = encryption.random_string(16);
    for (int i = 0; i < num; i++) {
        string replica = key + to_string(i);
        string e_key = encryption.hmac(replica);
        string e_val = encryption.enc(value);
        d.init_insert(e_key, e_val);
    }
}

// Generates a read batch of size B
vector<string> Pancake::create_secure_read_batch(DataStore& d, string query, int B) {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution coin(delta); // Pick either real or fake query based on alpha; with alpha = 1/N, this is a 1/2 chance
    client_pos.clear();
    vector<string> batch;
    // Push client query onto query queue
    if (!query.empty()) {
        query_queue.push(make_pair(query, to_string(gen() % (int)key_replica_num[query])));
    }
    string k;
    string r;
    string q;
    // Creating batch of size B
    for (int i = 0; i < B; i++) { 
        if (coin(gen)) { // Picked real distribution
            if (!query_queue.empty()) { // If a client query is waiting, inject it into the batch
                q = query_queue.front().first + query_queue.front().second;
                batch.push_back(encryption.hmac(q));
                sent_batch.push_back(make_pair(query_queue.front().first, query_queue.front().second));
                client_pos.push_back(i);
                query_queue.pop();
            }
            else { // Otherwise, sample a replica from the real distribution
                k = dist.choose_sample();
                r = to_string(gen() % (int)key_replica_num[k]);
                q = k + r;
                batch.push_back(encryption.hmac(q));
                sent_batch.push_back(make_pair(k, r));
            }
        }
        else { // Picked fake distribution; sample replica from fake distribution
            k = fake_dist.choose_sample();
            r = to_string(gen() % (int)key_replica_num[k]);
            q = k + r;
            batch.push_back(encryption.hmac(q));
            sent_batch.push_back(make_pair(k, r));
        }
    }
    return batch;
}

// Checks the update cache if a specific replica needs to be updated
bool Pancake::check_update(const string& key, int repl_num) {
    if (updates.count(key) > 0)
        return updates[key].second[repl_num];
    else
        return true;
}

// Generates a write batch 
vector<pair<string, string>> Pancake::create_secure_write_batch(DataStore& d, vector<string> vals) {
    vector<pair<string, string>> batch;
    for (int i = 0; i < sent_batch.size(); i++) {
        string k;
        string v;
        // If an update is needed, then write the updated value to the replica
        if (!check_update(sent_batch[i].first, stoi(sent_batch[i].second))) {
            k = sent_batch[i].first + sent_batch[i].second;
            v = updates[sent_batch[i].first].first;
            updates[sent_batch[i].first].second[stoi(sent_batch[i].second)] = true;
            batch.push_back(make_pair(encryption.enc(k), encryption.enc(v)));
        }
        // If an update is not needed, then writeback the original value
        else {
            k = sent_batch[i].first + sent_batch[i].second;
            v = vals[i];
            batch.push_back(make_pair(encryption.enc(k), v));
        }
    }
    // UNCOMMENT TO SHOW READ QUERY RESPONSES
    /*
    for (auto i : client_pos) {
        cout << "READ: {" << encryption.dec(batch[i].first) << ", " << encryption.dec(batch[i].second) << "}" << endl;
    }
    */
    sent_batch.clear();
    return batch;
}

// Accesses the datastore
// All accesses are a read followed by a write in order to provide obliviousness
void Pancake::access(DataStore &d, char query_type, string key, string val, int B) {
    vector<string> read_batch;
    vector<string> read_return;
    vector<pair<string, string>> write_batch;
    // Read queries
    if (query_type == 'R') {
        read_batch = create_secure_read_batch(d, key, B);
        read_return = d.process_read(read_batch);
        write_batch = create_secure_write_batch(d, read_return);
        d.process_write(write_batch);
    }
    // Write queries
    else if (query_type == 'W') {
        vector<bool> updated(key_replica_num[key], false);
        updates[key] = make_pair(val, updated);
        read_batch = create_secure_read_batch(d, "", B);
        read_return = d.process_read(read_batch);
        write_batch = create_secure_write_batch(d, read_return);
        d.process_write(write_batch);
    }
}

