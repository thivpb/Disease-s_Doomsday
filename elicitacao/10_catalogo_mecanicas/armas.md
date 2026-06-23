# 10.2 Catálogo de Armas

> 5 armas com temática biológica real. Arma 5 desbloqueável por progressão.

---

## Arma 1 — Espada-Seringa

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Seringa hipodérmica / anticorpo neutralizante |
| **Tipo** | Corpo-a-corpo AoE |
| **Cooldown** | 0,22 s |
| **Dano base** | 15 + `attackPower` do jogador |
| **Área de efeito** | 360° ao redor do jogador, raio **140 px** |
| **Knockback** | 55 px (comum), 22 px (miniboss), 8 px (boss) |
| **Screen shake** | 0,25 s |
| **Efeito visual** | Partículas radiais + `slashAnimTimer` (0,22 s, raio 140 px) |
| **Disponibilidade** | Desde o início |

---

## Arma 2 — Rifle Contextual (muda por mundo)

### Mundo 1: Rifle de Bacteriófagos

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Bacteriófago — vírus que infecta e destrói bactérias |
| **Projétil** | `PROJ_PLAYER_PHAGE` |
| **Dano base** | 8 + `attackPower` |
| **Bônus** | +60% dano vs inimigos bacterianos |
| **Velocidade** | 620 px/s |
| **Alcance máximo** | 1050 px |
| **Cooldown** | 0,28 s |

### Mundo 2: Rifle de Vacina

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Anticorpo específico para antígenos virais |
| **Projétil** | `PROJ_PLAYER_VACCINE` |
| **Dano base** | 8 + `attackPower` |
| **Bônus** | +60% dano vs inimigos virais |
| **Velocidade** | 620 px/s |
| **Alcance máximo** | 1050 px |
| **Cooldown** | 0,28 s |

---

## Arma 3 — Desestabilizador de RNA

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Interferência de RNA / inibidores de replicação viral |
| **Tipo** | Granada (projétil AoE) |
| **Projétil** | `PROJ_PLAYER_GRENADE` |
| **Cooldown** | 1,5 s |
| **Dano base** | 40 + `attackPower` |
| **Velocidade do projétil** | 350 px/s |
| **Duração de voo** | 1,2 s (explode ao expirar) |
| **Efeitos** | AoE na explosão + veneno persistente |
| **Mecânica especial** | **Bypassa o capsídeo viral** — aplica dano diretamente ao HP |
| **Com Attack Boost** | Dano × 2 |
| **Disponibilidade** | Desde o início |

---

## Arma 4 — BFG Imunológico

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Resposta imune sistêmica — mobilização total do organismo |
| **Tipo** | Canhão perfurante |
| **Projétil** | `PROJ_PLAYER_BFG` |
| **Cooldown** | 5,0 s |
| **Dano base** | 100 + `attackPower` |
| **Velocidade** | 280 px/s |
| **Duração** | 3,0 s (percorre toda a tela) |
| **Mecânica especial** | **Piercing** — atravessa e atinge todos os inimigos na trajetória |
| **Screen shake** | 0,8 s |
| **Com Attack Boost** | Dano × 2 |
| **Disponibilidade** | Desde o início |

---

## Arma 5 — Lâmina Bioelétrica *(desbloqueável)*

| Atributo | Valor |
| -------- | ----- |
| **Conceito biológico** | Potencial bioelétrico das membranas celulares / ação anti-capsídeo |
| **Tipo** | Corpo-a-corpo AoE (idêntica à Espada-Seringa em área) |
| **Cooldown** | 0,22 s |
| **Dano base** | 15 + `attackPower` |
| **Área de efeito** | 360°, raio 140 px |
| **Mecânica especial** | **3× dano ao capsídeo viral** (`shieldHp`) |
| **Dano a Núcleos** | Sim (AoE automática); sem multiplicador adicional |
| **Desbloqueio** | 30 kills totais — anúncio via banner (exibido uma única vez por campanha) |

---

## Skins de Arma

| Skin | Primary | Secondary |
| ---- | ------- | --------- |
| Default (0) | Azul-imune `{120,200,255}` | Branco-gelo `{235,245,255}` |
| Plasma (1) | Magenta `{220,70,255}` | Ciano `{0,229,255}` |
| Toxic (2) | Verde-ácido `{130,220,40}` | Lima `{200,255,90}` |

A cor secundária é usada nos **textos de dano flutuante** e nos traços visuais do projétil.

---

## Tabela Comparativa

| Arma | Dano base | Cooldown | Área | Efeito especial | Unlock |
| ---- | --------- | -------- | ---- | --------------- | ------ |
| Espada-Seringa | 15 + AP | 0,22 s | 360° 140 px | Knockback forte | Inicial |
| Rifle (Phage/Vacina) | 8 + AP | 0,28 s | Projétil 1050 px | +60% vs tipo correto | Inicial |
| Granada RNA | 40 + AP | 1,5 s | AoE + veneno | Ignora capsídeo | Inicial |
| BFG Imunológico | 100 + AP | 5,0 s | Piercing total | Acerta todos | Inicial |
| Lâmina Bioelétrica | 15 + AP | 0,22 s | 360° 140 px | 3× vs capsídeo | 30 kills |
