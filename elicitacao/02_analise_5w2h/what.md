# 2.1 What — O quê?

**Disease's Doomsday** é um jogo de ação 2D _top-down_ desenvolvido em C + Raylib. O jogador controla um **Anticorpo** injetado na corrente sanguínea de um paciente infectado e deve eliminar patógenos onda após onda, aprendendo sobre doenças reais, formas de transmissão e medidas de prevenção do SUS.

---

## Funcionalidades Implementadas

### Campanha

| Item | Detalhe |
| ---- | ------- |
| Mundos | 2 — Bactérias e Vírus, com transição narrativa entre eles |
| Ondas por mundo | 5 (onda 5 = sempre chefe) |
| Total de ondas | 10 ondas de combate + 2 chefes principais |
| Mapa do Mundo 1-2 | 5657 × 5657 px — mapa do corpo humano com máscara de colisão por pixel |
| Tutorial | Mapa separado (seringa, 1600 × 400 px) com 3 etapas: movimento, combate, saída |

### Inimigos

| Categoria | Tipos | Exemplos |
| --------- | ----- | -------- |
| Bactérias | Melee, Ranged, Miniboss (KPC), Boss (KPC) | Coco, Bacilo, Superbactéria KPC |
| Vírus | Swarm, Melee, Ranged, Elite, Boss | Rinovirus, Dengue, Influenza, Sarampo mutante, Coronavírus |
| Total de tipos | 12 tipos com IDs 0–11 | |

### Armas (5)

| # | Nome | Mecânica |
| - | ---- | -------- |
| 1 | Espada-Seringa | AoE 360°, raio 140 px, cooldown 0.22 s |
| 2 | Rifle contextual | Bacteriófago (Mundo 1) / Rifle de Vacina (Mundo 2); bônus +60% vs tipo correto |
| 3 | Desestabilizador de RNA | Granada: AoE + veneno; ignora capsídeo viral |
| 4 | BFG Imunológico | Projétil perfurante; acerta todos os inimigos na trajetória; cooldown 5 s |
| 5 | Lâmina Bioelétrica | Desbloqueável (30 kills); dano 3× no escudo de capsídeo viral |

### Sistema de Quiz Educativo

- 39 perguntas sobre SUS, vigilância epidemiológica, vacinação, dengue, resistência antimicrobiana e microbiologia
- Aparece entre ondas
- Embaralhamento por Fisher-Yates; histórico de 14 perguntas recentes evita repetição

### Progressão

| Recurso | Descrição |
| ------- | --------- |
| XP / Nível | Ganho por kills; sobe atributos do anticorpo |
| Pontos do SUS | Moeda de upgrades mid-run (HP máx., ataque, velocidade) |
| Score | Acumulado por toda a campanha |
| Desbloqueáveis | Lâmina Bioelétrica (30 kills), skins (3 jogador + 3 arma), cosméticos (8 slots) |

### Power-ups (8 tipos)

`HP_RECOVERY` · `SPEED_BOOST` · `SHIELD` · `ATTACK_BOOST` · `POWERUP_MASK` (dano −40%) · `POWERUP_DISTANCING` (aura repulsora) · `POWERUP_RNA_GRENADE` · `POWERUP_CYTOKINE` (regeneração)

### Save / Load

- 3 slots com metadados (nome, nível, score, onda, data)
- Quicksave F5 / Quickload F9
- Formato `.txt` com header versionado (`AUDIO2`)

### Outras telas

Arsenal · Skins · Cosméticos · Configurações (volume, skin) · Seleção de dificuldade (Fácil / Médio / Difícil) · Tela de vitória · Game Over · Transição de mundo (cutscene)
