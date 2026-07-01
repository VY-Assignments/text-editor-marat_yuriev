#include "TextEditor.h"

#ifdef _WIN32
static const char* kDefaultCipherLibrary = "CipherLib.dll";
#else
static const char* kDefaultCipherLibrary = "./libCipherLib.so";
#endif

int main()
{
    TextEditor editor(kDefaultCipherLibrary);
    editor.Run();
    return 0;
}
