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









PlayfairCipher::PlayfairCipher(const std::string& key) {
    buildSquare(key);
}

char PlayfairCipher::normalize(char c) {
    c = std::toupper(c);
    if (c == 'J') return 'I';
    return c;
}

void PlayfairCipher::buildSquare(const std::string& key) {
    bool used[26] = { false };
    used['J' - 'A'] = true;

    std::string pool;
    for (char ch : key) {
        if (!std::isalpha(ch)) continue;
        char c = normalize(ch);
        if (!used[c - 'A']) {
            pool.push_back(c);
            used[c - 'A'] = true;
        }
    }

    for (char c = 'A'; c <= 'Z'; ++c) {
        if (c == 'J') continue;
        if (!used[c - 'A']) {
            pool.push_back(c);
            used[c - 'A'] = true;
        }
    }

    int idx = 0;
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            square_[r][c] = pool[idx++];
        }
    }
}

void PlayfairCipher::findPos(char ch, int& r, int& cnd) const {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (square_[i][j] == ch) {
                r = i; cnd = j; return;
            }
        }
    }
}

static std::string prepare_text(const std::string& text) {
    std::string s;
    for (char ch : text) {
        if (std::isalpha(ch)) {
            char c = std::toupper(ch);
            if (c == 'J') c = 'I';
            s.push_back(c);
        }
    }
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        char a = s[i];
        char b = (i + 1 < s.size()) ? s[i + 1] : 0;
        if (b == 0) {
            out.push_back(a);
            out.push_back('X');
        }
        else if (a == b) {
            out.push_back(a);
            out.push_back('X');
        }
        else {
            out.push_back(a);
            out.push_back(b);
            ++i;
        }
    }
    return out;
}

std::string PlayfairCipher::encrypt(const std::string& text) {
    std::string prepared = prepare_text(text);
    std::string result;
    for (size_t i = 0; i < prepared.size(); i += 2) {
        char a = prepared[i];
        char b = prepared[i + 1];
        int ra, ca, rb, cb;
        findPos(a, ra, ca);
        findPos(b, rb, cb);
        if (ra == rb) {
            result.push_back(square_[ra][(ca + 1) % 5]);
            result.push_back(square_[rb][(cb + 1) % 5]);
        }
        else if (ca == cb) {
            result.push_back(square_[(ra + 1) % 5][ca]);
            result.push_back(square_[(rb + 1) % 5][cb]);
        }
        else {
            result.push_back(square_[ra][cb]);
            result.push_back(square_[rb][ca]);
        }
    }
    return result;
}

std::string PlayfairCipher::decrypt(const std::string& text) {
    std::string s;
    for (char ch : text) if (std::isalpha(ch)) s.push_back(std::toupper(ch));
    std::string result;
    for (size_t i = 0; i + 1 < s.size(); i += 2) {
        char a = s[i];
        char b = s[i + 1];
        int ra, ca, rb, cb;
        findPos(a, ra, ca);
        findPos(b, rb, cb);
        if (ra == rb) {
            result.push_back(square_[ra][(ca + 5 - 1) % 5]);
            result.push_back(square_[rb][(cb + 5 - 1) % 5]);
        }
        else if (ca == cb) {
            result.push_back(square_[(ra + 5 - 1) % 5][ca]);
            result.push_back(square_[(rb + 5 - 1) % 5][cb]);
        }
        else {
            result.push_back(square_[ra][cb]);
            result.push_back(square_[rb][ca]);
        }
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

    EXPORT cipher_t* cipher_create_playfair(const char* key) {
        return new PlayfairCipher(key);
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
