#include "TextEditor.h"

#ifdef _WIN32
static const char* kDefaultCipherLibrary = "DLL.dll";
#else
static const char* kDefaultCipherLibrary = "./libDLL.so";
#endif

int main()
{
    TextEditor editor(kDefaultCipherLibrary);
    editor.Run();
    return 0;
}
