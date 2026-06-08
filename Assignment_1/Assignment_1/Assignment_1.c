#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
// ================================ Linked List ================================

typedef struct Node
{
    char x;
    struct Node* next;
} Node;

typedef struct {
    Node* position;    
    int line;            
    int index;           
} Cursor;

// ================================ Undo/Redo Stack ================================

typedef struct {
    Node* snapshot;
    int snapshot_size;  
} StackState;

typedef struct {
    StackState* states;
    int top;           
    int max_size; 
} Stack;

Node* head = NULL;
Cursor cursor = { NULL, 0, 0 };

Stack undo_stack;
Stack redo_stack;

void FreeList(void);

Node* CreateNode(char value_x)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation error\n");
        return NULL;
    }
    newNode->x = value_x;
    newNode->next = NULL;
    return newNode;
}

void InitializeStacks()
{
    undo_stack.max_size = 5; 
    undo_stack.top = -1;
    undo_stack.states = (StackState*)malloc(sizeof(StackState) * undo_stack.max_size);

    redo_stack.max_size = 5; 
    redo_stack.top = -1;
    redo_stack.states = (StackState*)malloc(sizeof(StackState) * redo_stack.max_size);
}

Node* CopyLinkedList(Node* source)
{
    if (source == NULL)
        return NULL;

    Node* newHead = CreateNode(source->x);
    Node* newCurrent = newHead;
    Node* current = source->next;

    while (current != NULL)
    {
        Node* newNode = CreateNode(current->x);
        newCurrent->next = newNode;
        newCurrent = newNode;
        current = current->next;
    }

    return newHead;
}

void FreeLinkedListCopy(Node* node)
{
    while (node != NULL)
    {
        Node* temp = node;
        node = node->next;
        free(temp);
    }
}

int CountCharacters(Node* node)
{
    int count = 0;
    while (node != NULL)
    {
        count++;
        node = node->next;
    }
    return count;
}

void SaveStateToUndoStack()
{
    if (undo_stack.top >= undo_stack.max_size - 1)
    {
        printf("Undo stack is full. Oldest action will be removed.\n");

        FreeLinkedListCopy(undo_stack.states[0].snapshot);
        for (int i = 0; i < undo_stack.top; i++)
        {
            undo_stack.states[i] = undo_stack.states[i + 1];
        }
        undo_stack.top--;
    }

    undo_stack.top++;
    undo_stack.states[undo_stack.top].snapshot = CopyLinkedList(head);
    undo_stack.states[undo_stack.top].snapshot_size = CountCharacters(head);

    for (int i = 0; i <= redo_stack.top; i++)
    {
        FreeLinkedListCopy(redo_stack.states[i].snapshot);
    }
    redo_stack.top = -1;
}

void FreeStacks()
{
    for (int i = 0; i <= undo_stack.top; i++)
    {
        FreeLinkedListCopy(undo_stack.states[i].snapshot);
    }
    free(undo_stack.states);

    for (int i = 0; i <= redo_stack.top; i++)
    {
        FreeLinkedListCopy(redo_stack.states[i].snapshot);
    }
    free(redo_stack.states);
}

// ================================ Useful Function ================================
void AppendNode(Node* newNode)
{
    if (newNode == NULL)
        return;

    if (head == NULL)
    {
        head = newNode;
        return;
    }

    Node* current = head;

    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = newNode;
}

char* ReadLine()
{
    int size = 200;
    int length = 0;

    char* buffer = malloc(size);

    if (buffer == NULL)
    {
        printf("Memory allocation error\n");
        return NULL;
    }

    int user_input;

    while ((user_input = getchar()) != '\n' && user_input != EOF)
    {
        buffer[length] = user_input;
        length++;

        if (length >= size - 1)
        {
            size *= 2;

            char* temp = realloc(buffer, size);

            if (temp == NULL)
            {
                free(buffer);
                printf("Memory allocation error\n");
                return NULL;
            }

            buffer = temp;
        }
    }


    buffer[length] = '\0';

    return buffer;
}


// ================================ Task 1 ================================

void Append(char user_text[])
{
    int i = 0;

    while (user_text[i] != '\0')
    {
        if (user_text[i] == '\n')
            break;

        Node* newNode = CreateNode(user_text[i]);
        AppendNode(newNode);

        i++;
    }
    SaveStateToUndoStack();
    printf("Text was appended\n");
    return;
}
// ================================ Task 2 ================================

void StartNewLine()
{
    AppendNode(CreateNode('\n'));
    SaveStateToUndoStack();
}

// ================================ Task 3 ================================ 

void SaveInfo(const char filename[])
{
    FILE* file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    Node* current = head;

    while (current != NULL)
    {
        fputc(current->x, file);
        current = current->next;
    }

    fclose(file);

    printf("Text saved successfully\n");
}


// ================================ Task 4 ================================

void LoadFile(const char filename[])
{
    FILE* file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    FreeList();

    int ch;

    while ((ch = fgetc(file)) != EOF)
    {
        AppendNode(CreateNode(ch));
    }

    fclose(file);

    printf("Text loaded successfully\n");
}

// ================================ Task 5 ================================

void Print()
{
    Node* current = head;

    if (head == NULL)
    {
        printf("Buffer is empty\n");
        return;
    }

    while (current != NULL)
    {
        printf("%c", current->x);
        current = current->next;
    }

    printf("\n");
}

// ================================ Task 6 ================================

void InsertLine(char user_text[], int line, int index)
{
    Node* current = head;
    Node* prev = NULL;

    int currentLine = 0;
    int currentIndex = 0;

    while (current != NULL && currentLine < line)
    {
        if (current->x == '\n')
        {
            currentLine++;
        }

        prev = current;
        current = current->next;
    }

    if (currentLine != line)
    {
        printf("Line does not exist\n");
        return;
    }

    while (current != NULL &&
        current->x != '\n' &&
        currentIndex < index)
    {
        prev = current;
        current = current->next;
        currentIndex++;
    }

    if (currentIndex != index)
    {
        printf("Index out of range\n");
        return;
    }

    for (int i = 0; user_text[i] != '\0'; i++)
    {
        Node* newNode = CreateNode(user_text[i]);

        if (prev == NULL)
        {
            newNode->next = head;
            head = newNode;
        }
        else
        {
            newNode->next = current;
            prev->next = newNode;
        }

        prev = newNode;
    }
    SaveStateToUndoStack();
    printf("Text inserted successfully\n");
}

// ================================ Task 7 ================================

void Search(char user_text[])
{

    bool found = false;
    Node* current = head;

    if (head == NULL)
    {
        printf("Buffer is empty\n");
        return;
    }

    int currentLine = 0;
    int currentIndex = 0;

    while (current != NULL)
    {
        Node* scan = current;
        int i = 0;

        while (scan != NULL && user_text[i] != '\0')
        {
            if (scan->x == '\n')
                break;

            if (scan->x != user_text[i])
                break;

            scan = scan->next;
            i++;
        }

        if (user_text[i] == '\0')
        {
            found = true;
            printf("Found at: %d %d\n", currentLine, currentIndex);
        }

        if (current->x == '\n')
        {
            currentLine++;
            currentIndex = 0;
        }
        else
        {
            currentIndex++;
        }


        current = current->next;
    }

    if (!found)
    {
        printf("Text not found\n");
    }
}

// ================================ Task 8 ================================

void Delete(int num_chars)
{
    if (num_chars <= 0)
    {
        printf("Invalid number of characters to delete\n");
        return;
    }

    if (cursor.position == NULL && head != NULL)
    {
        printf("Cursor is at invalid position\n");
        return;
    }

    if (head == NULL)
    {
        printf("Buffer is empty\n");
        return;
    }

    if (cursor.position == NULL)
    {
        cursor.position = head;
    }

    Node* current = cursor.position;
    Node* prev = NULL;

    if (cursor.position != head)
    {
        Node* temp = head;
        while (temp != NULL && temp->next != cursor.position)
        {
            temp = temp->next;
        }
        prev = temp;
    }

    int deleted = 0;
    for (int i = 0; i < num_chars && current != NULL; i++)
    {
        Node* temp = current;
        current = current->next;
        deleted++;

        if (prev == NULL)
        {
            head = current;
        }
        else
        {
            prev->next = current;
        }

        free(temp);
    }

    cursor.position = current;

    SaveStateToUndoStack();
    printf("Deleted %d characters\n", deleted);
}

// ================================ Task 9 ================================

void Undo()
{
    if (undo_stack.top < 0)
    {
        printf("Nothing to undo\n");
        return;
    }

    if (redo_stack.top < redo_stack.max_size - 1)
    {
        redo_stack.top++;
        redo_stack.states[redo_stack.top].snapshot = CopyLinkedList(head);
        redo_stack.states[redo_stack.top].snapshot_size = CountCharacters(head);
    }

    FreeLinkedListCopy(head);
    head = CopyLinkedList(undo_stack.states[undo_stack.top].snapshot);
    cursor.position = head;
    cursor.line = 0;
    cursor.index = 0;

    FreeLinkedListCopy(undo_stack.states[undo_stack.top].snapshot);
    undo_stack.top--;

    printf("Undo executed\n");
}

// ================================ Task 10 ================================
    
void Redo()
{
    if (redo_stack.top < 0)
    {
        printf("Nothing to redo\n");
        return;
    }

    if (undo_stack.top < undo_stack.max_size - 1)
    {
        undo_stack.top++;
        undo_stack.states[undo_stack.top].snapshot = CopyLinkedList(head);
        undo_stack.states[undo_stack.top].snapshot_size = CountCharacters(head);
    }

    FreeLinkedListCopy(head);
    head = CopyLinkedList(redo_stack.states[redo_stack.top].snapshot);
    cursor.position = head;
    cursor.line = 0;
    cursor.index = 0;

    FreeLinkedListCopy(redo_stack.states[redo_stack.top].snapshot);
    redo_stack.top--;

    printf("Redo executed\n");
}

// ================================ Task 11 ================================

void MoveCursorToPosition(int line, int index)
{
    Node* current = head;
    int currentLine = 0;
    int currentIndex = 0;

    while (current != NULL && currentLine < line)
    {
        if (current->x == '\n')
        {
            currentLine++;
        }
        current = current->next;
    }

    if (currentLine != line)
    {
        printf("Line does not exist\n");
        return;
    }

    while (current != NULL && currentIndex < index && current->x != '\n')
    {
        current = current->next;
        currentIndex++;
    }

    if (currentIndex != index)
    {
        printf("Index out of range\n");
        return;
    }

    cursor.position = current;
    cursor.line = line;
    cursor.index = index;
    printf("Cursor moved to Line %d, Index %d\n", line, index);
}

// ================================ Task 12 ================================

void DisplayCursorPosition()
{
    if (cursor.position == NULL)
    {
        printf("Cursor is at the start (or buffer is empty)\n");
    }
    else
    {
        printf("Cursor at Line %d, Index %d, Character: '%c'\n",
            cursor.line, cursor.index, cursor.position->x);
    }
}

// ================================ Task 13 ================================

void FreeList()
{
    Node* current = head;

    while (current != NULL)
    {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    head = NULL;
}


//================================ Main Function ================================

int main(void)
{
    InitializeStacks();
    int user_choice;
    printf(
        "Choose the command(Enter 1 or 2...7): \n"
        "1.Enter text to append\n"
        "2.Start the new line\n"
        "3.Use files to save the information\n"
        "4.Use files to load the information\n"
        "5.Print the current text to console\n"
        "6.Insert the text by line and symsbol index\n"
        "7.Search\n"
        "8.Delete command\n"
        "9.Undo command\n"
        "10.Redo command\n"
        "11.Move cursor to position (line, index)\n"
        "12.Display cursor position\n"
        "13.Clearing the console\n");
    while (true)
    {
        scanf_s("%d", &user_choice);
        while (getchar() != '\n');

        switch (user_choice)
        {
        case 1:
            printf("Write text to append:\n");
            char* user_input = ReadLine();

            if (user_input != NULL)
            {
                Append(user_input);
                free(user_input);
            }
            break;

        case 2:
            StartNewLine();
            printf("A new line was added\n");
            break;

        case 3:
            printf("Enter file name to save:\n");

            char* filename_save = ReadLine();

            if (filename_save != NULL)
            {
                SaveInfo(filename_save);
                free(filename_save);
            }
            break;

        case 4:
            printf("Enter file name to load:\n");

            char* filename_load = ReadLine();

            if (filename_load != NULL)
            {
                LoadFile(filename_load);
                free(filename_load);
            }
            break;

        case 5:
            Print();
            break;

        case 6:
        {
            int line, index;

            printf("Choose line and index (example: 0 5):\n");
            scanf_s("%d %d", &line, &index);
            while (getchar() != '\n');

            printf("Enter text to insert:\n");
            char* user_input = ReadLine();

            if (user_input != NULL)
            {
                InsertLine(user_input, line, index);
                free(user_input);
            }
            break;
        }

        case 7:

            printf("Choose word to search:\n");

            char* search_input = ReadLine();

            if (search_input != NULL)
            {
                Search(search_input);
                free(search_input);
            }

            break;

        case 8:
        {
            int num_chars;
            printf("How many characters to delete:\n");
            scanf_s("%d", &num_chars);
            while (getchar() != '\n');
            Delete(num_chars);
            break;
        }

        case 9:
            Undo();  
            break;

        case 10:
            Redo(); 
            break;

        case 11:
        {
            int line, index;
            printf("Enter line and index:\n");
            scanf_s("%d %d", &line, &index);
            while (getchar() != '\n');
            MoveCursorToPosition(line, index);
            break;
        }

        case 12:
            DisplayCursorPosition();
            break;

        case 13:
            FreeList();
            FreeStacks();
            return 0;

        default:
            printf("Wrong command\n");
            break;
        }
    }       
}