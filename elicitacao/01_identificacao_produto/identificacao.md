# 1. Identificação do Produto

| Campo                 | Informação                                                   |
| --------------------- | ------------------------------------------------------------ |
| **Nome do Produto**   | Disease's Doomsday                                           |
| **Tipo**              | Jogo educativo digital — ação 2D _top-down_                  |
| **Linguagem**         | C (padrão C11)                                               |
| **Biblioteca gráfica**| Raylib 5.x (licença zlib, open-source)                       |
| **Plataforma-alvo**   | Windows / Linux / macOS                                      |
| **Setor**             | Educação em Saúde Pública                                    |
| **Tema**              | Vigilância Epidemiológica e prevenção de doenças infecciosas |
| **Repositório**       | Felps-26/Disease-s_Doomsday (GitHub)                         |

---

## Stack Técnica

| Camada              | Tecnologia / Decisão                                                       |
| ------------------- | -------------------------------------------------------------------------- |
| **Render**          | Raylib — OpenGL 3.3 / ES 2.0 via abstração                                 |
| **Áudio**           | Raylib + miniaudio (embutido); dual-track crossfade com fade de 12s        |
| **Shader**          | Shader GLSL customizado: distorção orgânica + vinheta biológica             |
| **Supersampling**   | Render em 2560 × 1440 (2×), downscale bilinear para 1280 × 720            |
| **Build (Windows)** | MinGW / GCC via `compilar.bat`                                              |
| **Build (Unix)**    | GCC via `Makefile` + `build.sh`                                             |
| **Save/Load**       | Arquivos `.txt` simples; header versionado (`AUDIO2`)                       |
| **Testes**          | Suite própria em C pura (sem framework externo); 5 arquivos em `tests/`    |
| **Ferramenta extra**| `bake_collision_mask.py` — gera máscara de colisão a partir do PNG do mapa |

---

## Arquitetura de Pastas

```
Disease's Doomsday/
├── src/
│   ├── entities/        player, enemy, projectiles
│   ├── systems/         combat_system, wave_manager
│   ├── logic/           fsm, update_gameplay
│   ├── render/          render_gameplay
│   ├── telas/           tela_quiz, tela_arsenal, telas
│   ├── gameplay/        input_controller
│   └── assets/          asset_manager, sprite_manager
├── include/             headers públicos
├── Assets/
│   ├── @models/         player_model, enemy_model, weapons_model
│   ├── Maps/            map_body, map_seringa (tutorial)
│   ├── Sprites/         spritesheet por entidade
│   └── SFX/             efeitos sonoros
├── Saves/               config.txt + save_slot_*.txt
└── tests/               test_*.c
```
