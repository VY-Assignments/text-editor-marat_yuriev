#include "pch.h"
#include "cipher.h"
#include "cipher_api.h"
#include <cstring>
#include <cctype>


CaesarCipher::CaesarCipher(int key) : key_(key)
{
    key_ = ((key_ % 26) + 26) % 26;
}

char CaesarCipher::shiftChar(char c, int shift) {
    if (std::isalpha(c)) {
        char base = std::isupper(c) ? 'A' : 'a';
        return base + (c - base + shift + 26) % 26;
    }
    return c;
}

std::string CaesarCipher::encrypt(const std::string& text) {
    std::string result;
    for (char c : text) {
        result += shiftChar(c, key_);
    }
    return result;
}

std::string CaesarCipher::decrypt(const std::string& text) {
    std::string result;
    for (char c : text) {
        result += shiftChar(c, -key_);
    }
    return result;
}






VigenereCipher::VigenereCipher(const std::string& key) : key_(key) {
    for (char& c : key_) {
        c = std::toupper(c);
    }
}

char VigenereCipher::shiftChar(char c, int shift) {
    if (std::isalpha(c)) {
        char base = std::isupper(c) ? 'A' : 'a';
        return base + (c - base + shift + 26) % 26;
    }
    return c;
}

std::string VigenereCipher::encrypt(const std::string& text) {
    std::string result;
    int keyIndex = 0;

    for (char c : text) {
        if (std::isalpha(c)) {
            int shift = key_[keyIndex % key_.length()] - 'A';
            result += shiftChar(c, shift);
            keyIndex++;
        }
        else {
            result += c;
        }
    }
    return result;
}

std::string VigenereCipher::decrypt(const std::string& text) {
    std::string result;
    int keyIndex = 0;

    for (char c : text) {
        if (std::isalpha(c)) {
            int shift = key_[keyIndex % key_.length()] - 'A';
            result += shiftChar(c, -shift);
            keyIndex++;
        }
        else {
            result += c;
        }
    }
    return result;
}

extern "C" {
    EXPORT cipher_t* cipher_create_caesar(int key) {
        return new CaesarCipher(key);
    }

    EXPORT cipher_t* cipher_create_vigenere(const char* key) {
        return new VigenereCipher(key);
    }

    EXPORT char* cipher_encrypt(cipher_t* cipher, const char* text) {
        Cipher* c = static_cast<Cipher*>(cipher);
        std::string result = c->encrypt(text);

        char* output = new char[result.length() + 1];
        strcpy_s(output, result.length() + 1, result.c_str());
        return output;
    }

    EXPORT char* cipher_decrypt(cipher_t* cipher, const char* text) {
        Cipher* c = static_cast<Cipher*>(cipher);
        std::string result = c->decrypt(text);

        char* output = new char[result.length() + 1];
        strcpy_s(output, result.length() + 1, result.c_str());
        return output;
    }

    EXPORT void cipher_destroy(cipher_t* cipher) {
        Cipher* c = static_cast<Cipher*>(cipher);
        delete c;
    }

    EXPORT void cipher_free(char* str) {
        delete[] str;
    }
}
