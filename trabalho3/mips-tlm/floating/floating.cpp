//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include "floating.h"
#include <stdio.h>
#include <netinet/in.h>
#include <math.h>

#define FLOATING_ARGS FLOATING_BASE
#define FLOATING_ANSWER FLOATING_ARGS + 2*8U

#define ARG(z) ( atan((z).re/(z).im))
#define ABS(z) ( sqrt((z).re*(z).re + (z).im*(z).im ) )
//////////////////////////////////////////////////////////////////////////////


/// Constructor
ac_tlm_floating::ac_tlm_floating( sc_module_name module_name , int k) :
	sc_module( module_name ),
	target_export("iport")
{
	/// Binds target_export to the memory
	target_export( *this );

}

/// Destructor
ac_tlm_floating::~ac_tlm_floating() {

}

/** Internal Write
	* Note: Always write 32 bits
	* @param a is the address to write
	* @param d id the data being write
	* @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status ac_tlm_floating::writem( const uint32_t &a , const uint32_t &d )
{
	#ifdef DEBUG
	//cout << "writing... addr: " <<  std::hex  << a << " data: " << d << endl;
	#endif
	//uint32_t aux = ntohl(d);
	//arg = *((float*) &aux);
	uint32_t delta = a - FLOATING_BASE;
	int proc_id = delta/(FLOATING_SIZE/NUM_PROCS);
	uint32_t addr = FLOATING_BASE + delta%(FLOATING_SIZE/NUM_PROCS);
	uint32_t data = be32toh(d);
	float fdata = *reinterpret_cast<float*>(&data);


	#ifdef DEBUG
	cout << delta << " mod " << (FLOATING_SIZE/NUM_PROCS) << " = " << delta%(FLOATING_SIZE/NUM_PROCS) << endl;
	#endif
	if(addr == FLOATING_ARG1){
		#ifdef DEBUG
		cout << "Simulator arg1[" << proc_id << "]:= " <<  arg1[proc_id] << endl;
		#endif
		arg1[proc_id] = fdata;
	}else
	if(addr == FLOATING_ARG2){
		#ifdef DEBUG
		cout << "Simulator arg2[" << proc_id << "]:= " <<  arg2[proc_id] << endl;
		#endif
		arg2[proc_id] = fdata;
	}else
	return SUCCESS;
}

/** Internal Read
	* Note: Always read 32 bits
	* @param a is the address to read
	* @param d id the data that will be read
	* @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status ac_tlm_floating::readm( const uint32_t &a , uint32_t &d )
{
	#ifdef DEBUG
	//cout << "reading... addr: " << std::hex << a << " data: " << d << endl;
	#endif
	uint32_t delta = a - FLOATING_BASE;
	int proc_id = delta/(FLOATING_SIZE/NUM_PROCS);
	uint32_t addr = FLOATING_BASE + delta%(FLOATING_SIZE/NUM_PROCS);
	float fdata;
    if(addr == FLOATING_ADD ){
    	#ifdef DEBUG
    	cout << arg1[proc_id] << " + " << arg2[proc_id] << " = ";
    	#endif
    	fdata = arg1[proc_id] + arg2[proc_id];
    } else
	if(addr == FLOATING_SUB ){
		#ifdef DEBUG
		cout << arg1[proc_id] << " - " << arg2[proc_id] << " = ";
		#endif
		fdata = arg1[proc_id] - arg2[proc_id];
	} else
	if(addr == FLOATING_MULT){
		#ifdef DEBUG
		cout << arg1[proc_id] << " * " << arg2[proc_id] << " = ";
		#endif
		fdata = arg1[proc_id] * arg2[proc_id];
	} else
	if(addr == FLOATING_DIVI){
		#ifdef DEBUG
		cout << arg1[proc_id] << " / " << arg2[proc_id] << " = ";
		#endif
		fdata = arg1[proc_id] / arg2[proc_id];
	} else
	if(addr == FLOATING_LOG2){
		#ifdef DEBUG
		cout << "log2(" << arg1[proc_id]  << ") = ";
		#endif
		fdata = log2(arg1[proc_id]);
	} else
	if(addr == FLOATING_SQRT){
		#ifdef DEBUG
		cout << "sqrt(" << arg1[proc_id] << ") = ";
		#endif
		fdata = sqrt(arg1[proc_id]);
	}
	#ifdef DEBUG
	cout << fdata << " @ " << a << endl;
	#endif
    uint32_t data = htobe32(*reinterpret_cast<uint32_t*>(&fdata));
    d = data;
	return SUCCESS;

}
