// map_seringa.c
// Implementação do Mapa: Interior da Seringa de Vacina (Tutorial)
// Disease's Doomsday — Projeto de Saúde Pública / DF
#include "map_seringa.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>

// ============================================================================
// DrawMapSeringa — Renderiza o interior da seringa (chamar dentro BeginMode2D)
// ============================================================================
void DrawMapSeringa(Font font, int tutorialStep, float time, float injectionTimer)
{
    // -------------------------------------------------------------------------
    // LÍQUIDO DA VACINA (fundo translúcido azul médico)
    // -------------------------------------------------------------------------
    // Fundo azul médico mais intenso
    DrawRectangle(
        (int)SYR_LEFT, (int)SYR_WALL_TOP,
        (int)(SYR_RIGHT - SYR_LEFT),
        (int)(SYR_WALL_BOTTOM - SYR_WALL_TOP),
        (Color){ 10, 80, 140, 255 }
    );
    // Gradiente / Sobreposição para dar profundidade de líquido
    DrawRectangleGradientV(
        (int)SYR_LEFT, (int)SYR_WALL_TOP,
        (int)(SYR_RIGHT - SYR_LEFT),
        (int)(SYR_WALL_BOTTOM - SYR_WALL_TOP),
        Fade((Color){ 0, 180, 220, 255 }, 0.4f),
        Fade((Color){ 0, 50, 100, 255 }, 0.8f)
    );

    // Grade removida a pedido do usuário

    // Partículas flutuantes (bolhas do líquido da vacina)
    for (int i = 0; i < 40; i++)
    {
        // Movimento da direita para a esquerda e leve oscilação vertical
        float bx = SYR_LEFT + fmodf(i * 137.5f - time * (20.0f + (i%15)), SYR_RIGHT - SYR_LEFT);
        if (bx < SYR_LEFT) bx += (SYR_RIGHT - SYR_LEFT); // wrap
        
        float by = SYR_WALL_TOP + fmodf(i * 93.1f + sinf(time * 1.5f + i) * 15.0f, SYR_WALL_BOTTOM - SYR_WALL_TOP);
        float bradius = 2.0f + (i % 5);
        
        // Efeito de pulsação na bolha
        float alpha = 0.2f + 0.2f * sinf(time * 3.0f + i);
        
        DrawCircleV((Vector2){ bx, by }, bradius, Fade((Color){ 180, 240, 255, 255 }, alpha));
        // Brilho na bolha
        DrawCircleV((Vector2){ bx - bradius*0.3f, by - bradius*0.3f }, bradius*0.4f, Fade(WHITE, alpha * 1.5f));
    }

    // -------------------------------------------------------------------------
    // PAREDES LATERAIS DO CILINDRO (bordas plásticas cinza-metal)
    // -------------------------------------------------------------------------
    // Cima
    DrawRectangle((int)SYR_LEFT, 0,
                  (int)(SYR_RIGHT - SYR_LEFT), (int)SYR_WALL_TOP,
                  (Color){ 155, 175, 192, 255 });
    // Baixo
    DrawRectangle((int)SYR_LEFT, (int)SYR_WALL_BOTTOM,
                  (int)(SYR_RIGHT - SYR_LEFT), SYRINGE_HEIGHT - (int)SYR_WALL_BOTTOM,
                  (Color){ 155, 175, 192, 255 });

    // -------------------------------------------------------------------------
    // ÊMBOLO NA DIREITA (bloco cinza-escuro + anel de vedação)
    // -------------------------------------------------------------------------
    float plungerX = SYR_RIGHT;
    if (injectionTimer > 0.0f)
    {
        // Empurra o êmbolo rapidamente para a esquerda durante a cutscene (1.5s duração)
        float progress = injectionTimer / 1.5f;
        if (progress > 1.0f) progress = 1.0f;
        plungerX = SYR_RIGHT - (progress * (SYR_RIGHT - SYR_LEFT - 40.0f));
    }

    DrawRectangle((int)plungerX, 0, SYRINGE_WIDTH - (int)plungerX, SYRINGE_HEIGHT, (Color){ 128, 148, 162, 255 });
    // Anel de borracha do êmbolo (preto azulado)
    DrawRectangle(
        (int)(plungerX - 16), (int)(SYR_WALL_TOP - 8),
        24, (int)(SYR_WALL_BOTTOM - SYR_WALL_TOP + 16),
        (Color){ 40, 50, 60, 255 }
    );
    DrawRectangleLinesEx((Rectangle){ (int)plungerX, 0, SYRINGE_WIDTH - (int)plungerX, SYRINGE_HEIGHT }, 3.0f,
                          (Color){ 80, 100, 118, 255 });
    // Haste do êmbolo (centro)
    DrawRectangle((int)plungerX, SYRINGE_HEIGHT / 2 - 14, SYRINGE_WIDTH - (int)plungerX, 28,
                  (Color){ 100, 120, 138, 255 });

    // -------------------------------------------------------------------------
    // AFUNILAMENTO INFERIOR (paredes convergentes em direção ao bocal, agora na ESQUERDA)
    // -------------------------------------------------------------------------
    // Triângulo superior
    DrawTriangle(
        (Vector2){ SYR_TAPER_X, SYR_WALL_TOP },
        (Vector2){ SYR_LEFT, (float)SYRINGE_HEIGHT / 2.0f - 50.0f },
        (Vector2){ SYR_TAPER_X, 0.0f },
        (Color){ 155, 175, 192, 255 }
    );
    // Triângulo inferior
    DrawTriangle(
        (Vector2){ SYR_TAPER_X, SYRINGE_HEIGHT },
        (Vector2){ SYR_LEFT, (float)SYRINGE_HEIGHT / 2.0f + 50.0f },
        (Vector2){ SYR_TAPER_X, SYR_WALL_BOTTOM },
        (Color){ 155, 175, 192, 255 }
    );

    // -------------------------------------------------------------------------
    // BOCAL / AGULHA (mais detalhada)
    // -------------------------------------------------------------------------
    // Conector Luer-Lock (onde a agulha prende na seringa)
    DrawRectangle((int)SYR_NEEDLE_X + 15, (int)SYR_NEEDLE_Y - 5,
                  (int)SYR_NEEDLE_W - 15, (int)SYR_NEEDLE_H + 10,
                  (Color){ 200, 220, 230, 255 });
    DrawRectangleLinesEx(
        (Rectangle){ SYR_NEEDLE_X + 15, SYR_NEEDLE_Y - 5, SYR_NEEDLE_W - 15, SYR_NEEDLE_H + 10 },
        3.0f, (Color){ 100, 128, 158, 255 }
    );

    // Canhão metálico da agulha
    DrawRectangle((int)SYR_NEEDLE_X, (int)SYR_NEEDLE_Y + 10,
                  15, (int)SYR_NEEDLE_H - 20,
                  (Color){ 175, 195, 210, 255 });
    
    // Haste comprida e fina da agulha (indo para a esquerda)
    DrawRectangle((int)SYR_NEEDLE_X - 120, (int)SYR_NEEDLE_Y + 25,
                  120, 10,
                  (Color){ 220, 230, 240, 255 });
    DrawRectangleLinesEx(
        (Rectangle){ SYR_NEEDLE_X - 120, SYR_NEEDLE_Y + 25, 120, 10 },
        1.0f, (Color){ 100, 120, 140, 255 }
    );
    // Ponta chanfrada afiada da agulha
    DrawTriangle(
        (Vector2){ SYR_NEEDLE_X - 140.0f, (float)SYRINGE_HEIGHT / 2.0f },
        (Vector2){ SYR_NEEDLE_X - 120.0f, SYR_NEEDLE_Y + 35.0f },
        (Vector2){ SYR_NEEDLE_X - 120.0f, SYR_NEEDLE_Y + 25.0f },
        (Color){ 220, 230, 240, 255 }
    );

    // -------------------------------------------------------------------------
    // GATILHO DE SAÍDA — pisca em verde no passo 2
    // -------------------------------------------------------------------------
    if (tutorialStep == 2)
    {
        float pulse = sinf(time * 5.0f) * 0.35f + 0.65f;
        DrawRectangle(
            (int)SYR_EXIT_X, (int)SYR_EXIT_Y,
            (int)SYR_EXIT_W, (int)SYR_EXIT_H,
            Fade((Color){ 0, 220, 100, 255 }, pulse * 0.42f)
        );
        DrawRectangleLinesEx(
            (Rectangle){ SYR_EXIT_X, SYR_EXIT_Y, SYR_EXIT_W, SYR_EXIT_H },
            2.5f, (Color){ 0, 220, 100, 255 }
        );
        // Seta indicativa para a ESQUERDA
        float arrowX = SYR_EXIT_X + SYR_EXIT_W / 2.0f - 10.0f;
        float arrowY = SYR_EXIT_Y + SYR_EXIT_H / 2.0f;
        DrawTriangle(
            (Vector2){ arrowX - 14, arrowY },
            (Vector2){ arrowX + 10, arrowY + 18 },
            (Vector2){ arrowX + 10, arrowY - 18 },
            Fade(WHITE, pulse * 0.85f)
        );
        Vector2 saidaSz = MeasureTextEx(font, "SAIDA", 14.0f, 1.0f);
        DrawTextEx(font, "SAIDA",
            (Vector2){ SYR_EXIT_X + SYR_EXIT_W / 2.0f - saidaSz.x / 2.0f, arrowY + 25 },
            14.0f, 1.0f, Fade(WHITE, 0.9f));
    }

    // -------------------------------------------------------------------------
    // BORDA GERAL DO CILINDRO (highlight metálico)
    // -------------------------------------------------------------------------
    DrawRectangleLinesEx(
        (Rectangle){ SYR_LEFT, SYR_WALL_TOP,
                     SYR_RIGHT - SYR_LEFT, SYR_WALL_BOTTOM - SYR_WALL_TOP },
        4.0f, (Color){ 95, 125, 155, 255 }
    );

    // Reflexo de luz (linha branca na parte superior do cilindro)
    DrawRectangle((int)SYR_LEFT + 8, (int)SYR_WALL_TOP + 4,
                  (int)(SYR_RIGHT - SYR_LEFT - 16), 6,
                  Fade(WHITE, 0.18f));
}

// ============================================================================
// MapSeringa_CheckExit — Verifica se o jogador está na saída (bocal/agulha)
// ============================================================================
bool MapSeringa_CheckExit(Rectangle playerRect)
{
    Rectangle exitRect = { SYR_EXIT_X, SYR_EXIT_Y, SYR_EXIT_W, SYR_EXIT_H };
    return CheckCollisionRecs(playerRect, exitRect);
}

// ============================================================================
// MapSeringa_ApplyWallCollision — Empurra o jogador para dentro dos limites
// ============================================================================
void MapSeringa_ApplyWallCollision(Vector2 *pos, float r)
{
    float wallTop    = SYR_WALL_TOP;
    float wallBottom = SYR_WALL_BOTTOM;
    float wallLeft   = SYR_LEFT;
    float wallRight  = SYR_RIGHT;

    // Afunilamento: à esquerda de SYR_TAPER_X as paredes convergem no eixo Y
    if (pos->x < SYR_TAPER_X)
    {
        float t = (SYR_TAPER_X - pos->x) / (SYRINGE_WIDTH * 0.35f);
        if (t > 1.0f) t = 1.0f;
        wallTop    = SYR_WALL_TOP    + t * (SYRINGE_HEIGHT / 2.0f - 50.0f - SYR_WALL_TOP);
        wallBottom = SYR_WALL_BOTTOM - t * (SYR_WALL_BOTTOM - (SYRINGE_HEIGHT / 2.0f + 50.0f));
    }

    if (pos->x < wallLeft  + r) pos->x = wallLeft  + r;
    if (pos->x > wallRight - r) pos->x = wallRight - r;
    if (pos->y < wallTop   + r) pos->y = wallTop   + r;
    if (pos->y > wallBottom - r) pos->y = wallBottom - r;
}
