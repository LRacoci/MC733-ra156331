**Nome**: Lucas Alves Racoci  **RA**: 156331
Exercício 3
===========
## Introdução 

Os tipos de métricas válidos para avaliação do desempenho são os que melhor se adequam ao problema, se o objetivo for estabelecer uma métrica global, a melhor é a que se aplica a maioria dos casos, o que se relaciona com a máxima:

    " Make the Common Case Fast "

Uma forma de medir o desempenho de um programa que está sendo executado dentro de um simulador é usando algum parâmetro que seja comum tanto ao simulador quanto a um ambiente de execução real. Neste caso será usado o número de instruções execultadas de cada tipo.

Exceto nos casos onde o caso mais comum é a execução em simuladores, de forma geral, o tempo de execução do simulador não é relevante para avaliar o desempenho do sistema.

O desempenho de um sistema computacional pode ser medido através de diversos critérios, no caso de uma únidade de processamento, pode-se usar critérios como:

 - Tempo de resposta
 - Tempo de Execução
 - Throughput

Neste exercício haverá uma contagem do número de instruções e, usando uma estimativa do CPI médio de cada tipo de instrução, será calculada a quantidade de ciclos executados por instrução (CPI).




## Objetivo
Este exercício tem como objetivos:
- Aprender noções básicas do ambiente de simulação ArchC.
- Relembrar conceitos sobre medida de desempenho, em especial CPI.
- Avaliar o desempenho de um processador baseado no tempo de execução de um conjunto de programas.

## Procedimento

Realizou-se o procedimento conforme indicado no [enunciado](http://www.ic.unicamp.br/~lucas/teaching/mc723/2017-1/ex3.html).



## Resultados Resultados

| Dígitos RA | Benchmark   | CPI médio |
|------------|-------------|-----------|
|      3     | susan edges |    4,33   |
|      3     |     sha     |    2,87   |
|      1     |  jpg coder  |    4,28   |

## Resultados Resultados	


## Conclusão
