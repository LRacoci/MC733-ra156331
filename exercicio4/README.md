# mips-tlm,

Primeiramente, foram trocados os valores padrão de memória de 512MB para 64MB modificando os seqguintes arquivos

No arquivo mips/mips_parms.H
static const unsigned int AC_RAMSIZE = 67108864U; //536870912U; 	 //!< Architecture RAM size in bytes (storage DM).
static const unsigned int AC_RAM_END = 67108864U; //536870912U; 	 //!< Architecture end of RAM (storage DM).


No arquivo mem/memory.h
  ac_tlm_mem( sc_module_name module_name , int k = 67108864U); // 536870912U );


No arquivo bus/bus.cpp
// Deifnie limite da memória
#define MEMSIZE 67108864U //536870912U

A seguir foram feitas as seguintes modificações no arquivo peripheral/peripheral.h para armazenar e manipular os valores lidos/escritos


...
class ac_tlm_peripheral :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
public:
  /// Variavel Criada
  int ultimo_valor_escrito;
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export;
  /// Internal write
  ...
};
...



Na aplicação temos que ter o cuidado de colocar o prefixo volatile antes da declaração do ponteiro para garantir que o valor sempre será lido da memória.



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