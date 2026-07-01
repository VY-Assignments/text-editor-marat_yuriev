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

#ifdef _WIN32
    std::string DescribeLastError(DWORD err)
    {
        char* buffer = nullptr;
        DWORD size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&buffer), 0, nullptr);

        std::string message = (size > 0 && buffer) ? std::string(buffer, size) : "Unknown error";
        if (buffer)
            LocalFree(buffer);

        while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
            message.pop_back();

        return message;
    }

    std::string ResolveNextToExe(const std::string& libraryPath)
    {
        char exePath[MAX_PATH];
        DWORD len = GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        if (len == 0 || len >= MAX_PATH)
            return "";

        std::string exeDir(exePath, len);
        size_t slashPos = exeDir.find_last_of("\\/");
        if (slashPos == std::string::npos)
            return "";

        return exeDir.substr(0, slashPos + 1) + libraryPath;
    }
#endif
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
    {
        std::string candidate = ResolveNextToExe(libraryPath);
        if (!candidate.empty())
            handle = LoadLibraryA(candidate.c_str());
    }

    if (!handle)
    {
        DWORD err = GetLastError();
        throw std::runtime_error(
            "Failed to load cipher library '" + libraryPath + "': " + DescribeLastError(err) +
            " Check that: (1) the DLL project actually built successfully, "
            "(2) DLL.dll exists in the same folder as the .exe you are running "
            "(the post-build copy step from the tutorial), and "
            "(3) both projects target the same platform (both x64, not one x86/one x64).");
    }

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
        throw std::runtime_error("Cipher library is missing one or more required exports "
            "(check for extern \"C\" around the exports in cipher.cpp)");
    }
#else
    void* handle = dlopen(libraryPath.c_str(), RTLD_NOW);
    if (!handle)
        throw std::runtime_error("Failed to load cipher library: " + libraryPath + " (" + dlerror() + ")");

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
