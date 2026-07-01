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

    std::string encryptedBuffer;
    bool hasEncryptedBuffer = false;

    void PrintMenu() const;

    void CommandAppendText();
    void CommandStartNewLine();
    void CommandAddChecklistItem();
    void CommandAddContact();
    void CommandToggleChecklist();
    void CommandDeleteLastChars();
    void CommandDeleteRow();
    void CommandSearch();
    void CommandCut();
    void CommandCopy();
    void CommandPaste();
    void CommandSaveToFile();
    void CommandLoadFromFile();

    void CommandEncryptDocument();
    void CommandSaveEncryptedToFile();
    void CommandLoadEncryptedFromFile();
    void CommandDecryptDocument();

    CipherHandle PromptForCipher() const;

public:
    explicit TextEditor(const std::string& defaultLibraryPath);

    void Run();
};
