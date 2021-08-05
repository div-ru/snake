#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

#define STEP 20
#define FRAMES 6
#define WINDOWWIDTH 500
#define WINDOWHEIGHT 500

typedef struct Snake Snake;
typedef struct Game Game;
typedef struct Eat Eat;

struct Snake
{
    int posx, posy;
    Snake *next, *prev;
};

struct Eat
{
    uint16_t posx, posy;
};

struct Game
{
    Snake *SnakeHead;
    Snake *SnakeTail;
    Eat *Eat;
    uint16_t score;
    int StateHor, StateVer;
};

enum {
    NONE,
    HEAD,
    BODY,
    SELF,
};

static void SetEatPos(Game *cgame);
static uint8_t Collision(Game *cgame);
static void AddNewPartToSnake(Game *cgame);
static void FreeRes(Game *cgame);
static void DestroyEat(Game *cgame);
static void DestroySnake(Game *cgame);
static void Engine(Game *cgame);
static void KeyPress(Game *cgame);
static void InitRes(Game *cgame);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    srand(time(0));

    int framesCounter = 0;

    Game *cgame = (Game *) malloc(sizeof( Game ));
    InitRes(cgame);

    SetTraceLogLevel(LOG_WARNING);
    
    InitWindow(WINDOWWIDTH, WINDOWHEIGHT, "SNAKE");

    Texture2D background = LoadTexture("resources/background.png");
    Texture2D head = LoadTexture("resources/head.png");
    Texture2D eat = LoadTexture("resources/eat.png");
    Texture2D part = LoadTexture("resources/part.png");

    //---------------------------------------------------------------------------------------
	SetTargetFPS(30);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update your variables here
        //----------------------------------------------------------------------------------

        framesCounter++;

        KeyPress(cgame);

        if (framesCounter >= FRAMES) {
            framesCounter = 0;
            Engine(cgame);
        }
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            DrawTexture(background, 0, 0, WHITE);

            DrawTexture(eat, cgame->Eat->posx, cgame->Eat->posy, WHITE);

            DrawTexture(head, cgame->SnakeHead->posx, cgame->SnakeHead->posy, WHITE);

            Snake *CurSnake = cgame->SnakeTail;
            while (CurSnake != cgame->SnakeHead){
                    DrawTexture(part, CurSnake->posx, CurSnake->posy, WHITE);
                    CurSnake = CurSnake->prev;
            }

            DrawText(TextFormat("SCORE: %i", cgame->score), 10, 0, 22, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(head);
    UnloadTexture(background);
    UnloadTexture(eat);
    UnloadTexture(part);
    
    FreeRes(cgame);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

static void InitRes (Game *cgame) {
    cgame->score = 0;
    cgame->StateHor = 0;
    cgame->StateVer = 0;

    cgame->Eat = (Eat *) malloc(sizeof(Eat));

    Snake *hSnake = (Snake *) malloc(sizeof(Snake));
    hSnake->posx = 240;
    hSnake->posy = 240;
    hSnake->prev = NULL;
    cgame->SnakeHead = cgame->SnakeTail = hSnake;

    AddNewPartToSnake(cgame);
    AddNewPartToSnake(cgame);
    AddNewPartToSnake(cgame);

    SetEatPos(cgame);
}

static void KeyPress(Game *cgame)
{
    if (IsKeyPressed(KEY_RIGHT)) {
        if( !cgame->StateHor ) {
            cgame->StateHor = STEP;
            cgame->StateVer = 0;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if( !cgame->StateHor ) {
            cgame->StateHor = -STEP;
            cgame->StateVer = 0;
        }
    }

    if (IsKeyPressed(KEY_UP)) {
        if( !cgame->StateVer ) {
            cgame->StateVer = -STEP;
            cgame->StateHor = 0;
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        if( !cgame->StateVer ) {
            cgame->StateVer = STEP;
            cgame->StateHor = 0;
        }
    }
}

static void Engine(Game *cgame)
{
    Snake *CurSnake = cgame->SnakeTail;
    while (CurSnake != cgame->SnakeHead) {
        CurSnake->posx = CurSnake->prev->posx;
        CurSnake->posy = CurSnake->prev->posy;
        CurSnake = CurSnake->prev;
    }

    CurSnake->posx += cgame->StateHor;
    CurSnake->posy += cgame->StateVer;

    if (CurSnake->posy > 480) {
        CurSnake->posy = 20;
    }
    if (CurSnake->posy < 20) {
        CurSnake->posy = 480;
    }
    if (CurSnake->posx > 480) {
        CurSnake->posx = 0;
    }
    if (CurSnake->posx < 0) {
        CurSnake->posx = 480;
    }

    uint8_t tmp = Collision(cgame);
    if (tmp == HEAD) {
        SetEatPos(cgame);
        cgame->score ++;
        if ((cgame->score % 2) == 0) {
            AddNewPartToSnake(cgame);
        }
    }
    if (tmp == SELF) {
        DestroyEat(cgame);
        DestroySnake(cgame);
        InitRes(cgame);
    }
}

static void AddNewPartToSnake(Game *cgame)
{
    Snake *NewPart = (Snake *) malloc(sizeof(Snake));
    NewPart->prev = cgame->SnakeTail;
    NewPart->posx = NewPart->prev->posx;
    NewPart->posy = NewPart->prev->posy;
    NewPart->next = NULL;
    cgame->SnakeTail->next = NewPart;
    cgame->SnakeTail = NewPart;
}

static void DestroySnake(Game *cgame)
{
    if( cgame->SnakeHead ) {
        Snake *cur, *temp;
        cur = cgame->SnakeHead;
        while( cur ) {
            temp = cur->next;
            free( cur );
            cur = temp;
        }
    }
}

static void DestroyEat(Game *cgame)
{
    if( cgame->Eat ) {
        free( cgame->Eat );
    }
}

static void FreeRes(Game *cgame)
{
    if (cgame) {
        DestroySnake (cgame);
        DestroyEat (cgame);
        free (cgame);
    }
}

static void SetEatPos(Game *cgame)
{
    do {
        uint16_t tmp;

        do {
         tmp = rand() % 25 * STEP;
        } while (tmp == cgame->Eat->posx || tmp > 480 || tmp < 20);
        cgame->Eat->posx = tmp;

        do {
           tmp = rand() % 25 * STEP;
        } while (tmp == cgame->Eat->posy || tmp > 480 || tmp < 20);
        cgame->Eat->posy = tmp;

    } while (NONE != Collision(cgame));
}

static uint8_t Collision(Game *cgame)
{
   if (cgame->SnakeHead->posx == cgame->Eat->posx && cgame->SnakeHead->posy == cgame->Eat->posy) {
       return HEAD;
   }

   Snake *cSnake = cgame->SnakeHead->next;
   while (cSnake) {
        if( cgame->Eat->posx == cSnake->posx && cgame->Eat->posy == cSnake->posy ) {
            return BODY;
        }
       cSnake = cSnake->next;
   }

   cSnake = cgame->SnakeHead;
   uint8_t tmp = 0;
   while (cSnake->next) {
       if (cSnake->posx == cSnake->next->posx && cSnake->posy == cSnake->next->posy) {
           tmp++;
       }
    cSnake = cSnake->next;
   }

   if (!tmp) {
        cSnake = cgame->SnakeHead->next;
        while (cSnake) {
            if(cgame->SnakeHead->posx == cSnake->posx && cgame->SnakeHead->posy == cSnake->posy) {
                return SELF;
            }
         cSnake = cSnake->next;
        }
   }
   return NONE;
}
