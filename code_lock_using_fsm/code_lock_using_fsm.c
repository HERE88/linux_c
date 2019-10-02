#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    STATE0 = 0,
    STATE1,
    STATE2,
    STATE3,
    STATE4
}STATE;

int main()
{
    char ch;
    STATE current_state = STATE0;
    int cnt = 0;

    while(1)
    {
        printf("In put password:");
        while((ch = getchar()) != '\n')
        {
            if((ch < '0') || (ch > '9'))
            {
                printf("Input num error./n");
                break;
            }
            switch(current_state)
            {
                case STATE0:
                    if(ch == '1') current_state = STATE1;
                    break;
                case STATE1:
                    if(ch == '9') current_state = STATE2;
                    break;
                case STATE2:
                    if(ch == '9') current_state = STATE3;
                    break;
                case STATE3:
                    if(ch == '0') current_state = STATE4;
                    break;
                default:
                    current_state = STATE0;
                    break;
            }
        }

        if(current_state == STATE4)
        {
            printf("Correct, lock is open!\n");
            current_state = STATE0;
        }
        else
        {
            printf("Wrong, locked!\n");
            current_state = STATE0;
        }
        break;
    }

    return 0;
}