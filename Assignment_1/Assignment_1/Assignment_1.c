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

    char ch;

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

    if (current == NULL && currentLine < line)
        return;

    while (current != NULL && current->x != '\n' && currentIndex < index)
    {
        prev = current;
        current = current->next;
        currentIndex++;
    }

    for (int i = 0; user_text[i] != '\0'; i++)
    {
        if (user_text[i] == '\n')
            continue;

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
    Node* current = head;

    int currentLine = 0;
    int currentIndex = 0;

    while (current != NULL)
    {
        Node* scan = current;
        int i = 0;

        while (scan != NULL && user_text[i] != '\0')
        {
            if (scan->x != user_text[i])
                break;

            scan = scan->next;
            i++;
        }

        if (user_text[i] == '\0')
        {
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
}

//================================ Main Function ================================

int main(void)
{
    char user_input[1000];
    char search_input[100];
    char filename[100];
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
            fgets(user_input, sizeof(user_input), stdin);
            Append(user_input);
            break;

        case 2:
            StartNewLine();
            printf("A new line was added\n");
            break;

        case 3:
            printf("Enter file name to save:\n");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\r\n")] = 0;
            SaveInfo(filename);
            break;

        case 4:
            printf("Enter file name to load:\n");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\r\n")] = 0;
            LoadFile(filename);
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
            fgets(user_input, sizeof(user_input), stdin);

            InsertLine(user_input, line, index);
            break;
        }

        case 7:

            printf("Choose word to search:\n");

            fgets(search_input, sizeof(search_input), stdin);

            search_input[strcspn(search_input, "\n")] = '\0';

            Search(search_input);

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