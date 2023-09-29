#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "raylib.h"

void IncrementMenuSelection(int*, int, Sound);
void DecrementMenuSelection(int*, int, Sound);
void ChangeVolume(char**, float*);
float LoadSettings();
void SaveSettings(float);
char* DetermineVolumeText(float);
int LoadHighScore();
void SaveHighScore(int);
void Shutdown();

int main(void)
{
    srand(time(NULL));
    const int screenW = 1600, screenH = 900;
    int GameState = 0, menuSelection = 0, optionSelection = 0; // GAME STATES = 0: Main Menu; 1:Play; 2:Options; 3:Quit; 4:Title Screen;
    float MasterVolume = LoadSettings();
    int HighScore = LoadHighScore(), score = 0;
    bool laser = false;
    
    Image icon = LoadImage("resources/icon.png");
    
    char * mainMenu[3];
    mainMenu[0] = "Play";
    mainMenu[1] = "Options";
    mainMenu[2] = "Quit";
    
    InitWindow(screenW, screenH, "Lasers & Walls!");
    InitAudioDevice();
    
    float floorH = 800, ceilingH = 100;
    Vector2 playerPosition = { (float)screenW / 2, (float)screenH / 2 };
    Rectangle floor = { 0, floorH, screenW, 300 };
    Rectangle ceiling = { 0, 0, screenW, ceilingH};
    Rectangle lWall = { 0, 300, 0, 300};
    Rectangle rWall = { screenW, 300, screenW / 2, 300};
    Rectangle laserPosition = { 0, rand() % 300 + 300, screenW, 1 };
    Rectangle vLaserPosition = {0, ceilingH, 2, floorH - ceilingH };
    int vLaserDirection = 0, wallMovement = 0;
    
    Sound ambient = LoadSound("sfx/ambient.wav");
    Sound menuSound = LoadSound("sfx/menu.wav");
    Sound selectSound = LoadSound("sfx/select.wav");
    Sound ingameSound = LoadSound("sfx/ingame.wav");
    SetSoundVolume(ingameSound, 0.5f);
    Sound gameOverSound = LoadSound("sfx/gameover.wav");
    Sound highscoreSound = LoadSound("sfx/highscore.wav");
    
    SetMasterVolume(MasterVolume);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); 
    SetWindowIcon(icon);
    
    char * options[3];
    options[0] = DetermineVolumeText(MasterVolume);
    options[1] = "Reset High Score";
    options[2] = "Back";
    
    while (!WindowShouldClose())
    {
        if (!IsSoundPlaying(ambient) && IsAudioDeviceReady()) PlaySound(ambient);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Use `W` and `S` to navigate the menu, press `E` to select.", 10, 10, 20, LIGHTGRAY);
            DrawText(TextFormat("High Score: %i", HighScore), screenW - 250, 10, 25, LIGHTGRAY);
            switch(GameState)
            {
                case 0:
                if (IsSoundPlaying(gameOverSound) && IsAudioDeviceReady()) StopSound(gameOverSound);
                DrawText("LASERS", screenW / 2, screenH / 5, 150, MAROON);
                DrawText("&", screenW / 1.1, screenH / 4, 100, BLACK);
                DrawText("WALLS!", screenW / 2, screenH / 3, 150, GRAY);
                int i;
                for (i = 0; i < 3; i++)
                {
                    if (i == menuSelection) DrawText(TextFormat("%s", mainMenu[i]), 100, 100 + i * 50, 50, GRAY);
                    else DrawText(TextFormat("%s", mainMenu[i]), 100, 100 + i * 50, 50, LIGHTGRAY);
                }
                if (IsKeyPressed('S')) IncrementMenuSelection(&menuSelection, 3, menuSound);
                if (IsKeyPressed('W')) DecrementMenuSelection(&menuSelection, 3, menuSound);
                if (IsKeyPressed('E')) 
                {
                    if (IsAudioDeviceReady()) PlaySound(selectSound);
                    switch(menuSelection)
                    {
                        case 0:
                        GameState = 1;
                        break;
                        case 1:
                        GameState = 2;
                        break;
                        case 2:
                        GameState = 3;
                        break;
                    }
                }
                break;
                case 1:
                    if (IsSoundPlaying(ambient) && IsAudioDeviceReady()) StopSound(ambient);
                    if (IsSoundPlaying(gameOverSound) && IsAudioDeviceReady()) StopSound(gameOverSound);
                    if (!IsSoundPlaying(ingameSound) && IsAudioDeviceReady()) PlaySound(ingameSound);
                    score++;
                    if (score % 200 == 0) 
                    {
                        laser = false;
                        laserPosition.y = rand() % 300 + 300;
                    }
                    else if (score % 100 == 0) laser = true;
                    
                    if (ceilingH < 300)
                    {
                        ceilingH += 1;
                        ceiling.height += 1;
                        floorH -= 1;
                        floor.y -= 1;
                        vLaserPosition.y = ceilingH;
                        vLaserPosition.height = floorH - ceilingH;
                    } 
                    else if (lWall.width < 450)
                    {
                        laserPosition.x += 1;
                        laserPosition.width -= 2;
                        lWall.width += 1;
                        rWall.x -= 1;
                    }
                    else 
                    {
                        if (wallMovement == 0)
                        {
                            if (vLaserDirection == 0)
                            {
                                lWall.width -= 1;
                                rWall.x += 1;
                                laserPosition.x -= 1;
                                laserPosition.width += 2;
                            }
                            else
                            {
                                lWall.width += 1;
                                rWall.x -= 1;
                                laserPosition.x += 1;
                                laserPosition.width -= 2;
                            }
                            if (lWall.width == 450) wallMovement = 1;
                        }
                        else
                        {
                            if (vLaserDirection == 0)
                            {
                                lWall.width -= 1;
                                rWall.x -= 1;
                                laserPosition.x -= 1;
                            }
                            else
                            {
                                lWall.width += 1;
                                rWall.x += 1;
                                laserPosition.x += 1;
                            }
                            if (lWall.width == 450) wallMovement = 0;
                        }
                    }
                    
                    if (vLaserDirection == 0) vLaserPosition.x += 5.0f; else vLaserPosition.x -= 5.0f;
                    if (vLaserPosition.x < lWall.width) vLaserDirection = 0; else if (vLaserPosition.x == rWall.x) vLaserDirection = 1;
                    
                    DrawCircleV(playerPosition, 10, MAROON);
                    
                    DrawRectangleRec(floor, LIGHTGRAY);
                    DrawRectangleRec(ceiling, LIGHTGRAY);
                    DrawRectangleRec(rWall, LIGHTGRAY);
                    DrawRectangleRec(lWall, LIGHTGRAY);
                    
                    DrawText(TextFormat("%i", score), screenW / 2, 100, 40, BLACK);
                    DrawText("Press `W` for up, `A` and `D` for left/right. Avoid walls and dodge lasers.", 10, 10, 20, BLACK);
                    DrawText(TextFormat("High Score: %i", HighScore), screenW - 250, 10, 25, BLACK);
                    
                    if (laser) DrawRectangleRec(laserPosition, MAROON); else DrawRectangleRec(laserPosition, LIGHTGRAY);
                    if (laser) DrawRectangleRec(vLaserPosition, LIGHTGRAY); else DrawRectangleRec(vLaserPosition, MAROON);
                    
                    if (IsKeyDown('W') && playerPosition.y > ceilingH) playerPosition.y -= 10.0f;
                    if (IsKeyDown('A') && playerPosition.x > 0) playerPosition.x -= 5.0f;
                    if (IsKeyDown('D') && playerPosition.x < screenW) playerPosition.x += 5.0f;
                    
                    if (CheckCollisionCircleRec(playerPosition, 10, floor) || CheckCollisionCircleRec(playerPosition, 10, ceiling) || (laser && CheckCollisionCircleRec(playerPosition, 10, laserPosition)) || (!laser && CheckCollisionCircleRec(playerPosition, 10, vLaserPosition)) || CheckCollisionCircleRec(playerPosition, 10, lWall) || CheckCollisionCircleRec(playerPosition, 10, rWall))
                    {
                        if (score > HighScore)
                        {
                            HighScore = score;
                            SaveHighScore(score);
                            if (!IsSoundPlaying(highscoreSound) && IsAudioDeviceReady()) PlaySound(highscoreSound);
                        } else if (!IsSoundPlaying(gameOverSound) && IsAudioDeviceReady()) PlaySound(gameOverSound);
                        
                        GameState = 4;
                        laser = false;
                        playerPosition.x = (float)screenW / 2;
                        playerPosition.y = (float)screenH / 2;
                        ceilingH = 100;
                        ceiling.height = 100;
                        floorH = 800;
                        floor.y = 800;
                        vLaserPosition.y = ceilingH;
                        vLaserPosition.height = floorH - ceilingH;
                        vLaserPosition.x = 0;
                        vLaserDirection = 0;
                        lWall.width = 0;
                        rWall.x = screenW;
                        laserPosition.x = 0;
                        laserPosition.width = screenW;
                    }
                    else playerPosition.y += 5.0f;
                    break;
                break;
                case 2:
                    int j;
                    for (j = 0; j < 3; j++)
                    {
                        if (j == optionSelection) DrawText(TextFormat("%s", options[j]), 100, 100 + j * 50, 50, GRAY);
                        else DrawText(TextFormat("%s", options[j]), 100, 100 + j * 50, 50, LIGHTGRAY);
                    }
                    if (IsKeyPressed('S')) IncrementMenuSelection(&optionSelection, 3, menuSound);
                    if (IsKeyPressed('W')) DecrementMenuSelection(&optionSelection, 3, menuSound);
                    if (IsKeyPressed('E'))
                    {
                        if (IsAudioDeviceReady()) PlaySound(selectSound);
                        switch (optionSelection)
                        {
                            case 0:
                            ChangeVolume(&options[0], &MasterVolume);
                            SetMasterVolume(MasterVolume);
                            SaveSettings(MasterVolume);
                            break;
                            case 1:
                            HighScore = 0;
                            SaveHighScore(HighScore);
                            break;
                            case 2:
                            optionSelection = 0;
                            GameState = 0;
                            break;
                        }
                    }
                    break;
                case 3:
                    Shutdown();
                    return 0;
                case 4:
                    if (IsSoundPlaying(ingameSound) && IsAudioDeviceReady()) StopSound(ingameSound);
                    DrawText("Press 'R' to Restart or 'Q' for Main Menu.", screenW / 6, screenH - 200, 50, LIGHTGRAY);
                    DrawText(TextFormat("You've scored: %i", score), 200, screenH / 2, 100, MAROON);
                    
                    if (score == HighScore) 
                        DrawText("*New High Score!", 600, screenH / 2.5, 20, MAROON);
                    
                    if (IsKeyPressed('R'))
                    {
                        GameState = 1;
                        score = 0;
                    }
                    if (IsKeyPressed('Q'))
                    {
                        GameState = 0;
                        score = 0;
                    }
            }
        EndDrawing();
    }
    Shutdown();
    return 0;
}

void IncrementMenuSelection(int * selection, int count, Sound menu)
{
    if (*selection < count - 1)(*selection)++; else *selection = 0;
    if (IsAudioDeviceReady()) PlaySound(menu);
}

void DecrementMenuSelection(int * selection, int count, Sound menu)
{
    if (*selection == 0) *selection = count - 1; else (*selection)--;
    if (IsAudioDeviceReady()) PlaySound(menu);
}

void Shutdown()
{
    CloseAudioDevice();
    CloseWindow();
}

void ChangeVolume(char **text, float *master)
{
    if (*master == 1.0f)
    {
        *text = "Volume: Med";
        *master = 0.5f;
    }
    else if (*master == 0.5f)
    {
        *text = "Volume: Low";
        *master = 0.25f;
    }
    else if (*master == 0.25f)
    {
        *text = "Volume: Off";
        *master = 0.0f;
    }
    else
    {
        *text = "Volume: Full";
        *master = 1.0f;
    }
}

char * DetermineVolumeText(float master)
{
    if (master == 1.0f)
        return "Volume: Full";
    else if (master == 0.5f)
        return "Volume: Med";
    else if (master == 0.25f)
        return "Volume: Low";
    else 
        return "Volume: Off";
}

float LoadSettings() 
{
    float volume = 1.0f;
    FILE *file = fopen("settings.dat", "rb");
    if (file != NULL)
    {
        fread(&volume, sizeof(float), 1, file);
        fclose(file);
    }
    return volume;
}

void SaveSettings(float volume)
{
    FILE *file = fopen("settings.dat", "wb");
    if (file != NULL)
    {
        fwrite(&volume, sizeof(float), 1, file);
        fclose(file);
    }
}

int LoadHighScore()
{
    int highScore = 0;
    FILE *file = fopen("score.dat", "rb");
    if (file != NULL)
    {    
        fread(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
    return highScore;
}

void SaveHighScore(int highScore)
{
    FILE *file = fopen("score.dat", "wb");
    if (file != NULL)
    {
        fwrite(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
}