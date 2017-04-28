# Trabalho 2

## Características Avaliadas

### Pipeline

#### 5 Estágios
- Hazard de Dados:
  - Forwarding:
    - Instruções que usam registradores atualizados nas 2 anteriores.
    - Instruções de Load (também têm forwarding além do stall).
  - Usa o dado sendo carregado da memória:
    - Load seguido de instrução que não é de controle: 1 stall.
    - Load seguido de branch: 2 stall.
    - Load seguido de qualquer coisa seguido de branch: 1 stall.
  - Se uma instrução, exceto Load, fizer um cálculo e for seguida de branch que depende do cálculo: 1 stall.
- Hazard de Controle:
  - Branch not taken: Pode ou não ter stall.
  - Branch always taken: 1 stall.

#### 7 Estágios
No pipeline de 7 estágios, os estágios IF e MEM são separados em duas partes. Novos Hazards deverão ser considerados.

#### 13 Estágios
Também serão considerados modelos de pipelines de 13 estágios.

### Preditor de Branches
- Preditor de 2 bits: Funciona como uma máquina de 4 estados considerando estados fortes e fracos de branches tomados e não tomados.
- Preditor mais complexo.

### Superescalar Dual-Issue
- Alguns stalls deverão ser adicionados quando instruções simultâneas usarem e gravarem no mesmo registrador.
