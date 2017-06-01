# Exercício 4

## Procedimento
### Pré Atividade
Foram seguidas as instruções do  [enunciado](https://gitlab.com/LRacoci/MC733-ra156331/tree/master/exercicio4 "Exercício 4"), seguindo os seguintes passos:

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
