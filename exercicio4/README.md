# Exercício 4

## Introdução

### Objetivos
+ Integrar processador, memória, e periféricos através de um roteador.
+ Implementar um periférico simples.
+ Implementar e fazer uso de uma plataforma multicore.

## Procedimento
Foram seguidas as instruções do  [enunciado](https://gitlab.com/LRacoci/MC733-ra156331/tree/master/exercicio4 "Exercício 4") como detalhado a seguir:
### Pré Atividade
Obteve-se a [plataforma MIPS atualizada](http://www.ic.unicamp.br/~lucas/teaching/mc723/2017-1/mips-tlm-per.tar.gz).
Depois executou-se os seguintes comandos mostrados no enunciado:
```
tar xzf mips-tlm.tar.gz
cd mips-tlm/
```
Ao tentar rodar o comando ``` make ```, ocorre o seguinte erro:
```
    SystemC 2.3.1-Accellera --- Mar 10 2017 14:03:59
    Copyright (c) 1996-2014 by all Contributors,
    ALL RIGHTS RESERVED
sw/hello_world.mips

Error: (E549) uncaught exception: std::bad_alloc
In file: sc_except.cpp:98
Makefile:67: recipe for target 'run' failed
make: *** [run] Error 1
```
Para resolvê-lo, foi necessário diminuir a memória de *512 MB* para *64 MB*, o que foi feito modificando os seguintes arquivos (o que está comentado é como estava antes):

+ No [arquivo](mips-tlm/mips/mips_parms.H) de parâmetros:
```cpp
static const unsigned int AC_RAMSIZE = 67108864U; //536870912U; 	 //!< Architecture RAM size in bytes (storage DM).
static const unsigned int AC_RAM_END = 67108864U; //536870912U; 	 //!< Architecture end of RAM (storage DM).
```

+ No [arquivo](mips-tlm/mem/memory.h) da memória:
```cpp
ac_tlm_mem( sc_module_name module_name , int k = 67108864U); // 536870912U );
```

+ No [arquivo](mips-tlm/bus/bus.cpp) do barramento:
```cpp
// Deifnie limite da memória
#define MEMSIZE 67108864U //536870912U
```

Para entender como o roteador direciona pedidos para componentes de acordo com o seu endereço de acesso, observou-se que se o endereço for menor que ```MEMSIZE``` este é direcionado para a porta o componente de memória, mas se for maior ou igual, é direcionado ao periférico. Como pode ser visto no trecho de código a seguir do [arquivo](mips-tlm/bus/bus.cpp).

```cpp
/// This is the transport method. Everything should go through this file.
/// To connect more components, you will need to have an if/then/else or a switch
/// statement inside this method. Notice that ac_tlm_req has an address field.
ac_tlm_rsp ac_tlm_bus::transport(const ac_tlm_req &request)
{
	ac_tlm_rsp response;

	if(request.addr < MEMSIZE) {

	  response = MEM_port->transport(request);

	} else {

	  response = PERIPHERAL_port->transport(request);

	}
	return response;
}
```

### Periférico Básico

Foi criada uma aplicação que faz escrita no endereço base do periférico definido na constante mostrada a seguir, para isso foi preciso ter o cuidado de colocar o prefixo volatile antes da declaração do ponteiro para o endereço do periférico para garantir que o valor sempre será lido da memória, como mostrado a seguir:
```c
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ENDERECO_LOCK 536870912U //64M

volatile int* lock = (int*) ENDERECO_LOCK;

int main(int argc, char *argv[]){
	*lock = 42;
	printf("Meu: %d@%p\n", *lock, lock);
	exit(0); // To avoid cross-compiler exit routine
	return 0; // Never executed, just for compatibility
}

```
A seguir criou-se a variável ```ultimo_valor_escrito``` no [cabeçalho](mips-tlm/peripheral/peripheral.h) do código do periférico para armazenar e manipular os valores lidos/escritos
```cpp
...
class ac_tlm_peripheral :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
public:
  /// Variavel Criada
  uint32_t ultimo_valor_escrito;
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export;
  /// Internal write
  ...
};
...
```

Para implementar a funionalidade especificada no enunciado, modificou-se este [arquivo](mips-tlm/peripheral/peripheral.cpp) para ficar como mostrado no trecho a seguir:
```cpp
/** Internal Write
  * Note: Always write 32 bits
  * @param a is the address to write
  * @param d id the data being write
  * @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status ac_tlm_peripheral::writem( const uint32_t &a , const uint32_t &d )
{
  cout << d << '@' << std::hex << a << endl;
  ultimo_valor_escrito = d;
  return SUCCESS;
}

/** Internal Read
  * Note: Always read 32 bits
  * @param a is the address to read
  * @param d id the data that will be read
  * @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status ac_tlm_peripheral::readm( const uint32_t &a , uint32_t &d )
{
  *((uint32_t *) &d) = ultimo_valor_escrito;
  ultimo_valor_escrito = 1;
  return SUCCESS;
}

```
Testando a aplicação anteriormente mostrada:
```
$ make run

        SystemC 2.3.1-Accellera --- Apr  3 2017 02:27:47
        Copyright (c) 1996-2014 by all Contributors,
        ALL RIGHTS RESERVED
sw/hello_world.mips
ArchC: Reading ELF application file: sw/hello_world.mips

ArchC: -------------------- Starting Simulation --------------------

704643072@20000000
Meu: 42@0x20000000

ArchC: -------------------- Simulation Finished --------------------

Info: /OSCI/SystemC: Simulation stopped by user.
ArchC: Simulation statistics
    Times: 0.21 user, 0.01 system, 0.00 real
    Number of instructions executed: 9139
    Simulation speed: 43.52 K instr/s

```
Isso mostra que antes de escrever o valor 42, havia lixo naquela posição.

Outro teste feito foi modificar mais vezes:

```c
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ENDERECO_LOCK 536870912U //64M

volatile unsigned* lock = (unsigned*) ENDERECO_LOCK;

int main(int argc, char *argv[]){
	unsigned i;
	for (i = 1; i ; i <<= 4) {
		*lock = i;
		printf("Meu:\n%x@%p\n", *lock, lock);
	}

	exit(0); // To avoid cross-compiler exit routine
	return 0; // Never executed, just for compatibility
}
```
Que imprimi o seguinte na resposta:

```
$ make run

        SystemC 2.3.1-Accellera --- Apr  3 2017 02:27:47
        Copyright (c) 1996-2014 by all Contributors,
        ALL RIGHTS RESERVED
sw/hello_world.mips
ArchC: Reading ELF application file: sw/hello_world.mips

ArchC: -------------------- Starting Simulation --------------------

16777216@20000000
Meu:
1@0x20000000
10000000@20000000
Meu:
10@0x20000000
10000@20000000
Meu:
100@0x20000000
100000@20000000
Meu:
1000@0x20000000
100@20000000
Meu:
10000@0x20000000
1000@20000000
Meu:
100000@0x20000000
1@20000000
Meu:
1000000@0x20000000
10@20000000
Meu:
10000000@0x20000000

ArchC: -------------------- Simulation Finished --------------------

Info: /OSCI/SystemC: Simulation stopped by user.
ArchC: Simulation statistics
    Times: 0.18 user, 0.02 system, 0.01 real
    Number of instructions executed: 30582
    Simulation speed: 169.90 K instr/s

```

Isso mostra que a representação dos valores é diferente internamente no simulador e fora.

Outro teste, sugerido no enunciado é ler várias vezes do periférico e mostrar o que foi lido na tela, como a seguir:

```c
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ENDERECO_LOCK 536870912U //64M

volatile unsigned* lock = (unsigned*) ENDERECO_LOCK;

int main(int argc, char *argv[]){
	unsigned i, aux;
	for (i = 1; i ; i <<= 4) {
		*lock = i;
		printf("%08x --> %08x\n", *lock, *lock);
	}

	exit(0); // To avoid cross-compiler exit routine
	return 0; // Never executed, just for compatibility
}
```

```
$ make run

        SystemC 2.3.1-Accellera --- Apr  3 2017 02:27:47
        Copyright (c) 1996-2014 by all Contributors,
        ALL RIGHTS RESERVED
sw/hello_world.mips
ArchC: Reading ELF application file: sw/hello_world.mips

ArchC: -------------------- Starting Simulation --------------------

16777216@20000000
0x00000001 --> 0x01000000
10000000@20000000
0x00000010 --> 0x01000000
10000@20000000
0x00000100 --> 0x01000000
100000@20000000
0x00001000 --> 0x01000000
100@20000000
0x00010000 --> 0x01000000
1000@20000000
0x00100000 --> 0x01000000
1@20000000
0x01000000 --> 0x01000000
10@20000000
0x10000000 --> 0x01000000

ArchC: -------------------- Simulation Finished --------------------

Info: /OSCI/SystemC: Simulation stopped by user.
ArchC: Simulation statistics
    Times: 0.20 user, 0.00 system, 0.00 real
    Number of instructions executed: 33356
    Simulation speed: 166.78 K instr/s


```
Isso mostra que a implementação está fazendo exatamente o que pede a especificação, com excessão do novo valor que não é 1 como desejado, mas 0x01000000

### Plataforma Multicore


Para instanciar 2 processadores no simulador, foi feito o seguinte [neste arquivo](mips-tlm/main.cpp):
```c
int sc_main(int ac, char *av[])
{
    //!  ISA simulator
    mips mips_proc1("mips1");
    mips mips_proc2("mips2");

    //! Bus
    ac_tlm_bus bus("bus");
    // Memory
    ac_tlm_mem mem("mem");
    // Peripheral
    ac_tlm_peripheral peripheral("peripheral");

    #ifdef AC_DEBUG
    ac_trace("mips1_proc1.trace");
    ac_trace("mips1_proc2.trace");
    #endif

    mips_proc1.DM(bus.target_export);
    mips_proc2.DM(bus.target_export);

    bus.MEM_port(mem.target_export);
    bus.PERIPHERAL_port(peripheral.target_export);

    char** av1 = (char**) malloc(ac*sizeof(char**));
    memcpy (av1, av, ac*sizeof(char**));

    mips_proc1.init(ac, av1);
    mips_proc1.set_prog_args();
    cerr << endl;
    char** av2 = (char**) malloc(ac*sizeof(char**));
    memcpy (av2, av, ac*sizeof(char**));

    mips_proc2.init(ac, av2);
    mips_proc2.set_prog_args();
    cerr << endl;

    sc_start();

    mips_proc1.PrintStat();
    cerr << endl;
    mips_proc2.PrintStat();
    cerr << endl;

    #ifdef AC_STATS
    mips1_proc1.ac_sim_stats.time = sc_simulation_time();
    mips1_proc1.ac_sim_stats.print();
    mips1_proc2.ac_sim_stats.time = sc_simulation_time();
    mips1_proc2.ac_sim_stats.print();
    #endif
    #ifdef AC_DEBUG
      ac_close_trace();
    #endif

      return mips_proc1.ac_exit_status;
}
```

Como pode ser visto no método ```void mips_syscall::set_prog_args(int argc, char **argv)``` [deste arquivo](mips-tlm/mips/mips_syscall.cpp), é criada uma pilha separada para cada thread, caso contrário o comportamento de do programa em execução seria incoerente. Porém a região de dados, onde as variáveis globais são declaradas, continua compartilhada, portanto todas as variáveis globais serão declaradas como globais.

Para separar a execução de cada thread, foi usado o periférico de lock definido e implementado na etapa anterior e foram definidas as seguintes macros:

```c
// Implementa MACROS para locks globais
#define AcquireGlobalLock() {while (*lock);}
#define ReleaseGlobalLock() {*lock = 0;}

#define SEQUENTIAL(code) {\
	AcquireGlobalLock() \
	{code} \
	ReleaseGlobalLock()\
}

// Implementa MACROS para lcoks locais
#define AcquireLock(lock) {SEQUENTIAL(while(lock); lock = 1;)}

#define ReleaseLock(lock) {lock = 0;}

#define ATOMIC(lock, code) {\
	AcquireLock(lock) \
	{code} \
	ReleaseLock(lock)\
}
```
### Aplicação Implementada
A aplicação implementada calcula o histograma de uma imagem.
Para comparar o desempenho adequadamente, foram implementadas as versões [paralela](mips-tlm/sw/hist.c) e [serial](mips-tlm/sw/hist-serial.c). A diferença é que a versão serial faz tudo de uma vez sem necessidade de locks e barreiras. Já para a versão paralela foi necessário implementar algumas técnicas para lidar com a concorrência no compartilhamento de memória.

#### Implementação Serial
Esta é a versão trivial, que apenas inicializa um vetor com zeros até o número de bins necessários ao histograma e depois percorre cada pixel da imagem incrementando a posição dada pelo valor do pixel.

Para garantir que os valores de pixels estarão no intervalo de indices do vetor, foi implementada a macro SATURATE mostrada a seguir:
```c
#define SATURATE(min, x, max) ( ((x) < (min)) ? (min): (((x) > (max)) ? (max):(x)) )
```
Esta macro garante seu retorno em [min, max]. Se x > max, retorna max, se x < min retorna min, ou seja, satura x entre min e max.

Isso significa que se cada pixel da imagem era para ter valor máximo de 255, e algum pixel tiver um valor maior, este será contabilizado no histograma como no bin de 255. Simetricamente para valores negativos.

#### Implementação Paralela
Para implementar está aplicação em paralelo, percebeu-se que seria conveniente cada processador calcular primeiro um histograma parcial de parte da imagem e depois somar todos os parciais para obter o final.
Assim, implementação desta aplicação foi dividida em quatro partes:

1. Leitura da entrada e cálculo dos histogramas parciais
2. Soma dos histogramas parciais
3. Escrita do histograma final na saída padrão
4. Termina a execução

Entre cada uma dessas partes foi necessária uma barreira de sincronização pois, da forma como foram implementadas, uma parte não pode ser executada antes de a anterior ter terminado.

Todas as barreiras foram implementadas da seguinte forma:
```c
// Declaração da barreira
volatile int barreira = 0;
/* Declara
 * as
 * outras
 * variáveis
 */
int main(int argc, char *argv[]){
    /*
     * Execurta
     * Codigo
     * Antes
     * da
     * Barreira
     */

    SEQUENTIAL(
		barreira++;
	)
	// Barreira para separar as duas partes
	while (barreira < NUM_PROCS);

    /*
     * Execurta
     * Codigo
     * Depois
     * da
     * Barreira
     */
}
```

Isso garante que qualquer thread só passará do ```while``` quando a ultima thread incrementar a barreira dentro de ```SEQUENTIAL```.

##### Parte 1: Leitura da Entrada e Histogramas Parciais
Como as threads são identificadas pela ordem de chegada na primeira região crítica, podemos garantir que primeira thread lerá as primeiras linhas e as próximas que forem chegando lerão também na ordem de chegada e consequentemente na ordem de seus identificadores. Como a primeira thread que chega le as primeiras linhas, esta também ficará responsável por ler os 3 primeiros valores que representam o número de colunas, o número de linhas e o valor do maior pixel da imagem, que determina o número de bins no histograma.
Para determinar quais linhas são responsabilidade de cada thread para ler e calcular o histograma parcial, uma variável local de cada thread recebe o índice da primeira linha que é de sua responsabilidade e da primeira que não é, chamadas no código de ```my_start``` e ```my_end``` como pode ser visto no trecho a seguir, que as define em função do id dado pela ordem de chegada, do número total de linhas lido pela primeira thread e do número total de processadores que é uma constante:
```c
    my_start = my_id*h/NUM_PROCS;
	my_end = (1+my_id)*h/NUM_PROCS;
```
Quando ```h``` não é divisível pelo número de processadores, estas fórmulas garantem que a thread com ```my_id == 0``` receberá menos trabalho que alguma outra, porque ```my_start == 0``` e ```my_end == floor(h/NUM_PROC)```, então esta thread ficará responsável apenas por ```my_end - my_start == my_end == floor(h/NUM_PROC)``` linhas da matriz. Já a última tread será responsável por h/NUM_PROC + 1 linhas. Foi feito dessa forma para compensar o trabalho a mais que a primeira thread já teve para ler as primeiras linhas do cabeçalho da imagem para descobrir seus parâmetros principais.

Para calcular os histogramas parciais, cada thread se responsabiliza pelo contabilização das linhas que leu da entrada, pois usam as variáveis locais ```my_start``` e ```my_end```.

##### Parte 2: Soma dos Histogramas Parciais
A soma dos histogramas parciais é realizada de forma alternada, a primeira thread soma o primeiro bin, a segunda o segundo e assim por diante, até acabarem as threads, quando recomeça pela primeira.

Dessa forma o trabalho fica bem distribuído. O trecho do código relevante que faz isso pode ser visto a seguir:

```c
    for (i = my_id; i < d; i+=NUM_PROCS){
		hist[NUM_PROCS][i] = 0;
		for (j = 0; j < NUM_PROCS; j++){
			hist[NUM_PROCS][i] += hist[j][i];
		}
	}
```

##### Parte 3: Imprimir a Resposta
É feito apenas por uma thread. Não haveria nenhum ganho de desempenho se todas fizessem pois continuaria sendo necessário que apenas uma imprimisse por vez. Além disso, se todas fizessem, haveria risco de intercalar as saídas tornando-a ilegível.


## Comparação de Desempenho
Decidiu-se por instanciar 4 processadores ao invés de só 2, para que qualquer diferença entre a execução serial e a paralela ficasse mais evidente
Abaixo segue o final do que foi retornado nas execuções serial e paralela:
+ Execução Serial

```
Info: /OSCI/SystemC: Simulation stopped by user.
ArchC: Simulation statistics
    Times: 9.56 user, 0.19 system, 9.71 real
    Number of instructions executed: 214090853
    Simulation speed: 22394.44 K instr/s
```

+ Execução Paralela

```
ArchC: Simulation statistics
    Times: 45.21 user, 0.82 system, 46.00 real
    Number of instructions executed: 263203255
    Simulation speed: 5821.79 K instr/s
```
```
ArchC: Simulation statistics
    Times: 45.21 user, 0.82 system, 45.97 real
    Number of instructions executed: 263264160
    Simulation speed: 5823.14 K instr/s
```
```
ArchC: Simulation statistics
    Times: 45.21 user, 0.82 system, 45.94 real
    Number of instructions executed: 262938661
    Simulation speed: 5815.94 K instr/s
```
```
ArchC: Simulation statistics
    Times: 45.21 user, 0.82 system, 45.92 real
    Number of instructions executed: 263088042
    Simulation speed: 5819.24 K instr/s
```
O maior número de instruções é 263264160 do segundo processador.
Ou seja, há 21% mais instruções na versão paralela.
Isso provavelmente ocorreu, porque quando uma thread espera um lock, contiunua executando instruções (muitas instruções), e no caso desta aplicação estas esperas são necessárias para em dado instante de tempo, apenas uma thread esteja lendo a matriz da entrada padrão ou escrevendo a resposta obtida na saída padrão. O maior problema aqui é que o tempo gasto em processamento na resolução do problema é da mesma ordem de complexidade que a entrada, que deve ser lida serialmente. Isso faz com que a ordem de complexidade de leitura domine o tempo de processamento, tornando o paralelismo desvantajoso, neste caso.

Para testar esta última conjectura, implementou-se uma aplicação de quadrar uma matriz n por n em complexidade O(n^3) serial, mas com entrada O(n^2) e observou-se que para uma entrada de n = 100, obtiveram-se as seguintes saídas:

+ **Serial**

```
ArchC: Simulation statistics
    Times: 4.04 user, 0.18 system, 3.95 real
    Number of instructions executed: 86731814
    Simulation speed: 21468.27 K instr/s

```

+ **Paralelo**

```
ArchC: Simulation statistics
    Times: 12.34 user, 1.01 system, 13.11 real
    Number of instructions executed: 72617243
    Simulation speed: 5884.70 K instr/s

```
```
ArchC: Simulation statistics
    Times: 12.34 user, 1.01 system, 13.05 real
    Number of instructions executed: 72617202
    Simulation speed: 5884.70 K instr/s

```
```
ArchC: Simulation statistics
    Times: 12.34 user, 1.01 system, 12.98 real
    Number of instructions executed: 72617884
    Simulation speed: 5884.76 K instr/s

```
```
ArchC: Simulation statistics
    Times: 12.34 user, 1.01 system, 12.91 real
    Number of instructions executed: 48794275
    Simulation speed: 3954.16 K instr/s

```
O maior número de instruções é 72617884, do terceiro processador, que é 83,7% do número de instruções da execução serial. O que mostra que a relação assintótica entre o tempo gasto com processamento e com entrada/saída é importante.


# Conclusão
Nem sempre é possível melhorar o desempenho de aplicações paralelizando-as. Quando a complexidade de tempo gasto com operações seriais domina assintóticamente o tempo de processamento paralelizável e não há um mecanismo que evite desperdício de instruções como interrupções por hardware, pode ser o caso de a versão paralela ser até pior, que foi o que aconteceu com a primeira aplicação de histograma.
