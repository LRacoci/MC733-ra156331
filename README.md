# Relatórios

## Exercício 1

### Anotações (Rascunho):

Rodando apenas os comandos, sem flags:
```shell
gcc primo.c -o primo
time ./primo
```
obtem-se como tempos reais de *250* a *360* ms, ou seja, há variação de *36%* por fatores aleatórios. Para obter melhor precisão decidiu-se aumentar o inteiro *n* no código ```primo.c```. Para isso usou-se o INT*MAX da biblioteca ```limits.h```, que é primo.

Agora, rodando novamente os mesmos comandos obtem-se um tempo real de *5.263* s com uma variação de *5%*.

Usando agora as flags ```-O0```, ```-O1```, ```-O2``` e ```-O3``` obtem-se, respectivamente, *5.376* s, *5.273* s, *5.952* s, *5.931* s.

Usando a flag ```-mtune=native``` é possível adequar a otimização da compilação ao hardware onde o software será rodado, fazendo isso obteve-se, para o mesmo código, *5.266* s, mas considerando uma variação de aproximadamente *5%* conclui-se que não houve uma melhora tão significativa se comparado às otimizações ```-O0``` e ```-O1```
Considerando um erro 5%, pode-se concluir que ```-O0```, ```-O1``` e ```-mtune=native``` empatam como melhores que ```-O2``` e ```-O3```.

Depois disso separou-se o arquivo em **main.c** e **calc_primo.c**, para que compilassem, foi suficiente inserir a seguinte linha no arquivo **main.c**:

```c
#include "primo.c"
```

Para compilá-los basta o sqguinte comando:

```shell
gcc main.c -o main
```

que pode ser colocado tanto em um script quanto em um Makefile, sendo que a segunda opção tem a vantagem de não recompilar os arquivos que já estiverem compilados.

Separando o arquivo e usando a otimização ```-mtune=native```, obteve-se *5.294* s, ou seja, o tempo piorou possivelmente porque fica mais dificil otimizar os arquivos separados, mas considerando um erro de 5% por fatores aleatórios pode-se considerar que não houve diferença significativa.


Modificando a função main para que conte quantos primos há até **n** temos:
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
... obtem-se que "Há 78498 primos até 1000000" em um tempo real de *1* min e *31.854* seg. Rodando o mesmo comando com os dois códigos juntos, obtem-se *1* min e *32.326* s, ou seja, a performance parece ter piorado neste caso, apesar de a diferença ser pequena. Uma possível explicação seria o fato de este código exigir muito mais otimização na função main, a ponto de qualquer ganho que poderia ter acontecido na otimização ao se juntar os dois codigos ser suplantado pelo custo de otimizar o loop na main.

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
Como esperado, o tempo de execução real cai para *46.211* s, aproximadamete metade do tempo que demorava antes. Usando o gproof também obtiveram-se tempos muito próximos tanto para a versão separada do codigo quanto para a versão junta.

Em ambas as versões do código modificou-se a função procurou-se paralelizar os loops da função ```main```, como pode ser visto a seguir:
```c
#pragma omp parallel for reduction (+:p)
	for(i = 2; i <= n; i++){
		if(primo(i)){
			p++;
		}
	}
```
Obtendo-se *22.131* s e *22.332* s para as versões separadas e juntas respectivamente.
