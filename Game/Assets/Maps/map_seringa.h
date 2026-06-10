// map_seringa.h
// Mapa do Tutorial: Interior da Seringa de Vacina
// Disease's Doomsday — Projeto de Saúde Pública / DF
#ifndef MAP_SERINGA_H
#define MAP_SERINGA_H

#include "raylib.h"
#include "../../include/game.h"

// ============================================================================
// CONSTANTES GEOMÉTRICAS DA SERINGA
// ============================================================================

#define SYR_WALL_TOP     80.0f
#define SYR_WALL_BOTTOM  (SYRINGE_HEIGHT - 80.0f)
#define SYR_RIGHT        (SYRINGE_WIDTH - 60.0f)
#define SYR_LEFT         40.0f
#define SYR_TAPER_X      (SYRINGE_WIDTH * 0.35f)   // Início do afunilamento

// Bocal / agulha (ponto de saída, na esquerda)
#define SYR_NEEDLE_Y     (SYRINGE_HEIGHT / 2.0f - 30.0f)
#define SYR_NEEDLE_H     60.0f
#define SYR_NEEDLE_W     60.0f
#define SYR_NEEDLE_X     (SYR_LEFT - SYR_NEEDLE_W)

// Retângulo de gatilho de saída (colisão para avançar ao organismo)
#define SYR_EXIT_X       (SYR_LEFT - 40.0f)
#define SYR_EXIT_Y       (SYRINGE_HEIGHT / 2.0f - 55.0f)
#define SYR_EXIT_W       80.0f
#define SYR_EXIT_H       110.0f

// ============================================================================
// API DO MAPA
// ============================================================================

// Desenha o interior completo da Seringa (deve ser chamado dentro de BeginMode2D)
// tutorialStep: 0=movimento, 1=combate, 2=saída  |  time: GetTime() para animações
void DrawMapSeringa(Font font, int tutorialStep, float time, float injectionTimer);

// Retorna true se o playerRect colidiu com o bocal de saída da seringa
bool MapSeringa_CheckExit(Rectangle playerRect);

// Aplica colisão das paredes do cilindro (corrige playerPos in-place)
// playerRadius: raio de colisão circular do jogador
void MapSeringa_ApplyWallCollision(Vector2 *playerPos, float playerRadius);

#endif // MAP_SERINGA_H
