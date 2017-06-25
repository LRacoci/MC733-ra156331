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
	//cout << "writing... addr: " <<  std::hex  << a << " data: " << d << endl;
	//uint32_t aux = ntohl(d);
	//arg = *((float*) &aux);
	uint32_t data = be32toh(d);
	float fdata = *reinterpret_cast<float*>(&data);
	if(a == FLOATING_ARG1){
		arg1 = fdata;
		cout << "Simulator arg1:" <<  arg1 << endl;
	}else
	if(a == FLOATING_ARG2){
		arg2 = fdata;
		cout << "Simulator arg2:" <<  arg2 << endl;
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
	//cout << "reading... addr: " << std::hex << a << " data: " << d << endl;
	float fdata;
    if(a == FLOATING_ADD ){
    	cout << arg1 << " + " << arg2 << " = ";
    	fdata = arg1 + arg2;
    } else
	if(a == FLOATING_SUB ){
		cout << arg1 << " - " << arg2 << " = ";
		fdata = arg1 - arg2;
	} else
	if(a == FLOATING_MULT){
		cout << arg1 << " * " << arg2 << " = ";
		fdata = arg1 * arg2;
	} else
	if(a == FLOATING_DIVI){
		cout << arg1 << " / " << arg2 << " = ";
		fdata = arg1 / arg2;
	}
	cout << fdata << '@' << a << endl;
    uint32_t data = htobe32(*reinterpret_cast<uint32_t*>(&fdata));
    d = data;
	return SUCCESS;
	
}
