#include "raylib.h"
#include <iostream>
#include <map>
#include <vector>

void DrawShip(int length, int startX, int startY) {
    for (int i = 0; i < length; i++) {
        DrawRectangleLines(startX + 20 * i, startY, 20, 20, BLACK);
    }
}

int main() {
    InitWindow(1280, 720, "Raylib");

    enum class Statuses { clear, preset, block, fill }; 

    std::vector<int> SizeShips{1,1,1,1,2,2,2,3,3,4};
    int place_ships = 4;
    bool reverse = false;

    std::map<std::pair<int, int>, Statuses> Field;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            Field[{j, i}] = Statuses::clear;
        }
    }

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        for (auto& [coord, status] : Field) {
            auto [x, y] = coord;
            place_ships = SizeShips.back();
            Rectangle cellRect = {20.0f * x, 20.0f * y, 19.0f, 19.0f};

            if (CheckCollisionPointRec(mousePos, cellRect) && status == Statuses::clear) {
                
                if (reverse) {
                    if (y + place_ships > 10) break; 
                    if (Field[{x, y + place_ships - 1}] != Statuses::clear) break;
                } else {
                    if (x + place_ships > 10) break;
                    if (Field[{x + place_ships - 1, y}] != Statuses::clear) break;
                }
                
                

                for (int i = 0; i < place_ships; i++) {
                    int nx = x + (reverse ? 0 : i);
                    int ny = y + (reverse ? i : 0);

        
                    if (Field.find({nx, ny}) != Field.end() && Field[{nx, ny}] == Statuses::clear) {
                        Field[{nx, ny}] = Statuses::preset;
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool down = false;

        for (auto& [coord, status] : Field) {
            auto [x, y] = coord;

            if (status == Statuses::preset) {
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;

                        auto neighbor = std::make_pair(x + dx, y + dy);

                        if (Field.find(neighbor) != Field.end() && Field[neighbor] == Statuses::clear) {
                            Field[neighbor] = Statuses::block;
                        }
                    }
                }

                status = Statuses::fill;
                down = true;
            }
        }

        if (down) SizeShips.pop_back();
        
    }

        
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            reverse = !reverse;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (auto& [coord, status] : Field) {
            auto [x, y] = coord;
            Rectangle cellRect = {20.0f * x, 20.0f * y, 19.0f, 19.0f};

            Color color = GRAY;
            if (status == Statuses::fill) color = RED;
            if (status == Statuses::preset) color = DARKGRAY;
            if (status == Statuses::block) color = PINK;

            DrawRectangleRec(cellRect, color);
            DrawRectangleLinesEx(cellRect, 1, BLACK);
            
            if(status == Statuses::preset)
                status = Statuses::clear;
        }

        DrawShip(4, 300, 100);
        DrawShip(3, 300, 150);
        DrawShip(2, 300, 200);
        DrawShip(1, 300, 250);

        if (SizeShips.empty())
        {

            DrawRectangleRec({100, 250, 50, 25}, BLUE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}