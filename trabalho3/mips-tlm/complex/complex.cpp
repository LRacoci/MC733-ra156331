//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include "complex.h"
#include <stdio.h>
#include <netinet/in.h>
#include <math.h>

#define COMPLEX_ARGS COMPLEX_BASE
#define COMPLEX_ANSWER COMPLEX_ARGS + 2*8U

#define ARG(z) ( atan((z).re/(z).im))
#define ABS(z) ( sqrt((z).re*(z).re + (z).im*(z).im ) )
//////////////////////////////////////////////////////////////////////////////


/// Constructor
ac_tlm_complex::ac_tlm_complex( sc_module_name module_name , int k) :
	sc_module( module_name ),
	target_export("iport")
{
	/// Binds target_export to the memory
	target_export( *this );

}

/// Destructor
ac_tlm_complex::~ac_tlm_complex() {

}

/** Internal Write
	* Note: Always write 32 bits
	* @param a is the address to write
	* @param d id the data being write
	* @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status ac_tlm_complex::writem( const uint32_t &a , const uint32_t &d )
{
	if( COMPLEX_BASE <= a and a < COMPLEX_ANSWER ){
		//counter = 0;		
		*(float*)(&args + a - COMPLEX_BASE) = (float) ntohl(d);//*((float*) & ntohl(d))
	}
}

/** Internal Read
	* Note: Always read 32 bits
	* @param a is the address to read
	* @param d id the data that will be read
	* @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status ac_tlm_complex::readm( const uint32_t &a , uint32_t &d )
{
	/*
	while( counter < 16){
		counter ++;
		return FAILURE;
		*((uint32_t *) &d) = htonl(-1);
	}
	counter = 0;
	*/

	resp[0].re = args[0].re + args[0].re;
	resp[0].im = args[0].im + args[0].im;
	
	resp[1].re = args[0].re * args[1].re - args[0].im * args[1].im;
	resp[1].im = args[0].re * args[1].im + args[0].im * args[1].re;

	resp[2].re = exp(-args[1].im * ARG(args[0])) * ABS(args[0]) * cos(args[1].re * ARG(args[0]));
	resp[2].im = exp(-args[1].im * ARG(args[0])) * ABS(args[0]) * sin(args[1].re * ARG(args[0]));

	if( COMPLEX_ANSWER <= a and a < COMPLEX_BASE + COMPLEX_SIZE){
		//*((uint32_t *) &d) = htonl(*((uint32_t *) &real_part));
		d = htonl(*((uint32_t *) (&resp + a - COMPLEX_ANSWER)));
	}
	return SUCCESS;
}
