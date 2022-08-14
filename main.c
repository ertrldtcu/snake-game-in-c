#include <stdio.h>
#include <stdlib.h>
#include "beautify.h"
#include <time.h>
#include <stdbool.h>

// MAX MIN AND OTHER VALUES
#define MIN_NAME_LENGTH 4
#define MAX_NAME_LENGTH 12
#define MIN_BOARD_SIZE 15
#define MAX_BOARD_SIZE 30
#define MAX_APPLE_DENSITY 10
#define SCORE_ON_MOVE 1
#define SCORE_ON_APPLE 100

// TEXT AND QUESTIONS
#define SCORE_T "Puan: %d"
#define NICK_Q "Oyuncu ismi [%d-%d]: "
#define BOARDWIDTH_Q "Tahta genisligi [%d-%d]: "
#define BOARDHEIGHT_Q "Tahta yuksekligi [%d-%d]: "
#define SLOW_V "1: Yavas [200ms]\n"
#define MID_V "2: Orta [150ms]\n"
#define FAST_V "3: Hizli [100ms]\n"
#define VERYFAST_V "4: Cok Hizli [50ms]\n"
#define SPEED_Q "Oyun hizi: "
#define APPLE_Q "Elma sayisi [1-%d]: "

// COLORS
#define FRAME_C 14
#define APPLE_C 12
#define SNAKE_C 10

char *msgptr;

typedef struct apple
{
    int x, y;
} apple_t;

typedef struct node
{
    int x, y;
    struct node *previous;
    struct node *next;
} node_t;

typedef struct snake
{
    node_t *tongue;
    node_t *head;
    node_t *tail;
} snake_t;

const int scoreboardX = 1, scoreboardY = 1, scoreboardW = MAX_NAME_LENGTH;
int scoreboardH = 18;

char *playerName = "DENEME OYUNCU ISMI";
void getName(); // take player name

const int gameboardX = scoreboardX + scoreboardW + 6, gameboardY = 1;
int gameboardW = 24, gameboardH = 24;
void getBoardSize();                           // width and height
void drawOnGameboard(int x, int y, char *str); // draws anything on gameboard

int speedinMS = 100;
void selectGameSpeed(); // 0 (slow), 1 (medium), 2 (fast), 3 (very fast)

apple_t *apples;
int appleDensity = 4;
void selectAppleDensity(); // number of apples at the same time

snake_t *snake;
int directionX = 0, directionY = 0;
void moveSnake();      // moves snake
node_t *didBiteSelf(); // is snake bite self?

int life = 3;
void drawRemainingLife(); // draws remaining life

int score = 0;
void drawScore(); // draws score

int eatenApples = 0;
int didBiteApple();                     // is snake's head on the apple?
void spawnRandomApple(int index);       // spawns apple to random location
void drawEatenApples();                 // writes number of eaten apples
bool isPositionHaveApple(int x, int y); // is position currently have apple?
bool isPositionEmpty(int x, int y);     // is position empty (no snake node or apple)
bool isPositionFree(int x, int y);      // is position free for spawn apple

char *temp;

int main()
{
    temp = malloc(sizeof(char) * MAX_NAME_LENGTH);
    msgptr = malloc(sizeof(char) * 256);
    system("cls");
    srand(time(NULL));

    getName();
    getBoardSize();
    selectGameSpeed();
    selectAppleDensity();

    free(temp);

    apples = malloc(sizeof(apple_t) * appleDensity);
    snake = malloc(sizeof(snake_t));

    snake->head = malloc(sizeof(node_t));
    snake->head->x = gameboardW / 2;
    snake->head->y = gameboardH / 2;
    snake->head->next = malloc(sizeof(node_t));

    snake->tail = snake->head->next;
    snake->tail->x = gameboardW / 2 - 1;
    snake->tail->y = gameboardH / 2;
    snake->tail->previous = snake->head;
    snake->tail->next = NULL;

    drawFrame(scoreboardX, scoreboardY, scoreboardW, 3, FRAME_C);
    gotoxy(scoreboardX * 2 + (scoreboardW * 2 - strlen(playerName)) / 2 + 2, scoreboardY + 3);
    setcolor(15);
    printf("%c %s %c", 175, playerName, 174);

    drawFrame(scoreboardX, scoreboardY + 6, scoreboardW, scoreboardH - 6, FRAME_C);
    drawRemainingLife();
    drawScore();
    drawEatenApples();

    drawFrame(gameboardX, gameboardY, gameboardW, gameboardH, FRAME_C);
    for (int i = 0; i < appleDensity; i++)
        spawnRandomApple(i);

    clock_t tick = clock();
    while (life)
    {
        clock_t now = clock();
        if (kbhit())
        {
            int ch = getch();
            int oldDX = directionX;
            int oldDY = directionY;
            switch (ch)
            {
            case 75: // left
                directionX = -1;
                directionY = 0;
                break;
            case 72: // up
                directionX = 0;
                directionY = -1;
                break;
            case 77: // right
                directionX = 1;
                directionY = 0;
                break;
            case 80: // down
                directionX = 0;
                directionY = 1;
                break;
            }
            if (directionX + oldDX == 0 && directionY + oldDY == 0)
            {
                directionX = oldDX;
                directionY = oldDY;
            }
            if (directionX != oldDX || directionY != oldDY)
                if (ch == 75 || ch == 72 || ch == 77 || ch == 80)
                {
                    tick = now;
                    moveSnake();
                }
        }
        else if (now > tick + speedinMS && directionX + directionY != 0)
        {
            moveSnake();
            tick = now;
        }
    }
    getchar();
    getchar();

    return 0;
}

void getName()
{
    sprintf(msgptr, NICK_Q, MIN_NAME_LENGTH, MAX_NAME_LENGTH);

    while (1)
    {
        msg(msgptr, 0);
        gets(temp);
        system("cls");
        // scanf("%[^\n]s", temp);
        if (strlen(temp) >= MIN_NAME_LENGTH && strlen(temp) <= MAX_NAME_LENGTH)
        {
            playerName = malloc(sizeof(char) * strlen(temp));
            strcpy(playerName, temp);
            break;
        }
    }
}

void getBoardSize()
{
    sprintf(msgptr, BOARDWIDTH_Q, MIN_BOARD_SIZE, MAX_BOARD_SIZE);
    while (1)
    {
        msg(msgptr, 0);
        scanf("%s", temp);
        gameboardW = atoi(temp);
        system("cls");
        if (gameboardW >= MIN_BOARD_SIZE && gameboardW <= MAX_BOARD_SIZE)
            break;
    }
    sprintf(msgptr, BOARDHEIGHT_Q, MIN_BOARD_SIZE, MAX_BOARD_SIZE);
    while (1)
    {
        msg(msgptr, 0);
        scanf("%s", temp);
        gameboardH = atoi(temp);
        system("cls");
        if (gameboardH >= MIN_BOARD_SIZE && gameboardH <= MAX_BOARD_SIZE)
            break;
    }
    scoreboardH = gameboardH;
}

void selectGameSpeed()
{
    while (1)
    {
        msg(SLOW_V, 2);
        msg(MID_V, 2);
        msg(FAST_V, 2);
        msg(VERYFAST_V, 2);
        msg(SPEED_Q, 0);
        scanf("%s", temp);
        speedinMS = atoi(temp);
        system("cls");
        if (speedinMS >= 1 && speedinMS <= 4)
        {
            speedinMS = 250 - speedinMS * 50;
            break;
        }
    }
}

void selectAppleDensity()
{
    sprintf(msgptr, APPLE_Q, MAX_APPLE_DENSITY);
    while (1)
    {
        msg(msgptr, 0);
        scanf("%s", temp);
        appleDensity = atoi(temp);
        system("cls");
        if (appleDensity > 0 && appleDensity <= MAX_APPLE_DENSITY)
            break;
    }
}

void drawOnGameboard(int x, int y, char *str)
{
    gotoxy((gameboardX + x + 1) * 2, gameboardY + y + 1);
    printf("%s", str);
}

void moveSnake()
{

    setcolor(SNAKE_C);

    sprintf(msgptr, "%c%c", 178, 178);
    drawOnGameboard(snake->head->x, snake->head->y, msgptr);

    if (isPositionHaveApple(snake->tail->x, snake->tail->y + 1))
    {
        setcolor(10);
        sprintf(msgptr, " %c", 218);
        drawOnGameboard(snake->tail->x, snake->tail->y, msgptr);
    }
    else
        drawOnGameboard(snake->tail->x, snake->tail->y, "  ");

    int tailX = snake->tail->x, tailY = snake->tail->y;

    node_t *temp = snake->tail;

    snake->tail = snake->tail->previous;
    snake->tail->next = NULL;

    temp->x = snake->head->x + directionX;
    temp->y = snake->head->y + directionY;
    temp->next = snake->head;
    snake->head->previous = temp;

    snake->head = temp;
    if (snake->head->x > gameboardW)
        snake->head->x = 1;
    if (snake->head->x < 1)
        snake->head->x = gameboardW;
    if (snake->head->y > gameboardH)
        snake->head->y = 1;
    if (snake->head->y < 1)
        snake->head->y = gameboardW;

    sprintf(msgptr, "%c%c", 219, 219);
    drawOnGameboard(snake->head->x, snake->head->y, msgptr);

    score += SCORE_ON_MOVE;

    int i = didBiteApple();
    if (i > -1)
    {
        node_t *new = malloc(sizeof(node_t));
        new->x = tailX;
        new->y = tailY;
        new->previous = snake->tail;
        new->next = NULL;
        snake->tail->next = new;
        snake->tail = new;
        score += SCORE_ON_APPLE;
        if (isPositionEmpty(apples[i].x, apples[i].y - 1))
            drawOnGameboard(apples[i].x, apples[i].y - 1, "  ");
        spawnRandomApple(i);
        eatenApples++;
        drawEatenApples();
    }

    node_t *bittenNode = didBiteSelf();
    if (bittenNode != NULL)
    {
        node_t *temp = snake->tail;
        while (temp != bittenNode->previous)
        {
            drawOnGameboard(temp->x, temp->y, "  ");
            if (temp->next != NULL)
                free(temp->next);
            temp = temp->previous;
        }
        snake->tail = temp;
        free(temp->next);
        temp->next = NULL;
        life--;
        drawRemainingLife();
        setcolor(4);
        sprintf(msgptr, "%c%c", 219, 219);
        drawOnGameboard(snake->head->x, snake->head->y, msgptr);
        Sleep(1000);
    }

    sprintf(msgptr, "%c%c", 177, 177);
    drawOnGameboard(snake->tail->x, snake->tail->y, msgptr);

    drawScore();
}

node_t *didBiteSelf()
{
    node_t *temp = snake->head->next;
    while (temp != NULL)
    {
        if (temp->x == snake->head->x && temp->y == snake->head->y)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void drawRemainingLife()
{
    sprintf(msgptr, "%2d x %c ", life, 3);
    gotoxy(scoreboardX * 2 + (scoreboardW * 2 - strlen(msgptr)) / 2 + 4, scoreboardY + 9);
    setcolor(2);
    printf("%s", msgptr);
}

void drawScore()
{
    sprintf(msgptr, SCORE_T, score);
    gotoxy(scoreboardX * 2 + (scoreboardW * 2 - strlen(msgptr)) / 2 + 4, scoreboardY + 12);
    setcolor(9);
    printf("%s", msgptr);
}

void drawEatenApples()
{
    sprintf(msgptr, "%3d x %c%c ", eatenApples, 178, 178);
    gotoxy(scoreboardX * 2 + (scoreboardW * 2 - strlen(msgptr)) / 2 + 4, scoreboardY + 15);
    setcolor(8);
    printf("%3d x ", eatenApples);
    setcolor(12);
    printf("%c%c", 177, 177);
    setcolor(10);
    gotoxy(scoreboardX * 2 + (scoreboardW * 2 - strlen(msgptr)) / 2 + 4, scoreboardY + 14);
    for (int i = 0; i < strlen(msgptr) - 2; i++)
        printf(" ");
    printf("%c", 218);
}

int didBiteApple()
{
    for (int i = 0; i < appleDensity; i++)
    {
        if (snake->head->x == apples[i].x && snake->head->y == apples[i].y)
            return i;
    }
    return -1;
}

bool isPositionHaveApple(int x, int y)
{
    for (int i = 0; i < appleDensity; i++)
    {
        if (x == apples[i].x && y == apples[i].y)
            return true;
    }
    return false;
}

bool isPositionEmpty(int x, int y)
{
    if (isPositionHaveApple(x, y))
        return false;

    node_t *temp = snake->head;
    while (temp != NULL)
    {
        if (x == temp->x && y == temp->y)
            return false;
        temp = temp->next;
    }

    return true;
}

bool isPositionFree(int x, int y)
{
    if (!isPositionEmpty(x, y))
        return false;

    if (x == 1 || x == gameboardW || y == 1 || y == gameboardH)
        return false;

    return true;
}

void spawnRandomApple(int index)
{
    int x, y;
    do
    {
        x = rand() % gameboardW + 1;
        y = rand() % gameboardH + 1;
    } while (!isPositionFree(x, y));

    apples[index].x = x;
    apples[index].y = y;

    setcolor(APPLE_C);
    sprintf(msgptr, "%c%c", 177, 177);
    drawOnGameboard(apples[index].x, apples[index].y, msgptr);

    setcolor(10);
    sprintf(msgptr, " %c", 218);
    drawOnGameboard(apples[index].x, apples[index].y - 1, msgptr);
}