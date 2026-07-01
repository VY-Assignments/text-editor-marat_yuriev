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
        " 1. Append text row\n"
        " 2. Start new (blank) line\n"
        " 3. Add checklist item\n"
        " 4. Add contact\n"
        " 5. Toggle checklist item at cursor\n"
        " 6. Insert text at cursor (text rows only)\n"
        " 7. Delete characters at cursor (text rows only)\n"
        " 8. Move cursor (row, col)\n"
        " 9. Display cursor position\n"
        "10. Search\n"
        "11. Cut row(s)\n"
        "12. Copy row(s)\n"
        "13. Paste row(s)\n"
        "14. Undo\n"
        "15. Redo\n"
        "16. Print document\n"
        "17. Save document to plain file\n"
        "18. Load document from plain file\n"
        "19. Encrypt current document and save to file\n"
        "20. Load an encrypted file into the document (decrypt)\n"
        "21. Encrypt/decrypt an arbitrary text file\n"
        " 0. Exit\n"
        "> ";
}

void TextEditor::CommandAppendText()
{
    std::cout << "Enter text: ";
    document.AppendTextLine(ReadLine());
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
    document.ToggleChecklistAtCursor();
}

void TextEditor::CommandInsertText()
{
    std::cout << "Enter text to insert at cursor: ";
    std::string text = ReadLine();
    for (char c : text)
        document.InsertCharAtCursor(c);
}

void TextEditor::CommandDeleteChars()
{
    std::cout << "How many characters to delete: ";
    int count = ReadInt();
    document.DeleteAtCursor(count);
}

void TextEditor::CommandMoveCursor()
{
    std::cout << "Enter row and column: ";
    int row = ReadInt();
    int col = ReadInt();
    document.MoveCursor(row, col);
}

void TextEditor::CommandSearch()
{
    std::cout << "Text to search for: ";
    document.SearchText(ReadLine());
}

void TextEditor::CommandCut()
{
    std::cout << "How many rows to cut: ";
    document.CutRows(ReadInt());
}

void TextEditor::CommandCopy()
{
    std::cout << "How many rows to copy: ";
    document.CopyRows(ReadInt());
}

void TextEditor::CommandPaste()
{
    document.PasteRows();
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
    std::cout << "Choose algorithm (1 = Caesar, 2 = Vigenere, 3 = Playfair): ";
    int choice = ReadInt();

    std::cout << "Path to cipher library [" << defaultLibraryPath << "]: ";
    std::string path = ReadLine();
    if (path.empty())
        path = defaultLibraryPath;

    if (choice == 1)
    {
        std::cout << "Enter shift key (integer): ";
        int shiftKey = ReadInt();
        return CipherHandle::ForCaesar(path, shiftKey);
    }

    std::cout << "Enter keyword (letters only): ";
    std::string keyword = ReadLine();

    if (choice == 3)
        return CipherHandle::ForPlayfair(path, keyword);

    return CipherHandle::ForVigenere(path, keyword);
}

void TextEditor::CommandEncryptCurrentTextToFile()
{
    try
    {
        CipherHandle cipher = PromptForCipher();

        if (cipher.GetType() == CipherHandle::Type::Playfair)
        {
            std::cout << "Warning: Playfair strips punctuation/digits and "
                "uppercases everything, which will corrupt this "
                "document's row structure (TEXT|... / CHECK|... / "
                "CONTACT|...) and make it impossible to load back "
                "correctly. Caesar or Vigenere are recommended for "
                "encrypting the whole document instead.\n";
        }

        std::string encrypted = cipher.Encrypt(document.ToPlainString());

        std::cout << "Enter output file name: ";
        std::string path = ReadLine();
        WriteFileText(path, encrypted);

        std::cout << "Document encrypted and saved successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::CommandLoadAndDecryptFile()
{
    try
    {
        std::cout << "Enter encrypted file name to load: ";
        std::string path = ReadLine();
        std::string encrypted = ReadFileText(path);

        CipherHandle cipher = PromptForCipher();
        std::string plain = cipher.Decrypt(encrypted);

        document.LoadFromPlainString(plain);
        std::cout << "Document decrypted and loaded successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::CommandEncryptOrDecryptArbitraryFile()
{
    try
    {
        std::cout << "1. Encrypt  2. Decrypt\nChoice: ";
        int mode = ReadInt();

        std::cout << "Input file path: ";
        std::string inputPath = ReadLine();
        std::cout << "Output file path: ";
        std::string outputPath = ReadLine();

        CipherHandle cipher = PromptForCipher();
        std::string input = ReadFileText(inputPath);
        std::string output = (mode == 1) ? cipher.Encrypt(input) : cipher.Decrypt(input);

        WriteFileText(outputPath, output);
        std::cout << (mode == 1 ? "File encrypted successfully\n" : "File decrypted successfully\n");
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}

void TextEditor::Run()
{
    bool running = true;
    while (running)
    {
        PrintMenu();
        int choice = ReadInt();

        switch (choice)
        {
        case 1:  CommandAppendText(); break;
        case 2:  CommandStartNewLine(); break;
        case 3:  CommandAddChecklistItem(); break;
        case 4:  CommandAddContact(); break;
        case 5:  CommandToggleChecklist(); break;
        case 6:  CommandInsertText(); break;
        case 7:  CommandDeleteChars(); break;
        case 8:  CommandMoveCursor(); break;
        case 9:  document.DisplayCursor(); break;
        case 10: CommandSearch(); break;
        case 11: CommandCut(); break;
        case 12: CommandCopy(); break;
        case 13: CommandPaste(); break;
        case 14: document.Undo(); break;
        case 15: document.Redo(); break;
        case 16: document.Print(); break;
        case 17: CommandSaveToFile(); break;
        case 18: CommandLoadFromFile(); break;
        case 19: CommandEncryptCurrentTextToFile(); break;
        case 20: CommandLoadAndDecryptFile(); break;
        case 21: CommandEncryptOrDecryptArbitraryFile(); break;
        case 0:  running = false; break;
        default: std::cout << "Wrong command\n"; break;
        }
    }
}
