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

Node* head = NULL;

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

char* ReadLine()
{
    int size = 20;
    int length = 0;

    char* buffer = malloc(size);

    if (buffer == NULL)
    {
        printf("Memory allocation error\n");
        return NULL;
    }

    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        buffer[length] = ch;
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

    printf("Input length: %d\n", length);

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
}
// ================================ Task 2 ================================

void StartNewLine()
{
    AppendNode(CreateNode('\n'));
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
            currentLine++;

        prev = current;
        current = current->next;
    }

    if (currentIndex < index)
    {
        printf("Error: index out of range\n");
        return;
    }

    while (current != NULL && current->x != '\n' && currentIndex < index)
    {
        prev = current;
        current = current->next;
        currentIndex++;
    }

    if (current == NULL && currentIndex < index)
    {
        printf("Error: index out of range\n");
        return;
    }

    for (int i = 0; user_text[i] != '\0'; i++)
    {
        if (user_text[i] == '\n')
            break;

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

//================================ Main Function ================================

int main(void)
{
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
        "8.Clearing the console\n");
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

            char* filename = ReadLine();

            if (filename != NULL)
            {
                SaveInfo(filename);
                free(filename);
            }
            break;

        case 4:
            printf("Enter file name to load:\n");

            char* filename = ReadLine();

            if (filename != NULL)
            {
                LoadFile(filename);
                free(filename);
            }

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
            FreeList();
            return 0;

        default:
            printf("Wrong command\n");
            break;
        }
    }
        
}