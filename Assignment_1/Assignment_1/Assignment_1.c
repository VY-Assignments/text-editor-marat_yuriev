#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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


//================================ Main Function ================================

int main(void)
{
    char user_input[100];
    int user_choice;

    while(true) 
    {
        printf(
            "Choose the command(Enter 1 or 2...7): \n"
            "1.Enter text to append\n"
            "2.Start the new line\n"
            "3.Use files to load/save the information\n"
            "4.Print the current text to console\n"
            "5.Insert the text by line and symbol index\n"
            "6.Search\n"
            "7.Clearing the console\n");

        scanf_s("%d", &user_choice);
        while (getchar() != '\n');

        switch (user_choice)
        {   

        case(1):
            printf("Write text which you want to append\n");
            fgets(user_input, sizeof(user_input), stdin);
            Append(user_input);
            break;

        case(2):
            StartNewLine();
            break;

        default:
            printf("Wrong Command\n");
            break;
        }

    }
    
}