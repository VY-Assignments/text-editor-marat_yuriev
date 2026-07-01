#pragma once
#include <string>

class Cipher {
public:
    virtual std::string encrypt(const std::string& text) = 0;
    virtual std::string decrypt(const std::string& text) = 0;
    virtual ~Cipher() = default;
};

class CaesarCipher : public Cipher {
private:
    int key_;
    char shiftChar(char c, int shift);
public:
    CaesarCipher(int key);
    std::string encrypt(const std::string& text) override;
    std::string decrypt(const std::string& text) override;
};

class VigenereCipher : public Cipher {
private:
    std::string key_;
    char shiftChar(char c, int shift);
public:
    VigenereCipher(const std::string& key);
    std::string encrypt(const std::string& text) override;
    std::string decrypt(const std::string& text) override;
};

class PlayfairCipher : public Cipher {
private:
    char square_[5][5];
    void buildSquare(const std::string& key);
    void findPos(char c, int& r, int& cnd) const;
    static char normalize(char c);
public:
    PlayfairCipher(const std::string& key);
    std::string encrypt(const std::string& text) override;
    std::string decrypt(const std::string& text) override;
};
