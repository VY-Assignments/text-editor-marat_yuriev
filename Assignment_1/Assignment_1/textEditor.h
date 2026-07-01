#pragma once
#include "text.h"
#include "cipherLoader.h"
#include <string>
#include <memory>

class TextEditor
{
private:
    Text document;

    std::string defaultLibraryPath;

    void PrintMenu() const;

    void CommandAppendText();
    void CommandStartNewLine();
    void CommandAddChecklistItem();
    void CommandAddContact();
    void CommandToggleChecklist();
    void CommandInsertText();
    void CommandDeleteChars();
    void CommandMoveCursor();
    void CommandSearch();
    void CommandCut();
    void CommandCopy();
    void CommandPaste();
    void CommandSaveToFile();
    void CommandLoadFromFile();

    void CommandEncryptCurrentTextToFile();
    void CommandLoadAndDecryptFile();
    void CommandEncryptOrDecryptArbitraryFile();

    CipherHandle PromptForCipher() const;

public:
    explicit TextEditor(const std::string& defaultLibraryPath);

    void Run();
};
