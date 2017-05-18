# Relatório Projeto 2 MC733
## Grupo:

|           Nome           |   RA   |
|:------------------------:|:------:|
|    Lucas Alves Racoci    | 156331 |
|        Rafael Zane       | 157079 |
| Luiz Fernando da Fonseca | 156475 |

## Introdução
Esse projeto visa estudar o desempenho de um processador comparando diferentes cenários em que são alteradas suas características estruturais. Foi utilizado o simulador _mips-2.4.0_ programado em _ArchC_, que simula um processador _MIPS_ sem considerar a existência de pipelines e hazards. Foram contabilizados os ciclos para três programas, _sha small_, _susan smoothing small_ e _dijkstra large_, para que fosse possível estimar o tempo de execução de cada um a partir de uma frequência teórica de cada configuração do processador. Características simuladas do processador foram pipelines de 5, 7 e 13 estágios simplificado, escalar e superescalar de 5 estágios, branch predictor always taken, always not taken e de 2 bits, além de utilizar o Dinero como biblioteca para estimar o efeito de diferentes caches.

## Configurações utilizadas

Abaixo estão listadas as 12 configurações que foram testadas:

- Configuração 1: Pipeline de 5 estágios simples, preditor always not taken.

- Configuração 2: Pipeline de 5 estágios simples, preditor always taken.

- Configuração 3: Pipeline de 5 estágios simples, preditor 2 bits.

- Configuração 4: Pipeline de 7 estágios simples, preditor always not taken.

- Configuração 5: Pipeline de 7 estágios simples, preditor always taken.

- Configuração 6: Pipeline de 7 estágios simples, preditor 2 bits.

- Configuração 7: Pipeline de 5 estágios superescalar dual issue, preditor always not taken.

- Configuração 8: Pipeline de 5 estágios superescalar dual issue, preditor always taken.

- Configuração 9: Pipeline de 5 estágios superescalar dual issue, preditor 2 bits.

- Configuração 10: Pipeline de 13 estágios simples, preditor always not taken.

- Configuração 11: Pipeline de 13 estágios simples, preditor always taken.

- Configuração 12: Pipeline de 13 estágios simples, preditor 2 bits.

## Eventos analisados
Para poder analisar o desempenho das diferentes configurações foram  escolhidos os seguintes eventos como parâmetros de comparação:

- Total de instruções.
- Ciclos sem considerar hazards e stalls.
- Cache misses da L1i.
- Cache misses da L1d.
- Cache misses da L2.
- Ciclos de stall da cache.
- Contar hazard de dados de instruções aritméticas.
- Contar hazard de dados de instruções de load.
- Contar hazard de dados para branches que usam resultados anteriores.
- Número de ciclos devidos a stalls no branch
- Número de ciclos devidos a stalls em jumps.

## Programas utilizados
Foram executados os seguintes programas para todas as configurações para que fosse possível avaliar os eventos citados:

- Dijkstra Large
- Sha Small
- Susan Smoothing Small

## Características do Processador

Abaixo estão listadas e explicadas as diferentes características que o processador podia possuir, considerando diferentes caches, tamanho do pipeline, tipo do pipeline e branch predictor.

### Caches

Foram utilizados dois níveis de cache, L1 (instruções e dados) e L2. Para simular diferentes configurações de cache, foi utilizado o software Dinero como biblioteca em conjunto com o simulador. Para isso, a biblioteca foi compilada e ligada através do Makefile do simulador.

Para simular operações de leitura e escrita, foram criadas as caches de maneira hierárquica, criando uma memória, ligando a L2 na memória, e depois ligando L1i e L1d na L2. Foram escritas funções _doread_ e _dowrite_ que simulavam leituras e escritas, passando o endereço e a cache como parâmetro para o Dinero cuidar da checagem de miss, considerando os diferentes parâmetros variáveis das caches: tamanho da cache, tamanho do bloco e associação. Os dois primeiros parâmetros são colocados em _log2_, pois o Dinero pega o valor e faz _2^valor_ para inicializar os parâmetros.

Os endereços que foram utilizados foram extraídos das variáveis do processador. O _PC_ para ler da cache L1i foi extraído ao chamar _ac\_behavior (instruction)_ e estava na variável _ac\_pc_. Os valores da L1d estavam em registradores nos diferentes tipos de instruções, podendo ser extraídos em cada instrução individualmente.



## Hazards estudados
Como descrito, três dos eventos analisados envolvem contagem de hazards, para isso foi estudado onde esses hazards acontecem nos pipelines de 5, 7 e 13 estágios simples e no de 5 estágios superescalar dual issue.

- Pipeline 5 estágios simples
	- Instruções aritméticas
	- Instruções de load
	- Instruções de branch
- Pipeline 7 estágios simples
	- Instruções aritméticas
	- Instruções de load
	- Instruções de branch
- Pipeline 13 estágios simples
	- Instruções aritméticas
	- Instruções de load
	- Instruções de branch
- Pipeline 5 estágios superescalar dual issue
	- Instruções aritméticas
	- Instruções de load
	- Instruções de branch