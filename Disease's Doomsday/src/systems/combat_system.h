#ifndef COMBAT_SYSTEM_H
#define COMBAT_SYSTEM_H

#include "../../include/game.h"

// Calcula o dano e aplica efeitos quando um inimigo acerta o jogador
void HandlePlayerEnemyCollision(GameState *game, Enemy *enemy);

// Calcula o dano e aplica efeitos quando um projétil acerta o jogador
void HandleProjectileCollision(GameState *game, Projectile *proj);

#endif // COMBAT_SYSTEM_H
