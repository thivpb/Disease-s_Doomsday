# 8.3 Definition of Done

Um item é considerado **concluído** quando:

1. Compila sem novos warnings em GCC (Windows e Linux)
2. Roda a 60 FPS sem crashes no caminho principal
3. Save/Load funciona com retrocompatibilidade (header `AUDIO2` mantido)
4. Conteúdo educativo revisado pela equipe — referência a fonte (OMS, MS, SES-DF)
5. Teste automatizado cobrindo o comportamento critico, se aplicável
6. Documentado no `README.md` do repositório

---

## Critérios Técnicos por Área

| Área | Critério específico |
| ---- | ------------------- |
| Novo inimigo | `EnemyArchetypeFor()` retorna corretamente; HP/escudo escalam por onda; teste em `test_virus_waves.c` |
| Nova arma | Dano, cooldown, alcance e projétil testados; skin aplica cores corretas |
| Nova tela | FSM em `fsm.c` tem todos os caminhos de entrada e saída; `screenAnim` reseta |
| Novo power-up | Timer correto na struct Player; efeito visual ativo enquanto timer > 0 |
| Nova pergunta de quiz | Resposta correta (`correctOption` 0–3), opções embaralhadas, explicação obrigatória |
| Alteração no mapa | Máscara de colisão rebake via `bake_collision_mask.py`; `test_map_body.c` passa |
| Alteração no save | Header de versão atualizado ou campo de padding consumido; retrocompatibilidade testada |
