#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "Color.h"
#include "Cursor.h"

#undef main

int WIDTH = 800;
int HEIGHT = 800;
const int BORDER_SIZE = 0;
const int SPACING = 0;
int ROWS = 100;
int COLS = 100;

const Color COLORS[2] = {
    {255, 255, 255, 255},   // Dead
    {0, 0, 0, 255}          // Alive
};

Cursor cursor = {
    0, 0, 1, {255, 200, 0, 150}
};

SDL_Renderer* init(SDL_Renderer* renderer);
void updateGrid(int **grid, int **newGrid, int rows, int cols);
void drawGrid(SDL_Renderer* renderer, int **grid, int rows, int cols);
void drawCursor(SDL_Renderer* renderer, int **grid, int rows, int cols);
void randomizeGrid(int **grid, int rows, int cols);
void killGrid(int **grid, int rows, int cols);
void render(SDL_Renderer* renderer, int **grid, int rows, int cols);

SDL_Renderer* init(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, COLORS[0].r, COLORS[0].g, COLORS[0].b, COLORS[0].a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return renderer;
}

void updateGrid(int **grid, int **newGrid, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int liveNeighbors = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;
                    int r = row + i;
                    int c = col + j;
                    if (r >= 0 && r < rows && c >= 0 && c < cols) {
                        liveNeighbors += grid[r][c];
                    }
                }
            }
            if (grid[row][col] == 1) {
                newGrid[row][col] = (liveNeighbors == 2 || liveNeighbors == 3) ? 1 : 0;
            } else {
                newGrid[row][col] = (liveNeighbors == 3) ? 1 : 0;
            }
        }
    }
}

void drawGrid(SDL_Renderer* renderer, int **grid, int rows, int cols) {
    int cellWidth = (WIDTH - 2 * BORDER_SIZE) / cols;
    int cellHeight = (HEIGHT - 2 * BORDER_SIZE) / rows;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            SDL_SetRenderDrawColor(renderer, COLORS[grid[row][col]].r, COLORS[grid[row][col]].g, COLORS[grid[row][col]].b, COLORS[grid[row][col]].a);
            SDL_Rect cell = {BORDER_SIZE + col * cellWidth, BORDER_SIZE + row * cellHeight, cellWidth - SPACING, cellHeight - SPACING};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

void drawCursor(SDL_Renderer* renderer, int **grid, int rows, int cols) {
    int cursorWidth = (WIDTH - 2 * BORDER_SIZE) / cols;
    int cursorHeight = (HEIGHT - 2 * BORDER_SIZE) / rows;

    SDL_SetRenderDrawColor(renderer, cursor.color.r, cursor.color.g, cursor.color.b, cursor.color.a);
    cursor.cell.x = BORDER_SIZE + cursor.x * cursorWidth;
    cursor.cell.y = BORDER_SIZE + cursor.y * cursorHeight;
    cursor.cell.w = cursorWidth - SPACING;
    cursor.cell.h = cursorHeight - SPACING;
    SDL_RenderFillRect(renderer, &cursor.cell);
}

void randomizeGrid(int **grid, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            grid[row][col] = rand() % 2;
        }
    }
}

void sierpinskiTriangle(int **grid, int rows, int cols) {
    for (int row = 5; row < rows - 5; row++) {
        grid[row][cols / 2] = 1;
    }
}

void killGrid(int **grid, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            grid[row][col] = 0;
        }
    }
}

void render(SDL_Renderer* renderer, int **grid, int rows, int cols) {
    // Set background color to white and clear the screen
    SDL_SetRenderDrawColor(renderer, COLORS[0].r, COLORS[0].g, COLORS[0].b, COLORS[0].a);
    SDL_RenderClear(renderer);

    // Draw the grid and the cursor
    drawGrid(renderer, grid, rows, cols);
    drawCursor(renderer, grid, rows, cols);

    // Present the renderer
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    init(renderer);

    int **grid = (int **)malloc(ROWS * sizeof(int *));
    int **newGrid = (int **)malloc(ROWS * sizeof(int *));
    for (int i = 0; i < ROWS; ++i) {
        grid[i] = (int *)malloc(COLS * sizeof(int));
        newGrid[i] = (int *)malloc(COLS * sizeof(int));
    }

    randomizeGrid(grid, ROWS, COLS);
    render(renderer, grid, ROWS, COLS);

    SDL_Event event;
    int running = 1;
    int paused = 1;
    int ctrlPressed = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_LCTRL) {
                        ctrlPressed = 1;
                    } else if (event.key.keysym.sym == SDLK_SPACE) {
                        paused = !paused;
                    } else if (event.key.keysym.sym == SDLK_r) {
                        randomizeGrid(grid, ROWS, COLS);
                    } else if (event.key.keysym.sym == SDLK_w) {
                        if (ctrlPressed) {
                            cursor.y = (cursor.y - 5 >= 0) ? cursor.y - 5 : 0;
                        } else {
                            cursor.y = (cursor.y - 1 >= 0) ? cursor.y - 1 : 0;
                        }
                    } else if (event.key.keysym.sym == SDLK_s) {
                        if (ctrlPressed) {
                            cursor.y = (cursor.y + 5 < ROWS) ? cursor.y + 5 : ROWS - 1;
                        } else {
                            cursor.y = (cursor.y + 1 < ROWS) ? cursor.y + 1 : ROWS - 1;
                        }
                    } else if (event.key.keysym.sym == SDLK_a) {
                        if (ctrlPressed) {
                            cursor.x = (cursor.x - 5 >= 0) ? cursor.x - 5 : 0;
                        } else {
                            cursor.x = (cursor.x - 1 >= 0) ? cursor.x - 1 : 0;
                        }
                    } else if (event.key.keysym.sym == SDLK_d) {
                        if (ctrlPressed) {
                            cursor.x = (cursor.x + 5 < COLS) ? cursor.x + 5 : COLS - 1;
                        } else {
                            cursor.x = (cursor.x + 1 < COLS) ? cursor.x + 1 : COLS - 1;
                        }
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        grid[cursor.y][cursor.x] = !grid[cursor.y][cursor.x];
                    } else if (event.key.keysym.sym == SDLK_c) {
                        killGrid(grid, ROWS, COLS);
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        updateGrid(grid, newGrid, ROWS, COLS);
                        int **temp = grid;
                        grid = newGrid;
                        newGrid = temp;
                    } else if (event.key.keysym.sym == SDLK_t) {
                        killGrid(grid, ROWS, COLS);
                        sierpinskiTriangle(grid, ROWS, COLS);
                    }
                    render(renderer, grid, ROWS, COLS);
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LCTRL) {
                        ctrlPressed = 0;
                    }
                    break;
            }
        }

        if (!paused) {
            updateGrid(grid, newGrid, ROWS, COLS);

            int **temp = grid;
            grid = newGrid;
            newGrid = temp;

            render(renderer, grid, ROWS, COLS);

            SDL_Delay(50);
        }
    }

    for (int i = 0; i < ROWS; ++i) {
        free(grid[i]);
        free(newGrid[i]);
    }
    free(grid);
    free(newGrid);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}