#pragma once
#include <string>

class CipherHandle
{
public:
    enum class Type { Caesar, Vigenere, Playfair };

    static CipherHandle ForCaesar(const std::string& libraryPath, int key);
    static CipherHandle ForVigenere(const std::string& libraryPath, const std::string& key);
    static CipherHandle ForPlayfair(const std::string& libraryPath, const std::string& key);

    Type GetType() const { return type; }

    std::string Encrypt(const std::string& text) const;
    std::string Decrypt(const std::string& text) const;

private:
    CipherHandle(const std::string& libraryPath, Type type, int caesarKey, const std::string& keywordKey);

    std::string RunOperation(const std::string& text, bool encrypt) const;

    std::string libraryPath;
    Type type;
    int caesarKey;
    std::string keywordKey;
};
