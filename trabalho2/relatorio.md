# Relatório Projeto 2 MC733
## Grupo:

|           Nome           |   RA   |
|:------------------------:|:------:|
|    Lucas Alves Racoci    | 156331 |
|        Rafael Zane       | 157079 |
| Luiz Fernando da Fonseca | 156475 |

## Introdução
Esse projeto visa estudar o desempenho de um processador comparando diferentes cenários em que são alteradas suas características estruturais.

## Configurações utilizadas

As diferentes configurações tem como variações o tamanho do pipeline e o preditor de branches utilizado. Para a configuração 1 abaixo, foi testado diferentes configurações da cache L1, variando o tamanho da cache, o tamanho do bloco e a associatividade, para encontrar a melhor configuração para a cache e esta  foi utilizada nas configurações abaixo de 2 a 10. Foi utilizado o Dinero IV para gerar as informações de cache durante a simulação.

- Configuração 1: Pipeline de 5 estágios simples, preditor not always taken.

- Configuração 2: Pipeline de 5 estágios simples, preditor always taken.

- Configuração 3: Pipeline de 5 estágios simples, preditor 2 bits.

- Configuração 4: Pipeline de 7 estágios simples, preditor always not taken.

- Configuração 5: Pipeline de 7 estágios simples, preditor always taken.

- Configuração 6: Pipeline de 7 estágios simples, preditor 2 bits.

- Configuração 7: Pipeline de 13 estágios simples, preditor always not taken.

- Configuração 8: Pipeline de 5 estágios superescalar dual issue, preditor not always taken.

- Configuração 9: Pipeline de 5 estágios superescalar dual issue, preditor always taken.

- Configuração 10: Pipeline de 5 estágios superescalar dual issue, preditor 2 bits.

## Eventos analisados
Para poder analisar o desempenho das diferentes configurações foram  escolhidos os seguintes eventos como parâmetros de comparação:

- Total de instruções.
- Cache misses da L1.
- Contar hazard de dados de instruções aritméticas.
- Contar hazard de dados de instruções de load.
- Contar hazard de dados para branches que usam resultados anteriores.
- Número de ciclos devidos a stalls no branch.

## Programas utilizados
Foram executados os seguintes programas para todas as configurações para que fosse possível avaliar os eventos citados:

- Rijndael Coder Large.
- Sha Small.
- Basic Math Small.

Esses programas estão presentes no Dinero IV.

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