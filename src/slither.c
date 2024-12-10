//basic window
#include "raylib.h" 
#include "raymath.h"
#include <string.h>

#define screenWidth 1100
#define screenHeight 1000
#define gridWidth 15
#define gridHeight 15

const float scaleX = (float) screenWidth / gridWidth;
const float scaleY = (float) screenHeight / gridHeight;

bool grid[gridWidth][gridHeight] = { false };  // needs to be a bool for game of life, dead or alive
bool nextGrid[gridWidth][gridHeight] = { false }; // buffer grid to update grid at same time

typedef struct GridView {
    Vector2 offset; //how far we are from (0,0)
    float zoom;
} GridView;

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


void updateGrid() {
    // clear nextGrid
    //memset(nextGrid, 0, sizeof(nextGrid));


    for (int i = 0; i < gridWidth; i++) {
        for (int j = 0; j < gridHeight; j++) {

            //snake logic goes here
        }
    }
    // update cells in one go
    //memcpy(grid, nextGrid, sizeof(grid));
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "GRID");
    SetTargetFPS(60);

    bool isPaused = false;
    float updateTimer = 0.0f;
    float updateInterval = 0.5f;

    GridView view = {
        .offset = (Vector2) { 
            screenWidth / 2.0f - (gridWidth * scaleX * 0.9f) / 2.0f,
            screenHeight / 2.0f - (gridHeight * scaleY * 0.9f) / 2.0f,
        },
        .zoom = 0.9f,
    };

    while(!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 pos = GetMousePosition();
            Vector2 worldPos = ScreenToGrid(pos, view);

            int x = (int) worldPos.x;
            int y = (int) worldPos.y;
            // making sure mouse pos is within grid boundaries
            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                grid[x][y] = true;
            }
        }

        // center grid on key press C
        if (IsKeyPressed(KEY_C)) {
            view.offset.x = (screenWidth - (gridWidth * scaleX * view.zoom)) / 2.0f;
            view.offset.y = (screenHeight - (gridHeight * scaleY * view.zoom)) / 2.0f;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;
        }

        if (!isPaused) {
            updateTimer += GetFrameTime();
            updateGrid();
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
                    if (grid[i][j]) {
                        // fill in cells if alive (true)
                        Vector2 recPos = GridToScreen((Vector2) { i , j }, view);
                        Vector2 recSize = { scaleX * view.zoom, scaleY * view.zoom };
                        DrawRectangleV(recPos, recSize, WHITE);
                    }
                }
            }

            DrawText(isPaused ? "PAUSED" : "RUNNING", 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;       
}