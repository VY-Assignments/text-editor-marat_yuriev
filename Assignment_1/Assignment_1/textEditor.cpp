#include "TextEditor.h"
#include <iostream>
#include <fstream>
#include <limits>

namespace
{
    std::string ReadLine()
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    int ReadInt()
    {
        int value;
        while (!(std::cin >> value))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number: ";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }

    std::string ReadFileText(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Could not open file: " + path);

        return std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
    }

    void WriteFileText(const std::string& path, const std::string& data)
    {
        std::ofstream file(path);
        if (!file)
            throw std::runtime_error("Could not open file for writing: " + path);

        file << data;
    }
}

TextEditor::TextEditor(const std::string& defaultLibraryPath)
    : defaultLibraryPath(defaultLibraryPath)
{
}

void TextEditor::PrintMenu() const
{
    std::cout <<
        "\n--- Text Editor ---\n"
        " 1. Append text (continues current text block)\n"
        " 2. Start new (blank) line\n"
        " 3. Add checklist item\n"
        " 4. Add contact\n"
        " 5. Toggle checklist item (by row number)\n"
        " 6. Delete last N characters from current text\n"
        " 7. Delete a row (by row number)\n"
        " 8. Search\n"
        " 9. Cut rows (by start row + count)\n"
        "10. Copy rows (by start row + count)\n"
        "11. Paste rows (at a row, or at the end)\n"
        "12. Undo\n"
        "13. Redo\n"
        "14. Print document\n"
        "15. Save document to plain file\n"
        "16. Load document from plain file\n"
        "17. Encrypt document (choose cipher + key)\n"
        "18. Save encrypted data to file\n"
        "19. Load encrypted file\n"
        "20. Decrypt loaded data\n"
        "21. Print encrypted data (raw ciphertext, no decryption)\n"
        " 0. Exit\n";
}

void TextEditor::CommandAppendText()
{
    std::cout << "Enter text: ";
    document.AppendText(ReadLine());
}

void TextEditor::CommandStartNewLine()
{
    document.StartNewLine();
}

void TextEditor::CommandAddChecklistItem()
{
    std::cout << "Enter checklist item text: ";
    std::string item = ReadLine();
    std::cout << "Checked already? (1 = yes, 0 = no): ";
    int checked = ReadInt();
    document.AddChecklistItem(item, checked != 0);
}

void TextEditor::CommandAddContact()
{
    std::cout << "Name: ";
    std::string name = ReadLine();
    std::cout << "Surname: ";
    std::string surname = ReadLine();
    std::cout << "E-mail: ";
    std::string email = ReadLine();
    document.AddContact(name, surname, email);
}

void TextEditor::CommandToggleChecklist()
{
    std::cout << "Row number to toggle: ";
    document.ToggleChecklistItem(ReadInt());
}

void TextEditor::CommandDeleteLastChars()
{
    std::cout << "How many characters to delete from the end: ";
    document.DeleteLastChars(ReadInt());
}

void TextEditor::CommandDeleteRow()
{
    std::cout << "Row number to delete: ";
    document.DeleteRow(ReadInt());
}

void TextEditor::CommandSearch()
{
    std::cout << "Text to search for: ";
    document.SearchText(ReadLine());
}

void TextEditor::CommandCut()
{
    std::cout << "Start row: ";
    int startRow = ReadInt();
    std::cout << "How many rows to cut: ";
    int count = ReadInt();
    document.CutRows(startRow, count);
}

void TextEditor::CommandCopy()
{
    std::cout << "Start row: ";
    int startRow = ReadInt();
    std::cout << "How many rows to copy: ";
    int count = ReadInt();
    document.CopyRows(startRow, count);
}

void TextEditor::CommandPaste()
{
    std::cout << "Insert at row (any out-of-range number = paste at the end): ";
    document.PasteRows(ReadInt());
}

void TextEditor::CommandSaveToFile()
{
    std::cout << "Enter file name to save: ";
    std::string path = ReadLine();
    std::ofstream file(path);
    if (!file)
    {
        std::cout << "Error opening file\n";
        return;
    }
    file << document.ToPlainString();
    std::cout << "Text saved successfully\n";
}

void TextEditor::CommandLoadFromFile()
{
    std::cout << "Enter file name to load: ";
    std::string path = ReadLine();
    try
    {
        document.LoadFromPlainString(ReadFileText(path));
        std::cout << "Text loaded successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

CipherHandle TextEditor::PromptForCipher() const
{
    std::cout << "Choose cipher (1 = Caesar, 2 = Vigenere, 3 = Playfair): ";
    int choice = ReadInt();

    if (choice == 1)
    {
        std::cout << "Enter shift key (integer): ";
        int shiftKey = ReadInt();
        return CipherHandle::ForCaesar(defaultLibraryPath, shiftKey);
    }

    std::cout << "Enter keyword (letters only): ";
    std::string keyword = ReadLine();

    if (choice == 3)
        return CipherHandle::ForPlayfair(defaultLibraryPath, keyword);

    return CipherHandle::ForVigenere(defaultLibraryPath, keyword);
}

void TextEditor::CommandEncryptDocument()
{
    try
    {
        CipherHandle cipher = PromptForCipher();

        if (cipher.GetType() == CipherHandle::Type::Playfair)
        {
            std::cout << "Warning: Playfair strips punctuation/digits and "
                "uppercases everything, which will corrupt this "
                "document's row structure (TEXT|... / CHECK|... / "
                "CONTACT|...) and make it impossible to decrypt back "
                "correctly. Caesar or Vigenere are recommended for "
                "encrypting the whole document instead.\n";
        }

        encryptedBuffer = cipher.Encrypt(document.ToPlainString());
        hasEncryptedBuffer = true;
        document.Clear();

        std::cout << "Document encrypted and cleared from memory - Print will show "
            "an empty buffer now. Use 'Save encrypted data to file' to write "
            "the ciphertext to disk, and 'Decrypt loaded data' to get the "
            "plain text back.\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::CommandSaveEncryptedToFile()
{
    if (!hasEncryptedBuffer)
    {
        std::cout << "Nothing encrypted yet - use 'Encrypt document' first\n";
        return;
    }

    std::cout << "Enter output file name: ";
    std::string path = ReadLine();

    try
    {
        WriteFileText(path, encryptedBuffer);
        std::cout << "Encrypted data saved successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::CommandLoadEncryptedFromFile()
{
    std::cout << "Enter encrypted file name to load: ";
    std::string path = ReadLine();

    try
    {
        encryptedBuffer = ReadFileText(path);
        hasEncryptedBuffer = true;
        std::cout << "Encrypted data loaded. Use 'Decrypt loaded data' to turn it back into text.\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::CommandPrintEncrypted()
{
    if (!hasEncryptedBuffer)
    {
        std::cout << "No encrypted data available - encrypt or load some first\n";
        return;
    }

    std::cout << encryptedBuffer << "\n";
}

void TextEditor::CommandDecryptDocument()
{
    if (!hasEncryptedBuffer)
    {
        std::cout << "No encrypted data available - encrypt or load some first\n";
        return;
    }

    std::cout << "Reminder: use the SAME cipher and SAME key you encrypted with "
        "(e.g. shift 3, not -3, for Caesar).\n";

    try
    {
        CipherHandle cipher = PromptForCipher();
        std::string plain = cipher.Decrypt(encryptedBuffer);

        document.LoadFromPlainString(plain);
        std::cout << "Document decrypted and loaded successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::Run()
{
    PrintMenu();

    bool running = true;
    while (running)
    {
        std::cout << "\n> ";
        int choice = ReadInt();

        switch (choice)
        {
        case 1:  CommandAppendText(); break;
        case 2:  CommandStartNewLine(); break;
        case 3:  CommandAddChecklistItem(); break;
        case 4:  CommandAddContact(); break;
        case 5:  CommandToggleChecklist(); break;
        case 6:  CommandDeleteLastChars(); break;
        case 7:  CommandDeleteRow(); break;
        case 8:  CommandSearch(); break;
        case 9:  CommandCut(); break;
        case 10: CommandCopy(); break;
        case 11: CommandPaste(); break;
        case 12: document.Undo(); break;
        case 13: document.Redo(); break;
        case 14: document.Print(); break;
        case 15: CommandSaveToFile(); break;
        case 16: CommandLoadFromFile(); break;
        case 17: CommandEncryptDocument(); break;
        case 18: CommandSaveEncryptedToFile(); break;
        case 19: CommandLoadEncryptedFromFile(); break;
        case 20: CommandDecryptDocument(); break;
        case 21: CommandPrintEncrypted(); break;
        case 0:  running = false; break;
        default: std::cout << "Wrong command\n"; break;
        }
    }
}