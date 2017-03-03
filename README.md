# Relatório

## Exercício 1

###Anotações (Rascunho):


Rodando apenas os comandos, sem flags:
```bash
gcc primo.c -o primo
time ./primo
```
obtem-se como tempos reais de 250 a 360 ms, ou seja, há uma grande variação por fatores aleatórios. Para obter melhor precisão decidiu-se aumentar o inteiro n no código ```primo.c```. Para isso usou-se o INT_MAX da biblioteca ```limits.h```