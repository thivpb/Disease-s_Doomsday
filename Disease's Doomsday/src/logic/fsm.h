#ifndef FSM_H
#define FSM_H

#include "../../include/game.h"

// Atualiza a lógica do estado atual
void UpdateStateMachine(GameState *game);

// Desenha a HUD/Interface do estado atual
void DrawStateMachine(GameState *game);

#endif // FSM_H
