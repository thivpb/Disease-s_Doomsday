#include "../../include/gameplay.h"
#include "../../include/spatial_grid.h"
#include "../../Assets/Maps/map_seringa.h"
#include "../systems/combat_system.h"
#include "../systems/wave_manager.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/asset_manager.h"
#include <string.h>
#include <time.h>
#include <process.h>
#include <math.h>
#include <time.h>
#include <string.h>

// ============================================================================
// AUXILIAR: EMISSOR DE PARTÍCULAS
// ============================================================================
void InitParticlePool(GameState *game)
{
    game->particlePoolCount = MAX_PARTICLES;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        game->particles[i].active = false;
        game->particlePool[i] = MAX_PARTICLES - 1 - i;
    }
}

void FreeParticle(GameState *game, int idx)
{
    if (game->particles[idx].active) {
        game->particles[idx].active = false;
        if (game->particlePoolCount < MAX_PARTICLES) {
            game->particlePool[game->particlePoolCount++] = idx;
        }
    }
}

void SpawnParticle(GameState *game, Vector2 position, Vector2 velocity, Color color, float size, float lifeTime)
{
    if (game->particlePoolCount > 0)
    {
        game->particlePoolCount--;
        int idx = game->particlePool[game->particlePoolCount];

        game->particles[idx].position = position;
        game->particles[idx].velocity = velocity;
        game->particles[idx].color = color;
        game->particles[idx].size = size;
        game->particles[idx].lifeTime = lifeTime;
        game->particles[idx].maxLifeTime = lifeTime;
        game->particles[idx].active = true;
    }
}

// Emite uma explosão radial de partículas
void SpawnParticleExplosion(GameState *game, Vector2 pos, Color col, int count, float minSpeed, float maxSpeed, float size, float life)
{
    for (int i = 0; i < count; i++)
    {
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue((int)(minSpeed * 10), (int)(maxSpeed * 10)) / 10.0f;
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
        SpawnParticle(game, pos, vel, col, size, life + (float)GetRandomValue(-2, 2)/10.0f);
    }
}

// ============================================================================
// AUXILIAR: SPAWN DE POWER-UPS
// ============================================================================
void SpawnPowerUpAt(GameState *game, Vector2 position, int forcedType)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!game->powerUps[i].active)
        {
            game->powerUps[i].position = position;
            game->powerUps[i].type = (forcedType >= 0) ? (PowerUpType)forcedType : (PowerUpType)GetRandomValue(0, 3);
            game->powerUps[i].active = true;
            game->powerUps[i].pulseTimer = 0.0f;
            break;
        }
    }
}


// ============================================================================
// INICIALIZAÇÃO DO JOGO
// ============================================================================
void InitGame(GameState *game)
{
    // Preserva o nome do jogador se ja estiver definido
    char tempName[16] = "";
    float tempVol = 1.0f;
    if (game != NULL)
    {
        snprintf(tempName, sizeof(tempName), "%s", game->player.name);
        tempVol = game->masterVolume;
    }

    memset(game, 0, sizeof(GameState));

    if (tempName[0] != '\0')
    {
        snprintf(game->player.name, sizeof(game->player.name), "%s", tempName);
    }
    else
    {
        strcpy(game->player.name, "HERO");
    }
    game->masterVolume = tempVol;

    // Jogador inicial
    game->player.position = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
    // Status base padrao
    game->player.speed = 280.0f;
    game->player.maxHp = 100;
    game->player.attackPower = 15;


    game->player.hp = game->player.maxHp;
    game->player.score = 0;
    game->player.level = 1;
    game->player.xp = 0;
    game->player.xpNeeded = 100;
    game->player.attackCooldown = 0.0f;
    game->player.squashX = 1.0f;
    game->player.squashY = 1.0f;
    game->player.trailIndex = 0;
    game->player.equippedWeapon = 1; // 1 = Lâmina Imunológica
    game->player.healthPotions = 3;
    game->player.poisonTimer = 0.0f;
    game->player.slowTimer = 0.0f;
    game->player.speedTimer = 0.0f;
    game->player.shieldTimer = 0.0f;
    game->player.attackBoostTimer = 0.0f;
    game->player.facingDir = 1;
    game->player.isMoving = false;
    for (int i = 0; i < 10; i++) game->player.trail[i] = game->player.position;

    // Sistema
    game->wave = 1;
    game->totalEnemiesKilled = 0;
    game->timeElapsed = 0.0f;
    game->screenShake = 0.0f;
    game->slashAnimTimer = 0.0f;

    // Câmera
    game->camera.target = game->player.position;
    game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    // Limpa vetores
    for (int i = 0; i < MAX_ENEMIES; i++) game->enemies[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
    InitParticlePool(game);

    // Inicia o Tutorial (Seringa de Vacina) em vez de ir diretamente à onda 1
    InitTutorial(game);
}



// ============================================================================
// TUTORIAL: INICIALIZAÇÃO DA SERINGA DE VACINA
// ============================================================================
void InitTutorial(GameState *game)
{
    // Reseta flags do tutorial
    game->inTutorial           = true;
    game->tutorialStep         = 0;
    game->tutorialTimer        = 0.0f;
    game->tutorialEnemySpawned = false;

    // Inicia o diálogo do passo 0 (página 0)
    game->tutorialDialog.active    = true;
    game->tutorialDialog.page      = 0;
    game->tutorialDialog.charShown = 0;
    game->tutorialDialog.charTimer = 0.0f;

    // Reposiciona o jogador no canto direito da Seringa (para andar para a esquerda)
    game->player.position = (Vector2){ SYRINGE_WIDTH - 200.0f, SYRINGE_HEIGHT / 2.0f };

    // Ajusta a câmera para o mapa menor da seringa
    game->camera.target = game->player.position;
    game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    game->camera.zoom   = 1.0f;

    // Limpa power-ups anteriores
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;

    // Define a tela como Tutorial
    game->currentScreen = SCREEN_TUTORIAL;
}

// ============================================================================
// NÚmero de páginas de diálogo por passo do tutorial
// ============================================================================
static const int DIALOG_PAGES_PER_STEP[3] = { 2, 2, 2 };

// ============================================================================
// TUTORIAL: LÓGICA DE ATUALIZAÇÃO (3 PASSOS + SISTEMA DE DIÁLOGO)
// ============================================================================
void UpdateTutorial(GameState *game, float delta)
{
    UpdateGrid(game);
    DialogState *dlg = &game->tutorialDialog;

    // ========================================================================
    // SISTEMA DE DIÁLOGO: Typewriter + Q/ESPAÇO para avançar página
    // ========================================================================
    if (dlg->active)
    {
        // Obtém o texto do diálogo para o passo e página atuais para calcular o comprimento total
        const char *l1, *l2, *l3;
        GetTutorialDialogText(game->tutorialStep, dlg->page, &l1, &l2, &l3);
        int totalLen = strlen(l1) + strlen(l2) + strlen(l3);

        // --- Typewriter: revela um caractere a cada ~0.03s ---
        float typewriterSpeed = 0.030f; // segundos por caractere
        dlg->charTimer += delta;
        if (dlg->charTimer >= typewriterSpeed)
        {
            dlg->charTimer -= typewriterSpeed;
            if (dlg->charShown < totalLen)
            {
                dlg->charShown++;
            }
        }

        // --- Q ou ESPAÇO avançam o diálogo ---
        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_SPACE))
        {
            if (dlg->charShown < totalLen)
            {
                // Se o texto ainda não terminou de aparecer: pula pro fim
                dlg->charShown = totalLen;
            }
            else
            {
                int maxPages = DIALOG_PAGES_PER_STEP[game->tutorialStep];

                // Verifica se ainda faltam páginas no passo atual
                if (dlg->page < maxPages - 1)
                {
                    // Próxima página
                    dlg->page++;
                    dlg->charShown = 0;
                    dlg->charTimer = 0.0f;
                }
                else
                {
                    // Última página do passo: fecha o diálogo e libera a gameplay
                    dlg->active    = false;
                    dlg->page      = 0;
                    dlg->charShown = 0;
                    dlg->charTimer = 0.0f;
                }
            }
        }
        // Diálogo ativo = só anima câmera, não move jogador nem processa passo
        game->camera.target.x += (game->player.position.x - game->camera.target.x) * 0.10f;
        game->camera.target.y += (game->player.position.y - game->camera.target.y) * 0.10f;
        return; // bloqueia resto do update durante o diálogo
    }

    // ========================================================================
    // MOVIMENTAÇÃO DO JOGADOR (só quando diálogo está fechado)
    // ========================================================================
    float currentSpeed = game->player.speed;
    if (game->player.speedTimer > 0.0f) currentSpeed *= 1.6f;

    Vector2 moveDir = { 0.0f, 0.0f };
    // O jogador no tutorial (seringa) só pode se mover no eixo X
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { moveDir.x += 1.0f; game->player.facingDir = 1; }
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) { moveDir.x -= 1.0f; game->player.facingDir = -1; }

    bool moving = (moveDir.x != 0.0f || moveDir.y != 0.0f);
    game->player.isMoving = moving;

    if (moving)
    {
        moveDir = Vector2Normalize(moveDir);
        game->player.position = Vector2Add(
            game->player.position,
            Vector2Scale(moveDir, currentSpeed * delta)
        );
        // Partículas de rastro suave
        if (GetRandomValue(0, 8) == 0)
        {
            Vector2 smokeVel = { -moveDir.x * 25.0f, -moveDir.y * 25.0f };
            SpawnParticle(game, game->player.position, smokeVel, Fade((Color){100,220,150,255}, 0.4f), 4.0f, 0.5f);
        }
    }


    // Colisão com paredes do mapa (via função do mapa da seringa)
    MapSeringa_ApplyWallCollision(&game->player.position, 20.0f);

    // --- Atualiza partículas ---
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].velocity.y += 150.0f * delta;
            game->particles[i].velocity.x *= 0.96f; // Drag

            game->particles[i].position = Vector2Add(
                game->particles[i].position,
                Vector2Scale(game->particles[i].velocity, delta)
            );
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f)
                game->particles[i].active = false;
        }
    }

    // Decaimento de cooldowns e buffs do jogador no tutorial
    if (game->player.attackCooldown > 0.0f) game->player.attackCooldown -= delta;
    if (game->player.damageCooldown > 0.0f) game->player.damageCooldown -= delta;
    if (game->player.speedTimer > 0.0f) game->player.speedTimer -= delta;
    if (game->player.shieldTimer > 0.0f) game->player.shieldTimer -= delta;
    if (game->player.attackBoostTimer > 0.0f) game->player.attackBoostTimer -= delta;
    if (game->slashAnimTimer > 0.0f) game->slashAnimTimer -= delta;

    // ========================================================================
    // PASSO 0: Andar para a esquerda (Treino de Mobilidade)
    // ========================================================================
    if (game->tutorialStep == 0)
    {
        // Avança quando passar de X=1000
        bool itemsRemaining = (game->player.position.x > SYRINGE_WIDTH - 600.0f);

        if (!itemsRemaining)
        {
            game->tutorialStep = 1;
            game->tutorialTimer = 0.0f;
            // Abre diálogo do passo 1
            dlg->active    = true;
            dlg->page      = 0;
            dlg->charShown = 0;
            dlg->charTimer = 0.0f;
        }
    }

    // ========================================================================
    // PASSO 1: Combate — spawn de bactéria móvel e atiradora
    // ========================================================================
    else if (game->tutorialStep == 1)
    {
        // Spawna a bactéria uma única vez no passo 1
        if (!game->tutorialEnemySpawned)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (!game->enemies[i].active)
                {
                    game->enemies[i].active          = true;
                    game->enemies[i].position        = (Vector2){ SYRINGE_WIDTH / 2.0f, SYRINGE_HEIGHT / 2.0f };
                    game->enemies[i].hp              = 35;
                    game->enemies[i].maxHp           = 35;
                    game->enemies[i].speed           = 85.0f;
                    game->enemies[i].type            = 0;
                    game->enemies[i].tier            = TIER_1;
                    game->enemies[i].state           = IDLE;
                    game->enemies[i].isRanged        = false; // controlada de forma customizada abaixo
                    game->enemies[i].isTutorialEnemy = true;
                    game->enemies[i].cooldownTimer   = 1.5f; // primeiro tiro após 1.5s
                    game->enemies[i].patrolTarget    = game->enemies[i].position;
                    game->enemies[i].patrolTimer     = 99.0f;
                    break;
                }
            }
            game->tutorialEnemySpawned = true;
            // Partículas de invocação
            for (int p = 0; p < 15; p++)
            {
                Vector2 vel = { (float)GetRandomValue(-80, 80), (float)GetRandomValue(-80, 80) };
                SpawnParticle(game, (Vector2){ SYRINGE_WIDTH/2.0f, SYRINGE_HEIGHT/2.0f }, vel, (Color){50,200,80,255}, 5.0f, 0.8f);
            }
        }

        // IA da bactéria tutorial e seu status de morte
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (game->enemies[i].active && game->enemies[i].isTutorialEnemy)
            {
                Enemy *e = &game->enemies[i];
                if (e->state == DEATH)
                {
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f)
                    {
                        e->active = false;
                        // Spawna a célula de cura ao morrer!
                        SpawnPowerUpAt(game, e->position, HP_RECOVERY);
                    }
                    continue;
                }

                if (e->state == HURT)
                {
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f) e->state = IDLE;
                }
                else if (e->state == ATTACK)
                {
                    e->chargeTimer -= delta;
                    if (e->chargeTimer <= 0.0f)
                    {
                        // Dispara uma toxina lenta de treino em direção ao jogador
                        Vector2 targetDir = Vector2Subtract(game->player.position, e->position);
                        targetDir = Vector2Normalize(targetDir);
                        Vector2 projVel = Vector2Scale(targetDir, 160.0f); // Tiro lento de 160 px/s

                        // Procura slot de projétil
                        for (int j = 0; j < MAX_PROJECTILES; j++)
                        {
                            if (!game->projectiles[j].active)
                            {
                                game->projectiles[j].active = true;
                                game->projectiles[j].position = e->position;
                                game->projectiles[j].velocity = projVel;
                                game->projectiles[j].type = PROJ_ACID_ARC;
                                game->projectiles[j].damage = 6;
                                game->projectiles[j].isPlayerProjectile = false;
                                game->projectiles[j].lifeTime = 8.0f;
                                game->projectiles[j].hitbox = (Rectangle){ e->position.x - 10, e->position.y - 10, 20, 20 };
                                break;
                            }
                        }
                        e->state = IDLE;
                    }
                }
                else
                {
                    // Persegue o jogador lentamente
                    Vector2 chaseDir = Vector2Subtract(game->player.position, e->position);
                    float dist = Vector2Length(chaseDir);
                    if (dist > 15.0f)
                    {
                        chaseDir = Vector2Normalize(chaseDir);
                        e->position = Vector2Add(e->position, Vector2Scale(chaseDir, e->speed * delta));
                        e->state = AGGRO;
                    }
                    else
                    {
                        e->state = IDLE;
                    }

                    // Ataca atirando a cada 3.2s se perto
                    e->cooldownTimer -= delta;
                    if (e->cooldownTimer <= 0.0f && dist < 400.0f)
                    {
                        e->state = ATTACK;
                        e->chargeTimer = 0.5f;     // Tempo de carregamento do tiro
                        e->cooldownTimer = 3.2f;   // Cooldown entre tiros
                    }
                }
            }
        }

        // Ataque com ESPAÇO ou clique (Q está reservado para diálogo)
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            extern Vector2 g_virtualMouse;
            Vector2 worldMouse = GetScreenToWorld2D(g_virtualMouse, game->camera);
            PlayerAttack(game, worldMouse);
        }
        // Verifica se a bactéria tutorial foi eliminada
        bool algumVivo = false;
        for (int i = 0; i < MAX_ENEMIES; i++)
            if (game->enemies[i].active && game->enemies[i].isTutorialEnemy)
            { algumVivo = true; break; }

        if (!algumVivo && game->tutorialEnemySpawned)
        {
            game->tutorialStep  = 2;
            game->tutorialTimer = 0.0f;
            // Abre diálogo do passo 2
            dlg->active    = true;
            dlg->page      = 0;
            dlg->charShown = 0;
            dlg->charTimer = 0.0f;
        }
    }
    
    // ========================================================================
    // PASSO 2: Saída pela agulha (bocal da seringa)
    // ========================================================================
    else if (game->tutorialStep == 2)
    {
        Rectangle playerRect = {
            game->player.position.x - 18.0f,
            game->player.position.y - 18.0f,
            36.0f, 36.0f
        };

        if (!game->injectionCutscene && MapSeringa_CheckExit(playerRect))
        {
            game->injectionCutscene = true;
            game->injectionTimer = 0.0f;
            dlg->active = false;
        }

        if (game->injectionCutscene)
        {
            game->injectionTimer += delta;
            
            // Tremor de tela
            game->screenShake = 12.0f;

            // Empurra o jogador para a esquerda
            game->player.position.x -= 800.0f * delta;

            if (game->injectionTimer > 1.5f)
            {
                game->injectionCutscene = false;
                game->inTutorial = false;
                game->screenShake = 0.0f;

                // Limpa inimigos e partículas da seringa
                for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
                InitParticlePool(game);

                // Tela de carregamento para o organismo
                RequestLoadingScreen(game, LOAD_TO_GAMEPLAY, 2.0f);
            }
        }
    }

    // ========================================================================
    // ATUALIZA COLISÕES E PROJÉTEIS DENTRO DO TUTORIAL
    // ========================================================================
    // 1. Colisão Jogador <-> Bactéria
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].active && game->enemies[i].isTutorialEnemy && game->enemies[i].state != DEATH)
        {
            float distSqr = Vector2DistanceSqr(game->player.position, game->enemies[i].position);
            if (distSqr < 35.0f * 35.0f) // colidiu
            {
                HandlePlayerEnemyCollision(game, &game->enemies[i]);
            }
        }
    }

    // 2. Colisão Projétil <-> Jogador
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            game->projectiles[i].position = Vector2Add(
                game->projectiles[i].position,
                Vector2Scale(game->projectiles[i].velocity, delta)
            );
            game->projectiles[i].hitbox.x = game->projectiles[i].position.x - 10;
            game->projectiles[i].hitbox.y = game->projectiles[i].position.y - 10;

            Rectangle pRect = { game->player.position.x - 20, game->player.position.y - 20, 40, 40 };
            if (CheckCollisionRecs(game->projectiles[i].hitbox, pRect))
            {
                HandleProjectileCollision(game, &game->projectiles[i]);
            }

            if (game->projectiles[i].position.x < -100 || game->projectiles[i].position.x > SYRINGE_WIDTH + 100 ||
                game->projectiles[i].position.y < -100 || game->projectiles[i].position.y > SYRINGE_HEIGHT + 100)
            {
                game->projectiles[i].active = false;
            }
        }
    }

    // 3. Coleta de Power-ups (ampolas de vacina / esferas de treino e cura)
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            game->powerUps[i].pulseTimer += delta;
            float distSqr = Vector2DistanceSqr(game->player.position, game->powerUps[i].position);
            if (distSqr < 35.0f * 35.0f) // coletou
            {
                game->powerUps[i].active = false;
                game->player.score += 50;
                PlaySound(g_assets.sfxPickup);

                SpawnParticleExplosion(game, game->powerUps[i].position, YELLOW, 15, 60.0f, 160.0f, 4.5f, 0.6f);

                switch (game->powerUps[i].type)
                {
                    case HP_RECOVERY:
                        game->player.hp += 35;
                        if (game->player.hp > game->player.maxHp) game->player.hp = game->player.maxHp;
                        SpawnParticleExplosion(game, game->player.position, GREEN, 15, 40.0f, 100.0f, 4.0f, 0.8f);
                        break;
                    case SPEED_BOOST:
                        game->player.speedTimer = 8.0f;
                        break;
                    case SHIELD:
                        game->player.shieldTimer = 7.0f;
                        break;
                    case ATTACK_BOOST:
                        game->player.attackBoostTimer = 8.0f;
                        break;
                }
            }
        }
    }

    // Atualiza câmera suavemente
    game->camera.target.x += (game->player.position.x - game->camera.target.x) * 0.10f;
    game->camera.target.y += (game->player.position.y - game->camera.target.y) * 0.10f;
    
    // Squash & Stretch recovery (essencial para resetar após tomar dano)
    game->player.squashX = Lerp(game->player.squashX, 1.0f, 10.0f * delta);
    game->player.squashY = Lerp(game->player.squashY, 1.0f, 10.0f * delta);
}



// ============================================================================
// CICLO PRINCIPAL DE ATUALIZAÇÃO DA LOGICA
// ============================================================================
void UpdateGameplay(GameState *game, float delta)
{
    UpdateGrid(game);
    game->timeElapsed += delta;

    // Decaimento do screen shake
    if (game->screenShake > 0.0f)
    {
        game->screenShake -= delta * 1.5f;
        if (game->screenShake < 0.0f) game->screenShake = 0.0f;
    }

    // Decaimento da animação de slash
    if (game->slashAnimTimer > 0.0f)
    {
        game->slashAnimTimer -= delta;
    }

    // ------------------------------------------------------------------------
    // 1. ATUALIZA TIMERS E STATS DO JOGADOR
    // ------------------------------------------------------------------------
    if (game->player.speedTimer > 0.0f) game->player.speedTimer -= delta;
    if (game->player.shieldTimer > 0.0f) game->player.shieldTimer -= delta;
    if (game->player.attackBoostTimer > 0.0f) game->player.attackBoostTimer -= delta;
    if (game->player.attackCooldown > 0.0f) game->player.attackCooldown -= delta;
    if (game->player.damageCooldown > 0.0f) game->player.damageCooldown -= delta;
    if (game->player.poisonTimer > 0.0f) {
        game->player.poisonTimer -= delta;
        // Dano de poison a cada frame (equivalente a 8 dano por segundo)
        game->player.hp -= 8 * delta;
        if (game->player.hp <= 0) {
            game->player.hp = 0;
            game->currentScreen = SCREEN_GAMEOVER;
            return;
        }
        if (GetRandomValue(0, 30) == 0) {
            SpawnParticle(game, game->player.position, (Vector2){0, -20}, PURPLE, 4.0f, 0.5f);
        }
    }
    if (game->player.slowTimer > 0.0f) game->player.slowTimer -= delta;

    // ------------------------------------------------------------------------
    // 2. MOVIMENTAÇÃO DO JOGADOR
    // ------------------------------------------------------------------------
    float currentSpeed = game->player.speed;
    if (game->player.speedTimer > 0.0f) currentSpeed *= 1.6f; // Buff de velocidade
    if (game->player.slowTimer > 0.0f) currentSpeed *= 0.5f;  // Debuff de velocidade

    Vector2 moveDir = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { moveDir.x += 1.0f; game->player.facingDir = 1; }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { moveDir.x -= 1.0f; game->player.facingDir = -1; }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  moveDir.y += 1.0f;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    moveDir.y -= 1.0f;

    if (moveDir.x != 0.0f || moveDir.y != 0.0f)
    {
        game->player.isMoving = true;
        moveDir = Vector2Normalize(moveDir);
        game->player.position = Vector2Add(
            game->player.position,
            Vector2Scale(moveDir, currentSpeed * delta)
        );

        // Emite fumaça suave de movimento
        if (GetRandomValue(0, 10) == 0)
        {
            Vector2 smokeVel = { -moveDir.x * 30.0f, -moveDir.y * 30.0f };
            Color pCol = (game->player.speedTimer > 0.0f) ? YELLOW : LIGHTGRAY;
            SpawnParticle(game, game->player.position, smokeVel, Fade(pCol, 0.4f), 5.0f, 0.5f);
        }
    }
    else
    {
        game->player.isMoving = false;
    }

    // Atualiza Rastro (Trail)
    static float trailTimer = 0.0f;
    trailTimer += delta;
    if (trailTimer > 0.02f) {
        game->player.trail[game->player.trailIndex] = game->player.position;
        game->player.trailIndex = (game->player.trailIndex + 1) % 10;
        trailTimer = 0.0f;
    }

    // Squash & Stretch Lerp
    game->player.squashX = Lerp(game->player.squashX, 1.0f, 10.0f * delta);
    game->player.squashY = Lerp(game->player.squashY, 1.0f, 10.0f * delta);

    // Limites do mapa para o jogador
    float playerRadius = 20.0f;
    if (game->player.position.x < playerRadius) game->player.position.x = playerRadius;
    if (game->player.position.x > MAP_WIDTH - playerRadius) game->player.position.x = MAP_WIDTH - playerRadius;
    if (game->player.position.y < playerRadius) game->player.position.y = playerRadius;
    if (game->player.position.y > MAP_HEIGHT - playerRadius) game->player.position.y = MAP_HEIGHT - playerRadius;

    // ------------------------------------------------------------------------
    // 3. ENTRADA DE COMBATE E ARMAS
    // ------------------------------------------------------------------------
    if (IsKeyPressed(KEY_ONE))   game->player.equippedWeapon = 1;
    if (IsKeyPressed(KEY_TWO))   game->player.equippedWeapon = 2;
    if (IsKeyPressed(KEY_THREE)) game->player.equippedWeapon = 3;
    if (IsKeyPressed(KEY_FOUR))  game->player.equippedWeapon = 4;

    if (IsKeyPressed(KEY_E) && game->player.healthPotions > 0 && game->player.hp < game->player.maxHp) {
        game->player.healthPotions--;
        game->player.hp += game->player.maxHp / 2; // Cura 50%
        if (game->player.hp > game->player.maxHp) game->player.hp = game->player.maxHp;
        PlaySound(g_assets.sfxPickup);
        SpawnParticleExplosion(game, game->player.position, GREEN, 20, 50.0f, 150.0f, 4.0f, 0.8f);
    }

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        extern Vector2 g_virtualMouse;
        Vector2 worldMouse = GetScreenToWorld2D(g_virtualMouse, game->camera);
        PlayerAttack(game, worldMouse);
    }

    // Entrada de combate e fluxo de save serao tratados no rpg.c para capturar a screenshot

    // ------------------------------------------------------------------------
    // 4. ATUALIZA PARTÍCULAS
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            // Partículas com gravidade direcional e arraste (viscosidade)
            game->particles[i].velocity.y += 150.0f * delta;
            game->particles[i].velocity.x *= 0.96f; // Drag
            
            game->particles[i].position = Vector2Add(
                game->particles[i].position,
                Vector2Scale(game->particles[i].velocity, delta)
            );
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f) FreeParticle(game, i);
        }
    }

    // ------------------------------------------------------------------------
    // 5. ATUALIZA POWER-UPS (COLETA)
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game->powerUps[i].active)
        {
            game->powerUps[i].pulseTimer += delta;

            // Distância jogador -> item
            float distSqr = Vector2DistanceSqr(game->player.position, game->powerUps[i].position);
            if (distSqr < 35.0f * 35.0f) // Colidiu/Coletou
            {
                game->powerUps[i].active = false;
                game->player.score += 50;
                PlaySound(g_assets.sfxPickup);

                // Efeito radial de coleta
                SpawnParticleExplosion(game, game->powerUps[i].position, YELLOW, 15, 60.0f, 160.0f, 4.5f, 0.6f);

                // Aplica efeitos baseados no tipo do Power-up
                switch (game->powerUps[i].type)
                {
                    case HP_RECOVERY:
                        game->player.hp += 35;
                        if (game->player.hp > game->player.maxHp) game->player.hp = game->player.maxHp;
                        // Partículas verdes para cura
                        SpawnParticleExplosion(game, game->player.position, GREEN, 15, 40.0f, 100.0f, 4.0f, 0.8f);
                        break;
                    case SPEED_BOOST:
                        game->player.speedTimer = 8.0f; // 8 segundos
                        break;
                    case SHIELD:
                        game->player.shieldTimer = 7.0f; // 7 segundos
                        break;
                    case ATTACK_BOOST:
                        game->player.attackBoostTimer = 8.0f; // 8 segundos
                        break;
                }
            }
        }
    }

    // ------------------------------------------------------------------------
    // 6. ATUALIZA INIMIGOS E INTELIGÊNCIA ARTIFICIAL (IA)
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!game->enemies[i].active) continue;

        Enemy *enemy = &game->enemies[i];

        if (enemy->state == DEATH)
        {
            enemy->cooldownTimer -= delta;
            if (enemy->cooldownTimer <= 0.0f)
            {
                enemy->active = false;
            }
            continue;
        }
        float distSqrToPlayer = Vector2DistanceSqr(game->player.position, enemy->position);
        
        // Status updates
        if (enemy->poisonTimer > 0.0f && enemy->state != DEATH) {
            enemy->poisonTimer -= delta;
            enemy->hp -= 15 * delta; // 15 dmg per second
            if (enemy->hp <= 0) {
                enemy->hp = 0;
                enemy->state = DEATH;
                enemy->cooldownTimer = 0.5f; // Death animation duration
                PlaySound(g_assets.sfxEnemyHurt);
                game->player.xp += (enemy->tier + 1) * 10;
                game->player.score += (enemy->tier + 1) * 50;
                if (game->enemiesRemaining > 0) game->enemiesRemaining--;
            }
        }
        if (enemy->slowTimer > 0.0f) enemy->slowTimer -= delta;

        // State Machine Update
        if (enemy->state == HURT) {
            enemy->cooldownTimer -= delta;
            if (enemy->cooldownTimer <= 0.0f) enemy->state = IDLE;
        }
        else if (enemy->state == ATTACK) {
            enemy->chargeTimer -= delta;
            if (enemy->chargeTimer <= 0.0f) {
                // Seleciona tipo de projétil baseado no tipo do inimigo
                ProjectileType ptype = PROJ_ACID_ARC;
                int dmg = 8;
                
                if (enemy->type == 1) { // Dengue: picada espalhada
                    ptype = PROJ_BULLET_SPREAD;
                    dmg = 10;
                } else if (enemy->type == 2) { // KPC: tiro pesado
                    ptype = PROJ_VOID_BOLT;
                    dmg = 20;
                } else if (enemy->type == 4) { // TB: ácido moderado
                    ptype = PROJ_ACID_ARC;
                    dmg = 12;
                }
                
                SpawnProjectile(game, enemy->position, game->player.position, ptype, dmg);
                PlaySound(g_assets.sfxEnemyShoot);
                
                // KPC e Boss disparam múltiplos projéteis
                if (enemy->tier == TIER_3_BOSS) {
                    Vector2 off1 = { game->player.position.x + 100, game->player.position.y };
                    Vector2 off2 = { game->player.position.x - 100, game->player.position.y };
                    SpawnProjectile(game, enemy->position, off1, ptype, dmg);
                    SpawnProjectile(game, enemy->position, off2, ptype, dmg);
                }
                
                enemy->state = IDLE;
                enemy->cooldownTimer = (enemy->type == 2) ? 2.5f : 1.5f; // KPC tem cooldown maior
            }
        }
        else if (enemy->isRanged && distSqrToPlayer < 400.0f * 400.0f && enemy->cooldownTimer <= 0.0f) {
            enemy->state = ATTACK;
            enemy->chargeTimer = 0.6f;
        }
        else if (distSqrToPlayer < 450.0f * 450.0f) {
            enemy->state = AGGRO;
        }
        else {
            enemy->state = IDLE;
            enemy->patrolTimer -= delta;
        }
        
        if (enemy->cooldownTimer > 0.0f && enemy->state != HURT && enemy->state != ATTACK) {
            enemy->cooldownTimer -= delta;
        }

        // Lógica do Spawner (Type 3)
        if (enemy->type == 3 && enemy->cooldownTimer <= 0.0f && enemy->state != DEATH) {
            // Tenta spawnar um Aedes (type 1)
            for (int k = 0; k < MAX_ENEMIES; k++) {
                if (!game->enemies[k].active) {
                    game->enemies[k].active = true;
                    game->enemies[k].position = enemy->position;
                    game->enemies[k].type = 1; // Aedes
                    game->enemies[k].tier = TIER_2;
                    game->enemies[k].maxHp = 20 + game->wave * 5;
                    game->enemies[k].hp = game->enemies[k].maxHp;
                    game->enemies[k].speed = 210.0f + GetRandomValue(-15, 15);
                    game->enemies[k].isRanged = true;
                    game->enemies[k].state = IDLE;
                    game->enemies[k].cooldownTimer = 2.0f;
                    game->enemiesRemaining++; // Conta para a onda
                    // Partícula de spawn
                    SpawnParticleExplosion(game, enemy->position, DARKGRAY, 10, 50.0f, 150.0f, 2.0f, 0.4f);
                    break;
                }
            }
            enemy->cooldownTimer = 6.0f; // Tempo para o próximo spawn
        }

        // Ações de Movimentação
        if (enemy->state == AGGRO)
        {
            Vector2 chaseDir = Vector2Subtract(game->player.position, enemy->position);
            float distSqrToPlayer = Vector2LengthSqr(chaseDir);
            chaseDir = Vector2Normalize(chaseDir);
            
            float currentSpeed = enemy->speed * (enemy->slowTimer > 0.0f ? 0.5f : 1.0f);

            if (!enemy->isRanged) {
                // Inimigo melee comum (Patrulha / Persegue)
                float chaseMult = (enemy->tier == TIER_2) ? 1.25f : 1.05f;
                enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, currentSpeed * chaseMult * delta));
            } else {
                // IA Específica: Aedes aegypti (Type 1) e KPC (Type 2)
                if (enemy->type == 1) { // Aedes aegypti (errático, tenta manter distância)
                    if (distSqrToPlayer > 300.0f * 300.0f) {
                        float timeFactor = GetTime() * 15.0f;
                        float angle = sinf(timeFactor) * 1.2f;
                        float nx = chaseDir.x * cosf(angle) - chaseDir.y * sinf(angle);
                        float ny = chaseDir.x * sinf(angle) + chaseDir.y * cosf(angle);
                        Vector2 zigzagDir = (Vector2){nx, ny};
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(zigzagDir, currentSpeed * delta));
                    } else if (distSqrToPlayer < 200.0f * 200.0f) {
                        // Foge se o jogador chegar muito perto
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, -currentSpeed * delta));
                    }
                } else if (enemy->type == 2) { // KPC / Superbactéria (Lento, mas persegue constantemente)
                    if (distSqrToPlayer > 250.0f * 250.0f) {
                        enemy->position = Vector2Add(enemy->position, Vector2Scale(chaseDir, currentSpeed * 0.8f * delta));
                    }
                }
            }
        }
        else if (enemy->state == IDLE && !enemy->isRanged && enemy->type != 3)
        {
            float currentSpeed = enemy->speed * (enemy->slowTimer > 0.0f ? 0.5f : 1.0f);
            Vector2 dir = Vector2Subtract(enemy->patrolTarget, enemy->position);
            if (Vector2LengthSqr(dir) > 10.0f * 10.0f)
            {
                dir = Vector2Normalize(dir);
                enemy->position = Vector2Add(enemy->position, Vector2Scale(dir, (currentSpeed * 0.4f) * delta));
            }
        }
        else if (enemy->state == IDLE)
        {
            float distSqrToTarget = Vector2DistanceSqr(enemy->position, enemy->patrolTarget);
            if (distSqrToTarget < 15.0f * 15.0f || enemy->patrolTimer <= 0.0f)
            {
                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                float radius = (float)GetRandomValue(100, 300);
                enemy->patrolTarget.x = enemy->position.x + cosf(angle) * radius;
                enemy->patrolTarget.y = enemy->position.y + sinf(angle) * radius;
                enemy->patrolTimer = (float)GetRandomValue(3, 7);
            }
            Vector2 patrolDir = Vector2Normalize(Vector2Subtract(enemy->patrolTarget, enemy->position));
            enemy->position = Vector2Add(enemy->position, Vector2Scale(patrolDir, enemy->speed * delta));
        }

        // --------------------------------------------------------------------
        // FÍSICA DE COLISÃO DESLIZANTE (SEPARAÇÃO ENTRE INIMIGOS)
        // --------------------------------------------------------------------
        float enemyRadius = (enemy->type == 2) ? 35.0f : 20.0f;
        int nearEnemies[MAX_ENEMIES];
        int nearCount = GetEnemiesInRadius(game, enemy->position, 70.0f, nearEnemies);
        for (int k = 0; k < nearCount; k++) {
            int j = nearEnemies[k];
            if (i != j && game->enemies[j].active && game->enemies[j].state != DEATH) {
                float otherRadius = (game->enemies[j].type == 2) ? 35.0f : 20.0f;
                float distSqr = Vector2DistanceSqr(enemy->position, game->enemies[j].position);
                float minDist = enemyRadius + otherRadius;
                if (distSqr < minDist * minDist && distSqr > 0.0001f) {
                    Vector2 separationDir = Vector2Subtract(enemy->position, game->enemies[j].position);
                    separationDir = Vector2Normalize(separationDir);
                    // Empurrão de separação ajustável
                    enemy->position = Vector2Add(enemy->position, Vector2Scale(separationDir, 60.0f * delta));
                }
            }
        }

        // Mantém inimigos nos limites do mapa
        if (enemy->position.x < enemyRadius) enemy->position.x = enemyRadius;
        if (enemy->position.x > MAP_WIDTH - enemyRadius) enemy->position.x = MAP_WIDTH - enemyRadius;
        if (enemy->position.y < enemyRadius) enemy->position.y = enemyRadius;
        if (enemy->position.y > MAP_HEIGHT - enemyRadius) enemy->position.y = MAP_HEIGHT - enemyRadius;

        // --------------------------------------------------------------------
        // COLISÃO: DANO NO JOGADOR
        // --------------------------------------------------------------------
        // Aumentado a pedido do usuário para que não entrem no personagem
        float colRange = (enemy->type == 2) ? 65.0f : 45.0f;
        if (distSqrToPlayer < colRange * colRange)
        {
            HandlePlayerEnemyCollision(game, enemy);
        }
    }

    // ------------------------------------------------------------------------
    // 6.5 ATUALIZA PROJÉTEIS
    // ------------------------------------------------------------------------
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (game->projectiles[i].active)
        {
            game->projectiles[i].position = Vector2Add(game->projectiles[i].position, Vector2Scale(game->projectiles[i].velocity, delta));
            game->projectiles[i].hitbox.x = game->projectiles[i].position.x - 10;
            game->projectiles[i].hitbox.y = game->projectiles[i].position.y - 10;
            
            if (game->projectiles[i].isPlayerProjectile) {
                // Diminui o tempo de vida para explosivos
                game->projectiles[i].lifeTime -= delta;
                
                // Granada
                if (game->projectiles[i].type == PROJ_PLAYER_GRENADE && game->projectiles[i].lifeTime <= 0.0f) {
                    game->projectiles[i].active = false;
                    SpawnParticleExplosion(game, game->projectiles[i].position, ORANGE, 25, 150.0f, 400.0f, 4.0f, 0.6f);
                    PlaySound(g_assets.sfxAttack); // reused as explosion
                    game->screenShake = 0.5f;
                    
                    int collIndices[MAX_ENEMIES];
                    int collCount = GetEnemiesInRadius(game, game->projectiles[i].position, 180.0f, collIndices);
                    for (int k = 0; k < collCount; k++) {
                        int eIdx = collIndices[k];
                        if (game->enemies[eIdx].active && game->enemies[eIdx].state != DEATH) {
                            game->enemies[eIdx].hp -= game->projectiles[i].damage;
                            if (game->enemies[eIdx].hp <= 0) {
                                game->enemies[eIdx].hp = 0;
                                game->enemies[eIdx].state = DEATH;
                                game->enemies[eIdx].cooldownTimer = 0.5f;
                                game->player.xp += (game->enemies[eIdx].tier + 1) * 10;
                                game->player.score += (game->enemies[eIdx].tier + 1) * 50;
                                if (game->enemiesRemaining > 0) game->enemiesRemaining--;
                            } else {
                                game->enemies[eIdx].state = HURT;
                                game->enemies[eIdx].cooldownTimer = 0.25f;
                            }
                        }
                    }
                }
                else {
                    // Colisão normal de projéteis do player (exceto granada que explode no timer)
                    if (game->projectiles[i].type != PROJ_PLAYER_GRENADE) {
                        for (int j = 0; j < MAX_ENEMIES; j++) {
                            if (game->enemies[j].active && game->enemies[j].state != DEATH) {
                                // Hitbox maior para garantir colisão
                                float eRadius = 45.0f;
                                Rectangle eRect = { game->enemies[j].position.x - eRadius, game->enemies[j].position.y - eRadius, eRadius * 2, eRadius * 2 };
                                if (CheckCollisionRecs(game->projectiles[i].hitbox, eRect)) {
                                    // BFG não é desativado no primeiro hit
                                    if (game->projectiles[i].type != PROJ_PLAYER_BFG) {
                                        game->projectiles[i].active = false;
                                    }
                                    
                                    game->enemies[j].hp -= game->projectiles[i].damage;
                                    PlaySound(g_assets.sfxEnemyHurt);
                                    SpawnParticleExplosion(game, game->enemies[j].position, RED, 10, 50.0f, 150.0f, 3.0f, 0.4f);
                                    
                                    if (game->enemies[j].hp <= 0) {
                                        game->enemies[j].hp = 0;
                                        game->enemies[j].state = DEATH;
                                        game->enemies[j].cooldownTimer = 0.5f;
                                        game->player.xp += (game->enemies[j].tier + 1) * 10;
                                        game->player.score += (game->enemies[j].tier + 1) * 50;
                                        if (game->enemiesRemaining > 0) game->enemiesRemaining--;
                                    } else {
                                        game->enemies[j].state = HURT;
                                        game->enemies[j].cooldownTimer = 0.25f;
                                    }
                                    
                                    // Se não é BFG, sai do loop (1 hit por projétil)
                                    if (game->projectiles[i].type != PROJ_PLAYER_BFG) break;
                                }
                            }
                        }
                    }
                }
            } else {
                // Colisão com jogador (Inimigo atirou)
                Rectangle pRect = { game->player.position.x - 22, game->player.position.y - 22, 44, 44 };
                if (CheckCollisionRecs(game->projectiles[i].hitbox, pRect))
                {
                    HandleProjectileCollision(game, &game->projectiles[i]);
                }
            }
            
            // BFG: expira pelo lifetime
            if (game->projectiles[i].type == PROJ_PLAYER_BFG && game->projectiles[i].lifeTime <= 0.0f) {
                game->projectiles[i].active = false;
                SpawnParticleExplosion(game, game->projectiles[i].position, GREEN, 30, 100.0f, 300.0f, 5.0f, 0.8f);
            }
            
            // Remove se sair do mapa
            if (game->projectiles[i].active &&
                (game->projectiles[i].position.x < -200 || game->projectiles[i].position.x > MAP_WIDTH + 200 ||
                 game->projectiles[i].position.y < -200 || game->projectiles[i].position.y > MAP_HEIGHT + 200)) {
                game->projectiles[i].active = false;
            }
        }
    }

    // ------------------------------------------------------------------------
    // 7. SISTEMA DE LEVEL UP DO HERÓI
    // ------------------------------------------------------------------------
    if (game->player.xp >= game->player.xpNeeded)
    {
        // Consome XP e sobe de nível
        game->player.xp -= game->player.xpNeeded;
        game->player.level++;
        game->player.xpNeeded = (int)(game->player.xpNeeded * 1.5f);

        // Melhora atributos do herói
        game->player.maxHp += 15;
        game->player.hp = game->player.maxHp; // Cura total no level up
        game->player.attackPower += 6;
        game->player.speed += 10.0f;

        // Grande efeito visual festivo de Level Up (Verde e Gold)
        SpawnParticleExplosion(game, game->player.position, LIME, 30, 80.0f, 220.0f, 5.0f, 1.0f);
        SpawnParticleExplosion(game, game->player.position, GOLD, 20, 100.0f, 250.0f, 4.0f, 1.2f);
        
        game->screenShake = 0.5f;
    }

    // ------------------------------------------------------------------------
    // 8. ATUALIZAÇÃO DA CÂMERA (SUAVE COM LERP + SHAKE)
    // ------------------------------------------------------------------------
    float targetX = game->player.position.x;
    float targetY = game->player.position.y;
    
    // Lerp da câmera em direção ao jogador
    game->camera.target.x += (targetX - game->camera.target.x) * 0.085f;
    game->camera.target.y += (targetY - game->camera.target.y) * 0.085f;

    // Aplica chacoalhar de tela na câmera se ativo
    if (game->screenShake > 0.0f)
    {
        game->camera.offset.x = (SCREEN_WIDTH / 2.0f) + (float)GetRandomValue(-15, 15) * game->screenShake;
        game->camera.offset.y = (SCREEN_HEIGHT / 2.0f) + (float)GetRandomValue(-15, 15) * game->screenShake;
    }
    else
    {
        game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    }
}

// ============================================================================
// PERSISTÊNCIA: SALVAR JOGO POR SLOT
// ============================================================================
typedef struct {
    GameState *gameSnapshot;
    int slot;
} SaveThreadData;

void SaveGameThread(void *lpParam)
{
    SaveThreadData *data = (SaveThreadData *)lpParam;
    GameState *game = data->gameSnapshot;
    int slot = data->slot;

    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "w");
    if (arquivo != NULL)
    {
        // 0. Metadados do Slot para carregamento rápido
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char dateBuffer[32];
        strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%Y %H:%M", tm_info);

        fprintf(arquivo, "%s\n", (game->player.name[0] != '\0') ? game->player.name : "HERO");
        fprintf(arquivo, "%d\n", game->player.level);
        fprintf(arquivo, "%d\n", game->player.score);
        fprintf(arquivo, "%d\n", game->wave);
        fprintf(arquivo, "%s\n", dateBuffer);

        // 1. Dados do Jogador
        fprintf(arquivo, "%f\n", game->player.position.x);
        fprintf(arquivo, "%f\n", game->player.position.y);
        fprintf(arquivo, "%d\n", game->player.hp);
        fprintf(arquivo, "%d\n", game->player.maxHp);
        fprintf(arquivo, "%d\n", game->player.score);
        fprintf(arquivo, "%d\n", game->player.level);
        fprintf(arquivo, "%d\n", game->player.xp);
        fprintf(arquivo, "%d\n", game->player.xpNeeded);
        fprintf(arquivo, "%d\n", game->player.attackPower);
        fprintf(arquivo, "%f\n", game->player.speed);

        // 2. Estado do Mundo
        fprintf(arquivo, "%d\n", game->wave);
        fprintf(arquivo, "%d\n", game->totalEnemiesKilled);
        fprintf(arquivo, "%f\n", game->timeElapsed);

        // 3. Contagem e Estado dos Inimigos
        fprintf(arquivo, "%d\n", game->enemiesRemaining);

        // Escreve cada inimigo ativo
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (game->enemies[i].active)
            {
                fprintf(arquivo, "%f %f %d %d %d %f %d %d\n",
                        game->enemies[i].position.x,
                        game->enemies[i].position.y,
                        game->enemies[i].hp,
                        game->enemies[i].maxHp,
                        game->enemies[i].type,
                        game->enemies[i].speed,
                        game->enemies[i].tier,
                        game->enemies[i].isRanged);
            }
        }

        fclose(arquivo);
    }

    // Libera a memória do snapshot e dos parâmetros da thread
    free(game);
    free(data);
    _endthread();
}

void SalvarJogoSlot(GameState *game, int slot)
{
    // Efeito visual imediato na thread principal para dar feedback ao jogador
    SpawnParticleExplosion(game, game->player.position, GREEN, 15, 30.0f, 90.0f, 4.0f, 0.6f);

    // Snapshot para o Salvamento Assíncrono
    GameState *snapshot = (GameState *)malloc(sizeof(GameState));
    if (snapshot != NULL) {
        memcpy(snapshot, game, sizeof(GameState));
        
        SaveThreadData *data = (SaveThreadData *)malloc(sizeof(SaveThreadData));
        if (data != NULL) {
            data->gameSnapshot = snapshot;
            data->slot = slot;
            
            // Inicia a thread de salvamento em background usando _beginthread (C runtime)
            _beginthread(SaveGameThread, 0, data);
        } else {
            free(snapshot);
        }
    }
}

// ============================================================================
// PERSISTÊNCIA: CARREGAR JOGO POR SLOT
// ============================================================================
void CarregarJogoSlot(GameState *game, int slot)
{
    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "r");
    if (arquivo != NULL)
    {
        // Reseta primeiro para evitar lixo nas partículas e power-ups
        float shakeOld = game->screenShake;
        GameScreen oldScreen = game->currentScreen;
        float tempVol = game->masterVolume;

        // Limpa estados de buffs temporários
        memset(game, 0, sizeof(GameState));

        game->currentScreen = oldScreen;
        game->screenShake = shakeOld;
        game->masterVolume = tempVol;
        InitParticlePool(game);

        // Pular/Ler metadados iniciais
        char nameLine[32];
        if (fgets(nameLine, sizeof(nameLine), arquivo) != NULL)
        {
            nameLine[strcspn(nameLine, "\r\n")] = '\0';
            strncpy(game->player.name, nameLine, 15);
            game->player.name[15] = '\0';
        }
        int dummyLevel, dummyScore, dummyWave;
        char dummyDate[32];
        fscanf(arquivo, "%d\n", &dummyLevel);
        fscanf(arquivo, "%d\n", &dummyScore);
        fscanf(arquivo, "%d\n", &dummyWave);
        if (fgets(dummyDate, sizeof(dummyDate), arquivo) != NULL)
        {
            // Apenas consome a linha da data
        }

        // 1. Dados do Jogador
        fscanf(arquivo, "%f\n", &game->player.position.x);
        fscanf(arquivo, "%f\n", &game->player.position.y);
        fscanf(arquivo, "%d\n", &game->player.hp);
        fscanf(arquivo, "%d\n", &game->player.maxHp);
        fscanf(arquivo, "%d\n", &game->player.score);
        fscanf(arquivo, "%d\n", &game->player.level);
        fscanf(arquivo, "%d\n", &game->player.xp);
        fscanf(arquivo, "%d\n", &game->player.xpNeeded);
        fscanf(arquivo, "%d\n", &game->player.attackPower);
        fscanf(arquivo, "%f\n", &game->player.speed);

        // 2. Estado do Mundo
        fscanf(arquivo, "%d\n", &game->wave);
        fscanf(arquivo, "%d\n", &game->totalEnemiesKilled);
        fscanf(arquivo, "%f\n", &game->timeElapsed);

        // 3. Contagem e Inimigos
        fscanf(arquivo, "%d\n", &game->enemiesRemaining);

        for (int i = 0; i < game->enemiesRemaining; i++)
        {
            if (i < MAX_ENEMIES)
            {
                int t = 0, isR = 0;
                fscanf(arquivo, "%f %f %d %d %d %f %d %d\n",
                        &game->enemies[i].position.x,
                        &game->enemies[i].position.y,
                        &game->enemies[i].hp,
                        &game->enemies[i].maxHp,
                        &game->enemies[i].type,
                        &game->enemies[i].speed,
                        &t,
                        &isR);
                game->enemies[i].tier = (EnemyTier)t;
                game->enemies[i].isRanged = (bool)isR;
                
                game->enemies[i].active = true;
                game->enemies[i].state = IDLE;
                game->enemies[i].patrolTarget = game->enemies[i].position;
                game->enemies[i].patrolTimer = 3.0f;
            }
        }

        // Câmera re-alinhada instantaneamente
        game->camera.target = game->player.position;
        game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        game->camera.zoom = 1.0f;
        game->camera.rotation = 0.0f;

        game->saveLoaded = true;
        fclose(arquivo);

        // Efeito visual de carregamento completo (Partículas ciano)
        SpawnParticleExplosion(game, game->player.position, SKYBLUE, 20, 50.0f, 150.0f, 4.0f, 0.7f);
    }
}

// ============================================================================
// PERSISTÊNCIA: CARREGAR METADADOS DE UM SLOT
// ============================================================================
SaveSlotMeta CarregarMetadadosSlot(int slot)
{
    SaveSlotMeta meta = { 0 };
    char path[64];
    sprintf(path, "Saves/save_slot_%d.txt", slot);

    FILE *arquivo = fopen(path, "r");
    if (arquivo != NULL)
    {
        meta.exists = true;
        
        // Linha 1: Nome do Jogador
        if (fgets(meta.name, sizeof(meta.name), arquivo) != NULL)
        {
            meta.name[strcspn(meta.name, "\r\n")] = '\0';
        }
        
        // Linha 2: Nível
        fscanf(arquivo, "%d\n", &meta.level);
        
        // Linha 3: Score
        fscanf(arquivo, "%d\n", &meta.score);
        
        // Linha 4: Wave
        fscanf(arquivo, "%d\n", &meta.wave);
        
        // Linha 5: Data
        if (fgets(meta.date, sizeof(meta.date), arquivo) != NULL)
        {
            meta.date[strcspn(meta.date, "\r\n")] = '\0';
        }
        
        fclose(arquivo);
    }
    else
    {
        meta.exists = false;
    }
    return meta;
}

// ===========================================================// Estruturas e variáveis globais para a thread de loading
static volatile bool g_isBackgroundLoading = false;
static GameState *g_tempLoadState = NULL;

typedef struct LoadThreadData {
    GameState *tempState;
    int slot;
} LoadThreadData;

void BackgroundLoadThread(void *arg) {
    LoadThreadData *data = (LoadThreadData *)arg;
    CarregarJogoSlot(data->tempState, data->slot);
    
    // Simula um carregamento de assets pesado
    int waitLoops = GetRandomValue(5, 10);
    for (int i = 0; i < waitLoops; i++) {
        // Simula delay de carregamento de texturas pesadas / leitura de disco
        WaitTime(0.1);  
    }

    g_isBackgroundLoading = false;
    free(data);
    _endthread();
}

// ============================================================================
// TELA DE CARREGAMENTO (LOADING SCREEN)
// ============================================================================
void RequestLoadingScreen(GameState *game, LoadTarget target, float duration)
{
    // Descarrega o mapa e entidades anteriores para otimização
    for (int i = 0; i < MAX_ENEMIES; i++) game->enemies[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) game->powerUps[i].active = false;
    InitParticlePool(game);
    for (int i = 0; i < MAX_PROJECTILES; i++) game->projectiles[i].active = false;

    // Se estivermos saindo do tutorial, reseta inTutorial
    if (target == LOAD_TO_GAMEPLAY)
    {
        game->inTutorial = false;
    }

    game->currentScreen = SCREEN_LOADING;
    game->loadTarget = target;
    game->loadingTimer = 0.0f;
    game->loadingDuration = duration;
    game->loadingTip = GetRandomValue(0, 4);

    if (game->loadSlot > 0) {
        g_isBackgroundLoading = true;
        g_tempLoadState = (GameState *)malloc(sizeof(GameState));
        if (g_tempLoadState) {
            memcpy(g_tempLoadState, game, sizeof(GameState));

            LoadThreadData *data = (LoadThreadData *)malloc(sizeof(LoadThreadData));
            if (data) {
                data->tempState = g_tempLoadState;
                data->slot = game->loadSlot;
                _beginthread(BackgroundLoadThread, 0, data);
            } else {
                free(g_tempLoadState);
                g_isBackgroundLoading = false;
            }
        } else {
            g_isBackgroundLoading = false;
        }
    } else {
        g_isBackgroundLoading = false;
    }
}

void UpdateTelaLoading(GameState *game, float delta)
{
    game->loadingTimer += delta;

    // Emite algumas partículas decorativas biológicas/de rede na tela de carregamento
    if (GetRandomValue(0, 15) == 0)
    {
        Vector2 pos = { (float)GetRandomValue(0, SCREEN_WIDTH), SCREEN_HEIGHT + 10.0f };
        Vector2 vel = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(-50, -20) };
        Color col = (GetRandomValue(0, 1) == 0) ? (Color){ 0, 229, 255, 255 } : (Color){ 0, 200, 100, 255 };
        SpawnParticle(game, pos, vel, col, (float)GetRandomValue(2, 4), (float)GetRandomValue(2, 4));
    }

    // Atualiza partículas decorativas da tela de loading
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].active)
        {
            game->particles[i].position.y += game->particles[i].velocity.y * delta;
            game->particles[i].position.x += game->particles[i].velocity.x * delta;
            game->particles[i].lifeTime -= delta;
            if (game->particles[i].lifeTime <= 0.0f) FreeParticle(game, i);
        }
    }

    if (game->loadingTimer >= game->loadingDuration && !g_isBackgroundLoading)
    {
        // Se houver slot de save especificado para carregar, carrega-o agora
        if (game->loadSlot > 0)
        {
            game->loadSlot = 0; // reseta
            if (g_tempLoadState) {
                memcpy(game, g_tempLoadState, sizeof(GameState));
                free(g_tempLoadState);
                g_tempLoadState = NULL;
            }
            game->currentScreen = SCREEN_GAMEPLAY;
            game->saveLoaded = true;
            strcpy(game->notificationMsg, "GAME LOADED!");
            game->timeElapsed = 0.0f;
            return;
        }

        // Caso contrário, faz o carregamento padrão do destino
        switch (game->loadTarget)
        {
            case LOAD_TO_TUTORIAL:
                InitTutorial(game);
                break;

            case LOAD_TO_GAMEPLAY:
                game->inTutorial = false;
                game->currentScreen = SCREEN_GAMEPLAY;
                // Posiciona jogador no centro do organismo
                game->player.position = (Vector2){ MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f };
                // Reinicia a câmera
                game->camera.target = game->player.position;
                game->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                game->camera.zoom = 1.0f;
                // Inicia wave
                StartNextWave(game);
                break;

            case LOAD_TO_MENU:
                game->currentScreen = SCREEN_MENU;
                break;

            case LOAD_TO_GAMEOVER:
                game->currentScreen = SCREEN_GAMEOVER;
                break;

            case LOAD_TO_VICTORY:
                game->currentScreen = SCREEN_VICTORY;
                break;
        }
    }
}

// ============================================================================
// TEXTOS DE DIÁLOGO DO TUTORIAL (PAGINADOS)
// ============================================================================
void GetTutorialDialogText(int step, int page, const char **line1, const char **line2, const char **line3)
{
    *line1 = "";
    *line2 = "";
    *line3 = "";
    if (step == 0)
    {
        if (page == 0)
        {
            *line1 = "Voce e um Anticorpo convocado para salvar o organismo de um humano infectado no DF.";
            *line2 = "Sua missao e combater patogenos e ensinar a populacao a prevenir doencas.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Voce foi gerado dentro de uma Seringa de Vacina, pronto para ser injetado.";
            *line2 = "Primeiro, vamos calibrar os biossensores e absorver doses iniciais de vacina.";
            *line3 = "Use WASD/Setas para se mover e colete as 3 ampolas de vacina no cenario.";
        }
    }
    else if (step == 1)
    {
        if (page == 0)
        {
            *line1 = "Alerta! Uma bacteria enfraquecida (vacina atenuada) foi inserida para treino!";
            *line2 = "Seu corpo precisa aprender a reconhecer e destruir essa ameaca.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Aproxime-se e use o BOTAO ESQUERDO DO MOUSE ou ESPACO para atacar.";
            *line2 = "Cuidado: ela persegue voce e dispara toxinas acidas. Esquive-se!";
            *line3 = "Ao ser eliminada, ela deixara uma cura. Colete-a para abrir o bocal!";
        }
    }
    else if (step == 2)
    {
        if (page == 0)
        {
            *line1 = "Excelente! Seu sistema imunologico registrou a assinatura desse patogeno.";
            *line2 = "Agora voce esta pronto para o combate real no corpo do paciente.";
            *line3 = "[Aperte 'Q' ou 'ESPACO' para continuar...]";
        }
        else
        {
            *line1 = "Va ate a ponta inferior da seringa (o bocal da agulha que esta piscando em verde).";
            *line2 = "Ao entrar no bocal, voce sera injetado na corrente sanguinea!";
            *line3 = "Siga em frente e salve o Distrito Federal!";
        }
    }
}

