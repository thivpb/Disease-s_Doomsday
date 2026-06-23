# 4.3 Regras de Negócio

> Regras derivadas diretamente do código; definem o comportamento esperado do sistema em situações específicas.

---

## Progressão de Ondas

| Regra | Detalhe |
| ----- | ------- |
| RN01 | Onda 5 de qualquer mundo é sempre a onda de chefe |
| RN02 | Inimigos comuns por onda: `8 + wave × 6`, máximo 60 |
| RN03 | Onda de chefe: 1 boss + 12 minions em formação circular |
| RN04 | Ondas 1–4: inimigos comuns + 1 miniboss + 1 a 3 escoltas do miniboss (`2 + wave/2`) |
| RN05 | Power-ups gerados por onda: `4 + wave`, máximo 10 |
| RN06 | Composição do Mundo 2 é determinística por onda (não aleatória): Onda 1=Swarm, 2=+Melee, 3=+Ranged, 4=+Elite |

## Sistema de Combate

| Regra | Detalhe |
| ----- | ------- |
| RN07 | Dano do inimigo ao jogador: `(8 + wave×2 + bônus_tier) × dificuldade_mul` |
| RN08 | Máscara Hospitalar reduz todo dano recebido em 40% (multiplicador 0,6) |
| RN09 | I-frames do jogador: 0,5 s após golpe corpo-a-corpo; 0,35 s após projétil |
| RN10 | Escudo (power-up) absorve todo o dano antes do HP; próprios I-frames: 0,3 s corpo, 0,2 s projétil |
| RN11 | Knockback por tier: inimigo comum = 55 px, miniboss = 22 px, boss = 8 px |
| RN12 | Attack Boost duplica todo o dano do jogador enquanto ativo |
| RN13 | Dano ao capsídeo viral: vai ao escudo primeiro; HP do inimigo só reduz quando `shieldHp ≤ 0` |
| RN14 | Lâmina Bioelétrica aplica multiplicador 3× somente sobre o escudo de capsídeo |
| RN15 | Granada (RNA) causa AoE + veneno e ignora capsídeo por design |
| RN16 | Rifle muda de tipo entre mundos: Bacteriófago (+60% vs bactérias) no Mundo 1; Vacina (+60% vs vírus) no Mundo 2 |
| RN17 | Boss Coronavírus é invulnerável enquanto pelo menos 1 Núcleo de Infecção estiver ativo |
| RN18 | Núcleos de Infecção só são atingidos pela AoE corpo-a-corpo (raio 140 px) |

## Desbloqueáveis

| Regra | Detalhe |
| ----- | ------- |
| RN19 | Lâmina Bioelétrica (arma 5) desbloqueia automaticamente ao atingir 30 kills totais |
| RN20 | Anúncio de desbloqueio é exibido uma única vez por campanha (`bioBladeAnnounced`) |
| RN21 | Armas 1–4 disponíveis desde o início; arma 5 exige progressão |

## Escalamento por Dificuldade

| Atributo | Fácil | Médio | Difícil |
| -------- | ----- | ----- | ------- |
| HP inimigo | ~0,7× | 1,0× | ~1,4× |
| Dano inimigo | ~0,6× | 1,0× | ~1,3× |
| Velocidade | ~0,8× | 1,0× | ~1,2× |
| Chance de desvio | reduzida | 15% | aumentada |
| Memória de aggro | menor | 5 s | maior |
| Cadência de invocação do boss | mais lenta | 6 s | mais rápida |

## Sistema de Quiz

| Regra | Detalhe |
| ----- | ------- |
| RN22 | Perguntas embaralhadas por Fisher-Yates a cada exibição |
| RN23 | Sistema evita repetir qualquer uma das últimas 14 perguntas da sessão |
| RN24 | Resposta bloqueada após primeira seleção (sem segunda chance) |
| RN25 | Explicação educativa sempre exibida após resposta (correta ou não) |

## Save / Load

| Regra | Detalhe |
| ----- | ------- |
| RN26 | Arquivo `config.txt` persiste: volumes, skin, weapon skin e dificuldade |
| RN27 | Save de slot persiste: posição, todos os atributos do jogador, onda atual, inimigos vivos |
| RN28 | Slot sem dados exibe indicador "vazio" na tela de seleção |
| RN29 | Retrocompatibilidade garantida por header de versão (`AUDIO2`); campos de padding reservados para futuras expansões |

## Cosmetics & Skins

| Regra | Detalhe |
| ----- | ------- |
| RN30 | Jogador possui 3 skins de personagem (Default, Doutor, Infectado) e 3 skins de arma (Default, Plasma, Tóxico) |
| RN31 | 8 slots de cosméticos independentes: capacete, rosto, peito, braços, pernas, botas, costas, efeito |
| RN32 | Skins e cosméticos são cosméticos puros — não alteram atributos de combate |
