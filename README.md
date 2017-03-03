# Relatório

## Exercício 1

### Anotações (Rascunho):


Rodando apenas os comandos, sem flags:
```bash
gcc primo.c -o primo
time ./primo
```
obtem-se como tempos reais de _250_ a _360_ ms, ou seja, há uma grande variação por fatores aleatórios. Para obter melhor precisão decidiu-se aumentar o inteiro _n_ no código ```primo.c```. Para isso usou-se o INT_MAX da biblioteca ```limits.h```, que é primo

Agora, rodando novamente os mesmos comandos obtem-se um tempo real de _5.263_ s.

Usando agora as flags -O0, -O1, O2 e -O3 obtem-se, respectivamente, _5.376_ s, _5.273_ s, _5.952_ s, _5.931_ s.

Usando a flag ```-mtune=native``` obteve-se _5.266_ s

Separando o arquivo e usando a otimização ```-mtune=native```, obteve-se _5.294_s, ou seja, o tempo piorou possivelmente porque fica mais dificil otimizar os arquivos separados.

Modificando a função main para que conte quantos primos ha até n temos:

```c
int main(int argc, char* argv[])
{
  int n = atoi(argv[1]);
  int p = 0;
  int i;

  if(argc != 2){
  	return 1;
  }

  for(i = 2; i <= n; i++){
  	if(primo(i)){
  		p++;		
  	}
  }

  if(p > 0){
  	printf("Há %d primos até %d\n",p, n);
  }

  if (primo(n) && n > 1)
    printf("%d é primo.\n", n);
  else
    printf("%d não é primo.\n", n);
}
```
Rodando:
```bash
time ./main 1000000
```
... obtem-se que "Há 78498 primos até 1000000" em um tempo real de _1_min e _31.854_seg







