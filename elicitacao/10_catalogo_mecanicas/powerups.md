# 10.3 Catálogo de Power-ups

> 8 tipos de power-up. Drops aparecem no mapa em quantidade `4 + onda` (máx. 10) por geração de onda.

---

## Power-ups Base (4 tipos)

| Tipo | `PowerUpType` | Conceito biológico | Efeito no jogo |
| ---- | ------------- | ------------------ | -------------- |
| Recuperação de HP | `HP_RECOVERY` | Nutrientes / glóbulos vermelhos | Restaura pontos de vida imediatamente |
| Boost de Velocidade | `SPEED_BOOST` | Adrenaline / resposta autonômica | Aumenta velocidade do anticorpo temporariamente |
| Escudo | `SHIELD` | Complemento imune / barreira de anticorpos | Absorve próximos danos antes do HP; partículas SKYBLUE ao absorver |
| Boost de Ataque | `ATTACK_BOOST` | Citocinas pró-inflamatórias / resposta imune adaptativa | Dobra todo o dano causado enquanto ativo |

---

## Power-ups Educativos (4 tipos)

| Tipo | `PowerUpType` | Conceito de saúde pública real | Efeito mecânico |
| ---- | ------------- | ------------------------------ | --------------- |
| Máscara Hospitalar | `POWERUP_MASK` | EPI obrigatório em ambientes hospitalares | Reduz todo dano recebido em **40%** (`maskTimer > 0` → multiplicador 0,6) |
| Distanciamento Social | `POWERUP_DISTANCING` | Medida de controle epidemiológico não-farmacológico | Ativa aura repulsora que afasta inimigos ao redor do jogador |
| Granada de RNA | `POWERUP_RNA_GRENADE` | Interferência de RNA (terapia gênica) | Projétil de área que bypassa capsídeo |
| Citocina | `POWERUP_CYTOKINE` | Citocinas anti-inflamatórias / imunomodulação | Regeneração contínua de HP (`regenTimer > 0`) |

---

## Mecânica de Geração

- Quantidade por onda: `4 + onda_atual` (máximo 10)
- Posição: spawn dentro do mapa do corpo humano com margem de 30 px das bordas
- Spawn após início de cada onda junto dos inimigos

---

## Timers Ativos (campo da struct Player)

| Campo | Comportamento |
| ----- | ------------- |
| `speedTimer` | > 0 → velocidade aumentada |
| `shieldTimer` | > 0 → escudo ativo; absorve dano em vez do HP |
| `attackBoostTimer` | > 0 → dano × 2 em todas as armas |
| `maskTimer` | > 0 → dano recebido × 0,6 |
| `distancingTimer` | > 0 → aura repulsora ativa |
| `regenTimer` | > 0 → regen por `regenAccum` (acumulador fracional para HP inteiro) |
| `poisonTimer` | > 0 → dano de veneno por tick |
| `slowTimer` | > 0 → velocidade reduzida (debuff de inimigos) |

---

## Relação com Conceitos do SUS

| Power-up | Campanha/Programa do SUS relacionado |
| -------- | ------------------------------------ |
| Máscara Hospitalar | PCIH (Prevenção e Controle de Infecções Hospitalares) / ANVISA |
| Distanciamento Social | Plano Nacional de Contingência para Emergências em Saúde Pública |
| Citocina | Pesquisa em imunobiológicos — Instituto Butantan / Fiocruz |
| Recuperação de HP | Atenção Básica / PSF — cuidado preventivo e recuperação |
| Escudo | Vacinação — programa NIP/PNI |
| Boost de Ataque | Vigilância Epidemiológica ativa — SINAN / CVE |
