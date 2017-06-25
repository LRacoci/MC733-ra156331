//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../bus/bus.h"
#include "trigonometric.h"
#include <netinet/in.h>
#include <byteswap.h>

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate ac_tlm_trigonometric from ArchC
using user::ac_tlm_trigonometric;

/// Constructor
ac_tlm_trigonometric::ac_tlm_trigonometric( sc_module_name module_name , int k ) :
	sc_module( module_name ),
	target_export("iport")
{
	/// Binds target_export to the ac_tlm_trigonometric
	target_export( *this );
}

/// Destructor
ac_tlm_trigonometric::~ac_tlm_trigonometric() {
}

/** Internal Write
 * Note: Always write 32 bits
 * @param a is the address to write
 * @param d id the data being write
 * @returns A TLM response packet with SUCCESS
 */
ac_tlm_rsp_status ac_tlm_trigonometric::writem( const uint32_t &a , const uint32_t &d )
{
	//cout << "writing... addr: " <<  std::hex  << a << " data: " << d << endl;
	//uint32_t aux = ntohl(d);
	//arg[proc_id] = *((float*) &aux);
	int proc_id = (a - TRIGONOMETRIC_BASE)/TRIGONOMETRIC_SIZE;
	uint32_t data = be32toh(d);
	float fdata = *reinterpret_cast<float*>(&data);
	arg[proc_id] = fdata;
	cout << "Simulator arg[proc_id]:" <<  arg[proc_id] << endl;
	return SUCCESS;
}

/** Internal Read
 * Note: Always read 32 bits
 * @param a is the address to read
 * @param d id the data that will be read
 * @returns A TLM response packet with SUCCESS and a modified d
 */
ac_tlm_rsp_status ac_tlm_trigonometric::readm( const uint32_t &a , uint32_t &d )
{
	//cout << "reading... addr: " << std::hex << a << " data: " << d << endl;
	int proc_id = (a - TRIGONOMETRIC_BASE)/TRIGONOMETRIC_SIZE;
	float fdata;
	// Calcula coseno,
	if (a == COS_ADD) {
		//uint32_t aux = *((uint32_t *) &arg[proc_id]);
        //d = htonl(aux);
        cout << "cos(" << arg[proc_id] << ") = ";
		fdata = cos(arg[proc_id]);
	}
	// Calcula seno
	else 
	if (a == SIN_ADD) {
		//uint32_t aux = *((uint32_t *) &arg[proc_id]);
        //d = htonl(aux);
        cout << "sin(" << arg[proc_id] << ") = ";
        fdata = sin(arg[proc_id]);
	}
	cout << fdata << '@' << a << endl;
    uint32_t data = htobe32(*reinterpret_cast<uint32_t*>(&fdata));

    d = data;
	
	return SUCCESS;
}

