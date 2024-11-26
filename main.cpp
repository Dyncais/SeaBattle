#include "raylib.h"
#include <iostream>
#include <map>
#include <vector>

enum class Statuses { clear, preset, block, fill }; 

void DrawShip(int length, int startX, int startY) {
    for (int i = 0; i < length; i++) {
        DrawRectangleLines(startX + 20 * i, startY, 20, 20, BLACK);
    }
}


int main() {
    InitWindow(1280, 720, "Raylib");

    

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

        /*DrawShip(4, 300, 100);
        DrawShip(3, 300, 150);
        DrawShip(2, 300, 200);
        DrawShip(1, 300, 250);*/

        if (SizeShips.empty())
        {
            DrawRectangleRec({100, 250, 50, 25}, BLUE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mousePos, {100, 250, 50, 25}))
                {
                    //BattleField(Field); //через лямбду в другое окно UPDATE: или нет

                    EndDrawing();
                    CloseWindow();
                    break;
                } 
            }
        }

        EndDrawing();
    }

    InitWindow(1280, 720, "Playtime");

    enum class CellState {
    Hidden,     // Неоткрытая клетка
    Miss,       // Промах
    Hit,        // Попадание по кораблю
    Ship        // Клетка с кораблём, видимая только игроку
    };

    bool player_turn{true};

    std::vector<CellState> PlayerField(100, CellState::Hidden); 
    std::vector<CellState> EnemyField(100, CellState::Hidden);  

    int count_live_player = 20;
    int count_live_bot = 20;

    for (int i = 0;i<10;i++)
    {
        for (int j = 0;j<10;j++)
        {
            if (Field[{i,j}] == Statuses::fill)
                PlayerField[10*i+j] = CellState::Ship;
        }
    }

    EnemyField = PlayerField;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                Rectangle cellRect = {20.0f * x, 20.0f * y, 19.0f, 19.0f};

                int cellIndex = 10 * x + y;

                switch (PlayerField[cellIndex]) {
                case CellState::Hidden: 
                    DrawRectangleRec(cellRect, LIGHTGRAY); 
                    break;
                case CellState::Ship: 
                    DrawRectangleRec(cellRect, BLUE); 
                    break;
                case CellState::Miss: 
                    DrawRectangleRec(cellRect, BLACK); 
                    break;
                case CellState::Hit: 
                    DrawRectangleRec(cellRect, RED); 
                    break;
                }
                DrawRectangleLinesEx(cellRect, 1, BLACK);
            }
        }

        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                Rectangle cellRect = {500 + 20.0f * x, 20.0f * y, 19.0f, 19.0f};
                int cellIndex = 10 * x + y;

                switch (EnemyField[cellIndex]) {
                case CellState::Hidden: 
                    DrawRectangleRec(cellRect, LIGHTGRAY); 
                    break;
                case CellState::Miss: 
                    DrawRectangleRec(cellRect, BLACK); 
                    break;
                case CellState::Hit: 
                    DrawRectangleRec(cellRect, RED); 
                    break;
                default:
                    DrawRectangleRec(cellRect, LIGHTGRAY); 
                    break;
            }

                DrawRectangleLinesEx(cellRect, 1, BLACK);

                if (player_turn){
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (CheckCollisionPointRec(mousePos, cellRect)) {
                            if (EnemyField[cellIndex] == CellState::Ship)
                            {
                                EnemyField[cellIndex] = CellState::Hit;
                                count_live_bot--;
                                if(count_live_bot == 0)
                                {
                                    EndDrawing();
                                    CloseWindow();
                                }
                            }
                            if (EnemyField[cellIndex] == CellState::Hidden)
                            {
                                EnemyField[cellIndex] = CellState::Miss;
                                player_turn = false;
                            }
                        }
                    }
                }
                if (!player_turn) {
                    while (true) {
                        int x = GetRandomValue(0, 9);
                        int y = GetRandomValue(0, 9);
                        int cellIndex = 10 * x + y;

                        if (PlayerField[cellIndex] != CellState::Miss) {
                            if (PlayerField[cellIndex] == CellState::Ship) {
                                PlayerField[cellIndex] = CellState::Hit;
                                count_live_player--;
                                if(count_live_player == 0)
                                {
                                    EndDrawing();
                                    CloseWindow();
                                }
                            } 
                            if (PlayerField[cellIndex] == CellState::Hidden)
                                PlayerField[cellIndex] = CellState::Miss;
                                player_turn = true;
                            }
                            break;
                        }
                    }
                
            }
        }

        EndDrawing();
    }


    CloseWindow();

    return 0;
}
