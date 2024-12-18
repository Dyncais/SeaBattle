#include "raylib.h"
#include <iostream>
#include <map>
#include <vector>
#include <enet/enet.h>
#include "BattleTime.h"
#include <string.h>

enum class Statuses { clear, preset, block, fill }; 

enum class CellState {
    Hidden,     // Неоткрытая клетка
    Miss,       // Промах
    Hit,        // Попадание по кораблю
    Ship        // Клетка с кораблём, видимая только игроку
    };
    

enum class SceneState{
    ChoosingMode,
    Preparation,
    HostLobby,
    JoinLobby,
    Playing
};

struct GameFields
{
    std::vector<CellState> PlayerField;
    std::vector<CellState> EnemyField; 

    int count_live_player;
    int count_live_bot;

    GameFields()
        : PlayerField(100, CellState::Hidden), 
          EnemyField(100, CellState::Hidden),
          count_live_player(20),             
          count_live_bot(20)
    {
    }
};

void FillGameField(std::map<std::pair<int, int>, Statuses>& Field, GameFields& Game)
{
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
            {
                if (Field[{i,j}] == Statuses::fill)
                {
                    Game.PlayerField[10*i+j] = CellState::Ship;
                }
            }   
    }
    Game.EnemyField = Game.PlayerField;
    return;
}

void AI_vs_Player(bool& turn, Vector2& mousePos, Rectangle& cellRect, int cellIndex, GameFields& Game)
{
    if (turn){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, cellRect)) {
                if (Game.EnemyField[cellIndex] == CellState::Ship)
                    {
                        Game.EnemyField[cellIndex] = CellState::Hit;
                        Game.count_live_bot--;
                        if(Game.count_live_bot == 0)
                            {
                                EndDrawing();
                                CloseWindow();
                                }
                            }
                        if (Game.EnemyField[cellIndex] == CellState::Hidden)
                            {
                                Game.EnemyField[cellIndex] = CellState::Miss;
                                turn = false;
                            }
                    }
                }
            }
    else {
        while (true) {
            int x = GetRandomValue(0, 9);
            int y = GetRandomValue(0, 9);
            int cellIndex = 10 * x + y;

            if (Game.PlayerField[cellIndex] != CellState::Miss) {
                if (Game.PlayerField[cellIndex] == CellState::Ship) {
                    Game.PlayerField[cellIndex] = CellState::Hit;
                    Game.count_live_player--;
                    if(Game.count_live_player == 0)
                        {
                            EndDrawing();
                            CloseWindow();
                        }
                                    } 
                    if (Game.PlayerField[cellIndex] == CellState::Hidden)
                        Game.PlayerField[cellIndex] = CellState::Miss;
                        turn = true;
            }
            break;
            }
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

    SceneState current_state = SceneState::ChoosingMode;

    bool player_turn{true};

    GameFields Game;


     if (enet_initialize() != 0) {
        std::cerr << "Не удалось инициализировать ENet!" << std::endl;
        return EXIT_FAILURE;
    }

    ENetHost* server = nullptr;
    ENetHost* client = nullptr;
    ENetPeer* peer = nullptr;
    GameFields fields;


    while (!WindowShouldClose()) {

        switch(current_state)
        {

            case SceneState::ChoosingMode: {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Выберите режим:", 50, 50, 20, BLACK);
            DrawText("1. Создать лобби (нажмите Enter)", 50, 100, 20, BLUE);
            DrawText("2. Подключиться к лобби (нажмите Space)", 50, 150, 20, GREEN);

            if (IsKeyPressed(KEY_ENTER)) {
                ENetAddress address = {};
                address.host = ENET_HOST_ANY;
                address.port = 1234;          

                server = enet_host_create(&address, 32, 2, 0, 0);
                if (!server) {
                    std::cerr << "Ошибка: сервер не создан!" << std::endl;
                    break;
                }
                std::cout << "Сервер создан на порту " << address.port << std::endl;
                current_state = SceneState::HostLobby;

            } else if (IsKeyPressed(KEY_SPACE)) {
                
                client = enet_host_create(nullptr, 1, 2, 0, 0);

                if (!client) {
                    std::cerr << "Ошибка: клиент не создан!" << std::endl;
                    break;
                }

                ENetAddress address = {};
                enet_address_set_host(&address, "localhost");
	            address.port = 1234;
                
                peer = enet_host_connect(client, &address, 2, 0);
                if (!peer) {
                    std::cerr << "Ошибка: подключение к серверу не удалось!" << std::endl;
                    break;
                }
                std::cout << "Клиент: попытка подключения к серверу." << std::endl;
                current_state = SceneState::JoinLobby;
            }
                
            

            EndDrawing();
            continue;
        }
            
            
            case SceneState::HostLobby: {
                BeginDrawing();
                ClearBackground(RAYWHITE);

                DrawText("Ожидание подключения...", 50, 50, 20, BLACK);
                

                ENetEvent event = {};
                if (enet_host_service(server, &event, 10) > 0) {
                    if (event.type == ENET_EVENT_TYPE_CONNECT) {
                        std::cout << "Сервер: клиент подключился." << std::endl;
                        peer = event.peer;
                        current_state = SceneState::Preparation;
                    }
                }
                EndDrawing();
                continue;
            }

             case SceneState::JoinLobby: {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Подключение к серверу...", 50, 50, 20, BLACK);

            ENetEvent event = {};
            if (enet_host_service(client, &event, 5000) > 0) { 
                if (event.type == ENET_EVENT_TYPE_CONNECT) {      
                    puts ("Connection to some.server.net:1234 succeeded.");   
                    //sending a package so the server knows that you connected
                    ENetPacket * packet = enet_packet_create ("packet", 
                                          strlen ("packet") + 1, 
                                          ENET_PACKET_FLAG_RELIABLE);     
                    enet_peer_send (peer, 0, packet);  
                    enet_host_flush (client);   
                    current_state = SceneState::Preparation;
                }
            }

            EndDrawing();
            continue;
        }

            case SceneState::Preparation:
            {
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

                if (SizeShips.empty())
                {
                    DrawRectangleRec({100, 250, 50, 25}, BLUE);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (CheckCollisionPointRec(mousePos, {100, 250, 50, 25}))
                        {
                            FillGameField(Field,Game);
                            current_state = SceneState::Playing;
                            EndDrawing();
                            continue;
                        } 
                    }
                }

                EndDrawing();
                continue;
            }
            
            case SceneState::Playing:
            {
                Vector2 mousePos = GetMousePosition();

                BeginDrawing();
                ClearBackground(RAYWHITE);

                for (int x = 0; x < 10; x++) {
                    for (int y = 0; y < 10; y++) {
                        Rectangle cellRect = {20.0f * x, 20.0f * y, 19.0f, 19.0f};
                        
                        int cellIndex = 10 * x + y;

                        switch (Game.PlayerField[cellIndex]) {
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


                        cellRect = {500 + 20.0f * x, 20.0f * y, 19.0f, 19.0f};

                        switch (Game.EnemyField[cellIndex]) {
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

                        AI_vs_Player(player_turn, mousePos, cellRect, cellIndex, Game);
                    }

                }

            EndDrawing();
            continue;
            }
        
            
        }
    if (server) enet_host_destroy(server);
    if (client) enet_host_destroy(client);
    enet_deinitialize();
    return 0;
} 
}