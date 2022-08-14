#include <conio.h>
#include <windows.h>

void setcolor(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        WORD wColor = (csbi.wAttributes & 0xF0) + (color & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    }
}

void gotoxy(short x, short y)
{
    HANDLE hConsoleOutput;
    COORD Cursor_Pos = {x, y};

    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsoleOutput, Cursor_Pos);
}

void drawFrame(int x, int y, int w, int h, int color)
{

    setcolor(color);

    int i;

    gotoxy(x * 2, y); // top
    for (i = 0; i < w * 2 + 8; i++)
        printf("%c", 219);

    gotoxy(x * 2, y + 1); // top line
    printf("%c%c %c", 219, 219, 218);
    for (i = 0; i < w * 2; i++)
        printf("%c", 196);
    printf("%c %c%c", 191, 219, 219);

    for (i = 0; i < h; i++) // sides
    {
        gotoxy(x * 2, y + 2 + i);
        printf("%c%c %c", 219, 219, 179);
        gotoxy((x + w) * 2 + 4, y + 2 + i);
        printf("%c %c%c", 179, 219, 219);
    }

    gotoxy(x * 2, y + h + 2); // bottom line
    printf("%c%c %c", 219, 219, 192);
    for (i = 0; i < w * 2; i++)
        printf("%c", 196);
    printf("%c %c%c", 217, 219, 219);

    gotoxy(x * 2, y + h + 3); // bottom
    for (i = 0; i < w * 2 + 8; i++)
        printf("%c", 219);
}

void msg(char *text, int type)
{
    if (type == 0) // question
    {
        setcolor(9);
        printf("[?] ");
    }
    else if (type == 1) // error
    {
        setcolor(4);
        printf("[%c] ", 120);
    }
    else if (type == 2) // warning
    {
        setcolor(6);
        printf("[!] ");
    }
    setcolor(15);
    printf("%s", text);
}
