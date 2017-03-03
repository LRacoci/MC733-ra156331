# Relatório

## Exercício 1

###Anotações (Rascunho):


Rodando apenas os comandos, sem flags:
```bash
gcc primo.c -o primo
time ./primo
```
obtem-se como tempos reais de _250_ a _360_ ms, ou seja, há uma grande variação por fatores aleatórios. Para obter melhor precisão decidiu-se aumentar o inteiro _n_ no código ```primo.c```. Para isso usou-se o INT_MAX da biblioteca ```limits.h```, que é primo

Agora, rodando novamente os mesmos comandos obtem-se um tempo real de _5.263_ s.

Usando agora as flags -O0, -O1, O2 e -O3 obtem-se, respectivamente, _5.376_ s, _5.273_ s, _5.952_ s, _5.931_ s.

Usando a flag ```-mtune=native``` obteve-se _5.266_ s

