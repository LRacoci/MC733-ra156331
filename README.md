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

Separando o arquivo e usando a otimização ```-mtune=native```, obteve-se _5.294_ s, ou seja, o tempo piorou possivelmente porque fica mais dificil otimizar os arquivos separados, mas considerando um erro de 5% por fatores aleatórios pode-se considerar que não houve diferença significativa.

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
Rodando com a otimização ```-mtune=native```:
```bash
time ./main 1000000
```
... obtem-se que "Há 78498 primos até 1000000" em um tempo real de _1_ min e _31.854_ seg. Rodando o mesmo comando com os dois códigos juntos, obtem-se _1_ min e _32.326_ s, ou seja, a performance parece ter piorado neste caso, apesar de a diferença ser pequena. Uma possível explicação seria o fato de este código exigir muito mais otimização na função main, a ponto de qualquer ganho que poderia ter acontecido na otimização ao se juntar os dois codigos ser suplantado pelo custo de otimizar o loop na main.

Ao otimizar a função primo como pedido no enunciado do exercício:
```c
int primo(int n)
{
	int i;
	if(2 < n && n % 2 == 0) 
		return 0;
	for(i = 3; i < n; i += 2)
		if (n % i == 0) 
			return 0;

	return 1;
}
```	
Como esperado, o tempo de execução real cai para _46.211 s_, aproximadamete metade do tempo que demorava antes. Usando o gproof também obtiveram-se tempos muito próximos tanto para a versão separada do codigo quanto para a versão junta.
	
Em ambas as versões do código modificou-se a função procurou-se paralelizar os loops da função ```main```, como pode ser visto a seguir:
```c
#pragma omp parallel for reduction (+:p)
	for(i = 2; i <= n; i++){
		if(primo(i)){
			p++;		
		}
	}
```
Obtendo-se 22.131s e 22.332s para as versões separadas e juntas respectivamente.















