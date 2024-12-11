//basic window
#include "raylib.h" 
#include "raymath.h"
#include <string.h>

#define screenWidth 1100
#define screenHeight 1000
#define gridWidth 25
#define gridHeight 25
#define ALIVE 1
#define DEAD 0

const float scaleX = (float) screenWidth / gridWidth;
const float scaleY = (float) screenHeight / gridHeight;

bool grid[gridWidth][gridHeight] = { false };  // needs to be a bool for game of life, dead or alive
bool nextGrid[gridWidth][gridHeight] = { false }; // buffer grid to update grid at same time
bool gameOver = false;

typedef struct GridView {
    Vector2 offset; //how far we are from (0,0)
    float zoom;
} GridView;

typedef struct Snake {
    Vector2 abovePos;
    Vector2 pos;
    Vector2 speed;
    Vector2 nextSpeed; // simple buffer for move commands
    int length;
    bool state;
} Snake;

Vector2 GridToScreen (Vector2 gridCoords, GridView view) {
    Vector2 screenCoords = { 
        (gridCoords.x * scaleX * view.zoom) + view.offset.x,
        (gridCoords.y * scaleY * view.zoom) + view.offset.y,
     };
    
    return screenCoords;
}

Vector2 ScreenToGrid(Vector2 screenCoords, GridView view) {
    Vector2 gridCoords = { 
        (screenCoords.x - view.offset.x) / (scaleX * view.zoom), 
        (screenCoords.y - view.offset.y) / (scaleY * view.zoom),
    };

    return gridCoords;
}

void clearGrid() {
    memset(grid, 0, sizeof(grid));
}

bool isFruitOnSnake(Vector2 pos, Snake* snake) {
    for (int i = 0; i < snake[0].length; i++) {
        if (Vector2Equals(pos, snake[i].pos)) return true;
    }
    return false;
}

void updateSnake(Snake* snake, Vector2* fruit) {
    Vector2 nextHeadPos = (Vector2) { 
        snake[0].pos.x + snake[0].speed.x, 
        snake[0].pos.y + snake[0].speed.y 
    };

    if (nextHeadPos.x < 0 || nextHeadPos.x >= gridWidth ||
        nextHeadPos.y < 0 || nextHeadPos.y >= gridHeight) {
            gameOver = true;
    } else {
        // eat fruit, grow big, new fruit locay
        if (Vector2Equals(nextHeadPos, *fruit)) {
            snake[snake[0].length] = (Snake){
                .pos = snake[snake[0].length - 1].pos,
                .abovePos = snake[snake[0].length - 1].pos,
                .state = ALIVE,
            };
            snake[0].length += 1;
            do {
                fruit->x = GetRandomValue(0, gridWidth - 1);
                fruit->y = GetRandomValue(0, gridHeight - 1);
            } while (isFruitOnSnake(*fruit, snake));
        }
        
        for (int i = 0; i < snake[0].length; i++) {
            // if crashed into self, game over!
            if (Vector2Equals(nextHeadPos, snake[i].pos)) {
                gameOver = true;
                return;
            } else {
                // clear old grid pos if withing boundary 
                grid[(int) snake[i].pos.x][(int) snake[i].pos.y] = false;

                
                Vector2 currPos = snake[i].pos;

                // make head follow it's next pos, all others follow segment above
                if (i == 0) {
                    snake[i].pos = nextHeadPos;
                } else {
                    snake[i].pos = snake[i-1].abovePos;
                }
                
                snake[i].abovePos = currPos;

                // set new grid position of snake
                grid[(int) snake[i].pos.x][(int) snake[i].pos.y] = true;
            }
        }
    }
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "GRID");
    SetTargetFPS(60);
    
    bool isPaused = false;
    float updateTimer = 0.0f;
    float updateInterval = 0.3f;

    GridView view = {
        .offset = (Vector2) { 
            screenWidth / 2.0f - (gridWidth * scaleX * 0.9f) / 2.0f,
            screenHeight / 2.0f - (gridHeight * scaleY * 0.9f) / 2.0f,
        },
        .zoom = 0.9f,
    };

    
    // start somewhere in the middle
    Vector2 startPos = { GetRandomValue((gridWidth / 6) , (gridWidth / 2)) , 
                         GetRandomValue((gridHeight / 6) , (gridHeight / 2)) };
    Vector2 startSpeed = { 1, 0 };

    // make snake array is big as it could possibly get
    Snake snake[(gridWidth * gridHeight) + 1] = { 0 };
    // start with snake body size 2
    snake[0] = (Snake) {
        .abovePos = startPos,
        .pos = startPos,
        .speed = startSpeed,
        .nextSpeed = startSpeed,
        .length = 2,
        .state = ALIVE,
    };
    snake[1] = (Snake) {
        .abovePos = snake[0].pos,
        .pos = (Vector2) { snake[0].pos.x - 1, snake[0].pos.y },
        .state = ALIVE,
    };

    // give fruit random spot that is not on snake or out of bounds
    Vector2 fruit;
    int fruitX;
    int fruitY;
    do {
        fruit.x = GetRandomValue(0, gridWidth - 1);
        fruit.y = GetRandomValue(0, gridHeight - 1);
    } while (isFruitOnSnake(fruit, snake));
        
    // initialize both starting snake segments in grid
    grid[(int)snake[0].pos.x][(int)snake[0].pos.y] = true;
    grid[(int)snake[1].pos.x][(int)snake[1].pos.y] = true;

    while(!WindowShouldClose()) {

        if (IsKeyPressed(KEY_LEFT) && snake[0].speed.x != 1) {
            snake[0].nextSpeed = (Vector2) { -1 , 0 };
        }
        if (IsKeyPressed(KEY_RIGHT) && snake[0].speed.x != -1) {
            snake[0].nextSpeed = (Vector2) { 1 , 0 };
        }
        if (IsKeyPressed(KEY_UP) && snake[0].speed.y != 1) {
            snake[0].nextSpeed = (Vector2) { 0 , -1 };
        }
        if (IsKeyPressed(KEY_DOWN) && snake[0].speed.y != -1) {
            snake[0].nextSpeed = (Vector2) { 0 , 1 };
        }

        if (snake[0].length > 5) {
            updateInterval = 0.25;
        }
        if (snake[0].length > 10) {
            updateInterval = 0.2;
        }
        if (snake[0].length > 15) {
            updateInterval = 0.15;
        }
        if (snake[0].length > 20) {
            updateInterval = 0.1;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;
        }

        if (!isPaused) {
            updateTimer += GetFrameTime();
            if (updateTimer >= updateInterval) {
                snake[0].speed = snake[0].nextSpeed;
                updateSnake(snake, &fruit);
                updateTimer = 0.0f;
            }
        }

        if(IsKeyPressed(KEY_R)) {
            gameOver = false;
            clearGrid();
            updateInterval = 0.3;
            // reset snake
            snake[0] = (Snake) {
                .abovePos = startPos,
                .pos = startPos,
                .speed = startSpeed,
                .nextSpeed = startSpeed,
                .length = 2,
                .state = ALIVE,
            };
            snake[1] = (Snake) {
                .abovePos = snake[0].pos,
                .pos = (Vector2) { snake[0].pos.x - 1, snake[0].pos.y },
                .state = ALIVE,
            };
            // reset grid positions
            grid[(int)snake[0].pos.x][(int)snake[0].pos.y] = true;
            grid[(int)snake[1].pos.x][(int)snake[1].pos.y] = true;
            // reset fruit
            do {
                fruit.x = GetRandomValue(0, gridWidth - 1);
                fruit.y = GetRandomValue(0, gridHeight - 1);
            } while (isFruitOnSnake(fruit, snake));
        }

        BeginDrawing();
            ClearBackground(BLACK);

            //DrawLineV()
            for(int x = 0; x <= gridWidth; x++) {
                Vector2 start = GridToScreen((Vector2) { x , 0 }, view);
                Vector2 end = GridToScreen((Vector2) { x , gridHeight }, view);

                DrawLineV(start, end, LIGHTGRAY);
            }   

            for(int y = 0; y <= gridHeight; y++) {
                Vector2 start = GridToScreen((Vector2) { 0 , y }, view);
                Vector2 end = GridToScreen((Vector2) { gridWidth , y }, view);

                DrawLineV(start, end, LIGHTGRAY);
            }
            for (int i = 0; i < sizeof(grid) / sizeof(grid[0]); i++) {
                for (int j = 0; j < sizeof(grid[0]) / sizeof(grid[0][0]); j++) {
                    Vector2 recPos = GridToScreen((Vector2) { i,j }, view);
                    Vector2 recSize = { scaleX * view.zoom, scaleY * view.zoom };
                    if (i == (int)fruit.x && j == (int)fruit.y) {
                        DrawRectangleV(recPos, recSize, GREEN);
                    }
                    else if (grid[i][j]) {
                        // fill in cells if alive (true)
                        DrawRectangleV(recPos, recSize, WHITE);
                    }
                }
            }

            if (gameOver) {
                DrawRectangle(0, 0, screenWidth, screenHeight, RED);
                DrawText("womp womp", screenWidth / 2.6, screenHeight / 2.6, 50, BLACK);
                DrawText("press 'r' to retry", screenWidth / 2.7, screenHeight / 2.2, 30, BLACK);
                DrawText(TextFormat("SCORE: %d", snake[0].length - 2), screenWidth / 2.3, screenHeight / 1.3, 25, BLACK);
            }

            DrawText(isPaused ? "PAUSED" : "RUNNING", 10, 10, 20, WHITE);
            DrawText(TextFormat("SCORE: %d", snake[0].length - 2), screenWidth / 1.2, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}