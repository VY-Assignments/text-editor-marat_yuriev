#include <stdio.h>

int main(void)
{
    char text[] = "";
    int user_input;
    printf(
        "Choose the command: \n"
        "1.Enter text to append\n"
        "2.Start the new line\n"
        "3.Use files to load/save the information\n"
        "4.Print the current text to console\n"
        "5.Insert the text by line and symbol index\n"
        "6.Search\n"
        "7.Clearing the console\n");

   scanf("%d", &user_input);

   printf("You entered: %d\n", user_input);

    return 0;
}