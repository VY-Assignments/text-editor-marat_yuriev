#include "cipherLoader.h"
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace
{
    using CreateCaesarFn = void* (*)(int);
    using CreateVigenereFn = void* (*)(const char*);
    using CreatePlayfairFn = void* (*)(const char*);
    using EncryptFn = char* (*)(void*, const char*);
    using DecryptFn = char* (*)(void*, const char*);
    using DestroyFn = void  (*)(void*);
    using FreeFn = void  (*)(char*);
}

CipherHandle::CipherHandle(const std::string& libraryPath, Type type, int caesarKey, const std::string& keywordKey)
    : libraryPath(libraryPath), type(type), caesarKey(caesarKey), keywordKey(keywordKey)
{
}

CipherHandle CipherHandle::ForCaesar(const std::string& libraryPath, int key)
{
    return CipherHandle(libraryPath, Type::Caesar, key, "");
}

CipherHandle CipherHandle::ForVigenere(const std::string& libraryPath, const std::string& key)
{
    return CipherHandle(libraryPath, Type::Vigenere, 0, key);
}

CipherHandle CipherHandle::ForPlayfair(const std::string& libraryPath, const std::string& key)
{
    return CipherHandle(libraryPath, Type::Playfair, 0, key);
}

std::string CipherHandle::RunOperation(const std::string& text, bool encrypt) const
{
#ifdef _WIN32
    HMODULE handle = LoadLibraryA(libraryPath.c_str());
    if (!handle)
        throw std::runtime_error("Failed to load cipher library: " + libraryPath);

    auto createCaesar = reinterpret_cast<CreateCaesarFn>(GetProcAddress(handle, "cipher_create_caesar"));
    auto createVigenere = reinterpret_cast<CreateVigenereFn>(GetProcAddress(handle, "cipher_create_vigenere"));
    auto createPlayfair = reinterpret_cast<CreatePlayfairFn>(GetProcAddress(handle, "cipher_create_playfair"));
    auto encryptFn = reinterpret_cast<EncryptFn>(GetProcAddress(handle, "cipher_encrypt"));
    auto decryptFn = reinterpret_cast<DecryptFn>(GetProcAddress(handle, "cipher_decrypt"));
    auto destroyFn = reinterpret_cast<DestroyFn>(GetProcAddress(handle, "cipher_destroy"));
    auto freeFn = reinterpret_cast<FreeFn>(GetProcAddress(handle, "cipher_free"));

    if (!createCaesar || !createVigenere || !createPlayfair || !encryptFn || !decryptFn || !destroyFn || !freeFn)
    {
        FreeLibrary(handle);
        throw std::runtime_error("Cipher library is missing one or more required exports");
    }
#else
    void* handle = dlopen(libraryPath.c_str(), RTLD_NOW);
    if (!handle)
        throw std::runtime_error("Failed to load cipher library: " + libraryPath);

    auto createCaesar = reinterpret_cast<CreateCaesarFn>(dlsym(handle, "cipher_create_caesar"));
    auto createVigenere = reinterpret_cast<CreateVigenereFn>(dlsym(handle, "cipher_create_vigenere"));
    auto createPlayfair = reinterpret_cast<CreatePlayfairFn>(dlsym(handle, "cipher_create_playfair"));
    auto encryptFn = reinterpret_cast<EncryptFn>(dlsym(handle, "cipher_encrypt"));
    auto decryptFn = reinterpret_cast<DecryptFn>(dlsym(handle, "cipher_decrypt"));
    auto destroyFn = reinterpret_cast<DestroyFn>(dlsym(handle, "cipher_destroy"));
    auto freeFn = reinterpret_cast<FreeFn>(dlsym(handle, "cipher_free"));

    if (!createCaesar || !createVigenere || !createPlayfair || !encryptFn || !decryptFn || !destroyFn || !freeFn)
    {
        dlclose(handle);
        throw std::runtime_error("Cipher library is missing one or more required exports");
    }
#endif

    void* cipherHandle = nullptr;
    switch (type)
    {
    case Type::Caesar:
        cipherHandle = createCaesar(caesarKey);
        break;
    case Type::Vigenere:
        cipherHandle = createVigenere(keywordKey.c_str());
        break;
    case Type::Playfair:
        cipherHandle = createPlayfair(keywordKey.c_str());
        break;
    }

    if (!cipherHandle)
    {
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        throw std::runtime_error("Failed to create cipher instance (check the key)");
    }

    char* resultPtr = encrypt ? encryptFn(cipherHandle, text.c_str())
        : decryptFn(cipherHandle, text.c_str());

    std::string result = resultPtr ? resultPtr : "";

    if (resultPtr)
        freeFn(resultPtr);

    destroyFn(cipherHandle);

#ifdef _WIN32
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif

    return result;
}

std::string CipherHandle::Encrypt(const std::string& text) const
{
    return RunOperation(text, true);
}

std::string CipherHandle::Decrypt(const std::string& text) const
{
    return RunOperation(text, false);
}
