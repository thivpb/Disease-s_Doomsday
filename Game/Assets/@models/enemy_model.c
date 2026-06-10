#include "enemy_model.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// MODELO: INIMIGOS
// ============================================================================
void DrawEnemyModel(Enemy *enemy, Vector2 renderPos, float destSize, float rotation, float squashFactor, float alpha)
{
    float currentDestSize = destSize * squashFactor;

    Color enemyCol = RED;
    if (enemy->state == HURT) {
        enemyCol = WHITE;
    } else {
        if (enemy->type == 0)      enemyCol = (Color){ 140, 50, 200, 255 }; // Roxo (SARS-CoV-2)
        else if (enemy->type == 1) enemyCol = (Color){ 80, 80, 80, 255 };   // Cinza escuro (Dengue)
        else if (enemy->type == 2) enemyCol = (Color){ 50, 200, 80, 255 };  // Verde toxico (KPC)
        else if (enemy->type == 3) enemyCol = (Color){ 30, 100, 220, 255 }; // Azul (Chagas)
        else if (enemy->type == 4) enemyCol = (Color){ 180, 60, 40, 255 };  // Marrom avermelhado (Tuberculose)
        else                       enemyCol = RED;
    }
    
    enemyCol = Fade(enemyCol, alpha);
    Color edgeCol = Fade(WHITE, alpha);

    // Tipo 0: Vírus (Círculo espinhoso)
    if (enemy->type == 0) {
        DrawPoly(renderPos, 8, currentDestSize, rotation + GetTime()*10.0f, enemyCol);
        DrawPolyLinesEx(renderPos, 8, currentDestSize, rotation + GetTime()*10.0f, 3.0f, edgeCol);
        // Spikes
        for (int i=0; i<8; i++) {
            float angle = (rotation + GetTime()*10.0f + i * 45.0f) * DEG2RAD;
            Vector2 end = { renderPos.x + cosf(angle) * currentDestSize * 1.3f, renderPos.y + sinf(angle) * currentDestSize * 1.3f };
            DrawLineEx(renderPos, end, 2.0f, enemyCol);
            DrawCircleV(end, 4.0f, edgeCol);
        }
    } 
    // Tipo 1: Dengue / Mosquito (Forma aerodinâmica / Triângulo)
    else if (enemy->type == 1) {
        DrawPoly(renderPos, 3, currentDestSize, rotation + GetTime()*20.0f, enemyCol);
        DrawPolyLines(renderPos, 3, currentDestSize, rotation + GetTime()*20.0f, edgeCol);
        DrawCircleV(renderPos, currentDestSize * 0.4f, RED); // Olho vermelho
    } 
    // Tipo 2: KPC / Superbactéria (Tanque Hexagonal)
    else if (enemy->type == 2) {
        int sides = 6;
        float rotSpeed = 2.0f;
        DrawPoly(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, enemyCol);
        DrawPolyLinesEx(renderPos, sides, currentDestSize, rotation + GetTime()*rotSpeed, 4.0f, edgeCol);
        DrawCircleV(renderPos, currentDestSize * 0.5f, BLACK);
    }
    // Tipo 3: Trypanosoma cruzi / Chagas (Forma alongada ou Estrela)
    else if (enemy->type == 3) {
        DrawPoly(renderPos, 4, currentDestSize, rotation + GetTime()*25.0f, enemyCol);
        DrawPoly(renderPos, 4, currentDestSize*0.7f, -rotation - GetTime()*25.0f, SKYBLUE);
    }
    // Tipo 4: Tuberculose (Pentágono)
    else if (enemy->type == 4) {
        DrawPoly(renderPos, 5, currentDestSize, rotation + GetTime()*5.0f, enemyCol);
        DrawPolyLines(renderPos, 5, currentDestSize, rotation + GetTime()*5.0f, edgeCol);
    }
}
