# Trabalho 2

## Grupo:

|           Nome           |   RA   |
|:------------------------:|:------:|
|    Lucas Alves Racoci    | 156331 |
|        Rafael Zane       | 157079 |
| Luiz Fernando da Fonseca | 156475 |

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
- Preditores always taken e always not taken.

### Superescalar Dual-Issue
- Alguns stalls deverão ser adicionados quando instruções simultâneas usarem e gravarem no mesmo registrador.
- Alguns dos forwardings já descritos também ocorrerão.

### Cache
- Sem Cache
- L1: Variando os parâmetros: Tamanho da cache, tamanho do bloco e associatividade.

## Programas
- rijndael coder large
- sha small
- basic math small

## Eventos

- Total de instruções
- Cache misses da L1
- Contar hazard de dados de instruções aritméticas
- Contar hazard de dados de instruções de load
- Contar hazard de dados para branches que usam resultados anteriores
- Número de ciclos devidos a stalls no branch

## Configurações

- Pipeline de 5 estágios simples, variando L1 para encontrar uma melhor configuração, preditor always not taken.
- Pipeline de 5 estágios simples, melhor configuração de L1, preditor always taken.
- Pipeline de 5 estágios simples, melhor configuração de L1, preditor de 2 bits.
- Pipeline de 7 estágios simples, melhor configuração de L1, preditor always not taken.
- Pipeline de 7 estágios simples, melhor configuração de L1, preditor always taken.
- Pipeline de 7 estágios simples, melhor configuração de L1, preditor de 2 bits.
- Pipeline de 13 estágios simples, melhor configuração de L1, preditor always not taken.
- Pipeline de 5 estágios superescalar dual-issue, melhor configuração de L1, preditor always not taken.
- Pipeline de 5 estágios superescalar dual-issue, melhor configuração de L1, preditor always taken.
- Pipeline de 5 estágios superescalar dual-issue, melhor configuração de L1, preditor de 2 bits.

## Para rodar os testes
  $ cd d4-7
  $ ./configure
  $ make
  $ cd ..
  $ acsim mips.ac -abi -s
  $ vim Makefile
Adicionar "d4-t/libd4.a" no final da linha que começa com LIBS
  $ make
  $ ./mips.x --load=susan input_small.pgm output_small.smoothing.pgm -s
