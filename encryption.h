#pragma once

#include <openssl/hmac.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <array>
#include <iostream>
#include <string>
#include <random>

using namespace std;

class Encryption {
private:
	string label_key; // HMAC key for replica keys
	string value_key; // AES key for replica values
	string value_iv; // AES nonce for replica values
public:
	Encryption(); // Initialization
	string hmac(string& plaintext); // Applies HMAC to a string
	string enc(string& plaintext); // AES encryption
	string dec(string& ciphertext); // AES decryption
	void generate_keys(); // Random key generation
	string random_string(int size); // Random string generation
};