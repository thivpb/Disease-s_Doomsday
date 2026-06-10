#include "player.h"
#include "../../include/game.h"
#include "../../include/gameplay.h"
#include "../../include/spatial_grid.h"
#include "../../include/asset_manager.h"
#include "../../src/entities/projectiles.h"
#include "raymath.h"

void PlayerAttack(GameState *game, Vector2 worldMousePos)
{
    if (game->player.attackCooldown > 0.0f) return;

    int wpn = game->player.equippedWeapon;
    int danoBase = game->player.attackPower;
    if (game->player.attackBoostTimer > 0.0f) danoBase *= 2; // Buff de ataque dobra dano

    if (wpn == 1) {
        // Define cooldown base
        game->player.attackCooldown = 0.22f;
        PlaySound(g_assets.sfxAttack);

        // Configura animação de ataque (Slash)
        game->slashAnimTimer = 0.22f;
        game->slashAnimPos = game->player.position;
        game->slashAnimRadius = 140.0f;

        // Efeitos visuais: explosão de partículas de slash (brancas e azul claro)
        SpawnParticleExplosion(game, game->player.position, LIGHTGRAY, 12, 100.0f, 250.0f, 4.0f, 0.4f);
        SpawnParticleExplosion(game, game->player.position, SKYBLUE, 8, 150.0f, 300.0f, 3.5f, 0.35f);

        // Câmera dá uma leve chacoalhada no ataque
        game->screenShake = 0.25f;

        Vector2 toMouse = Vector2Subtract(worldMousePos, game->player.position);
        float attackAngle = atan2f(toMouse.y, toMouse.x);

        // Verifica colisão com inimigos na área do ataque usando Spatial Grid
        int collIndices[MAX_ENEMIES];
        int collCount = GetEnemiesInRadius(game, game->player.position, game->slashAnimRadius, collIndices);

        for (int k = 0; k < collCount; k++)
        {
            int i = collIndices[k];
            if (!game->enemies[i].active || game->enemies[i].state == DEATH) continue;

            // Cone de ataque de 90 graus (PI/2 = +-45 de cada lado = 90 total)
            Vector2 toEnemy = Vector2Subtract(game->enemies[i].position, game->player.position);
            float enemyAngle = atan2f(toEnemy.y, toEnemy.x);
            float angleDiff = fabs(enemyAngle - attackAngle);
            if (angleDiff > PI) angleDiff = 2.0f * PI - angleDiff;
            
            if (angleDiff > PI / 2.0f) continue; // +-90 graus

            // Acertou! Causa dano
            game->enemies[i].hp -= (15 + danoBase);
            PlaySound(g_assets.sfxEnemyHurt);

            // Empurrão (Knockback) na direção oposta ao jogador
            Vector2 knockbackDir = Vector2Subtract(game->enemies[i].position, game->player.position);
            if (knockbackDir.x == 0.0f && knockbackDir.y == 0.0f) knockbackDir = (Vector2){ 1.0f, 0.0f };
            knockbackDir = Vector2Normalize(knockbackDir);
            
            // Empurra o inimigo a uma distância segura
            game->enemies[i].position = Vector2Add(game->enemies[i].position, Vector2Scale(knockbackDir, 55.0f));

            // Partículas de sangue/dano no local do inimigo
            Color hitColor = (game->enemies[i].type == 2) ? MAROON : RED;
            SpawnParticleExplosion(game, game->enemies[i].position, hitColor, 15, 80.0f, 180.0f, 3.0f, 0.5f);

            // Se o inimigo morreu
            if (game->enemies[i].hp <= 0)
            {
                game->enemies[i].state = DEATH;
                game->enemies[i].cooldownTimer = 0.5f; // Duração da animação de morte

                // Partículas de morte e ganho de xp
                SpawnParticleExplosion(game, game->enemies[i].position, GOLD, 20, 50.0f, 150.0f, 4.0f, 0.7f);

                // Durante o tutorial NÃO contabilizamos onda nem score real
                if (game->inTutorial)
                {
                    // Apenas efeito visual de vitória na seringa
                    SpawnParticleExplosion(game, game->enemies[i].position, (Color){0, 220, 120, 255}, 15, 60.0f, 140.0f, 4.0f, 0.7f);
                    continue;
                }

                // --- Lógica normal de jogo (fora do tutorial) ---
                game->enemiesRemaining--;
                game->totalEnemiesKilled++;

                // Aumenta score
                int xpGanho = 20 * (game->enemies[i].type + 1);
                int scoreGanho = 100 * (game->enemies[i].type + 1);
                game->player.score += scoreGanho;
                game->player.xp += xpGanho;

                // Chance de drop de PowerUp (25%)
                if (GetRandomValue(0, 100) < 25)
                {
                    SpawnPowerUpAt(game, game->enemies[i].position, -1);
                }

                // Verifica se eliminou todos os inimigos da onda
                if (game->enemiesRemaining <= 0)
                {
                    game->wave++;
                    if (game->wave > 5)
                    {
                        RequestLoadingScreen(game, LOAD_TO_VICTORY, 2.5f);
                    }
                    else
                    {
                        game->currentScreen = SCREEN_QUIZ;
                    }
                    return;
                }
            }
            else
            {
                // Se o inimigo não morreu, entra em estado de ferimento (stun/flash)
                game->enemies[i].state = HURT;
                game->enemies[i].cooldownTimer = 0.25f; // flash/stun de 0.25s
            }
        }
    }
    else if (wpn == 2) {
        game->player.attackCooldown = 0.15f;
        PlaySound(g_assets.sfxAttack);
        game->screenShake = 0.1f;
        SpawnProjectile(game, game->player.position, worldMousePos, PROJ_PLAYER_RIFLE, 8 + danoBase);
    }
    else if (wpn == 3) {
        game->player.attackCooldown = 1.5f;
        PlaySound(g_assets.sfxAttack);
        SpawnProjectile(game, game->player.position, worldMousePos, PROJ_PLAYER_GRENADE, 40 + danoBase);
    }
    else if (wpn == 4) {
        game->player.attackCooldown = 5.0f;
        PlaySound(g_assets.sfxAttack);
        game->screenShake = 0.8f;
        SpawnProjectile(game, game->player.position, worldMousePos, PROJ_PLAYER_BFG, 100 + danoBase);
    }
}
