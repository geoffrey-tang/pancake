#include "encryption.h"

using namespace std;

// Initialization function
Encryption::Encryption() {
    generate_keys();
}

// Generates random keys for HMAC and AES
void Encryption::generate_keys() {
    label_key = random_string(32);
    value_key = random_string(32);
    value_iv = random_string(16);
}

// Applies HMAC to a string
string Encryption::hmac(string& plaintext) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int hmac_len;
    HMAC(EVP_sha256(), label_key.data(), label_key.size(), reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size(), result, &hmac_len);
    return string{ reinterpret_cast<char const*>(result), hmac_len };
}

// Encrypts plaintext with AES
string Encryption::enc(string& plaintext) {
    EVP_CIPHER_CTX* ctx;
    unsigned char ciphertext[256];
    int len;
    int ciphertext_len;
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(value_key.data()), reinterpret_cast<const unsigned char*>(value_iv.data()));
    EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    ciphertext[ciphertext_len] = '\0';
    EVP_CIPHER_CTX_free(ctx);
    return string(reinterpret_cast<char const*>(ciphertext));
}

// Decrypts ciphertext with AES
string Encryption::dec(string& ciphertext) {
    EVP_CIPHER_CTX* ctx;
    int len;
    int plaintext_len;
    unsigned char plaintext[256];
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(value_key.data()), reinterpret_cast<const unsigned char*>(value_iv.data()));
    EVP_DecryptUpdate(ctx, plaintext, &len, reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    plaintext[plaintext_len] = '\0';
    EVP_CIPHER_CTX_free(ctx);
    return string(reinterpret_cast<char const*>(plaintext));
}

// Generates a random string
string Encryption::random_string(int size) {
    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> char_picker(0, chars.size() - 1);
    string rand_str;
    for (int i = 0; i < size; i++) {
        rand_str += chars[char_picker(gen)];
    }
    return rand_str;
}