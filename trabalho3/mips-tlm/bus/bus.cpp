/**
 * @file      memory.cpp
 * @author    Bruno de Carvalho Albertini
 *
 * @author    The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   0.1
 * @date      Sun, 02 Apr 2006 08:07:46 -0200
 *
 * @brief     Implements a ac_tlm memory.
 *
 * @attention Copyright (C) 2002-2005 --- The ArchC Team
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *
 */

//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

// Deifnie limite da memória

//#define DEBUG


#include "bus.h"

//////////////////////////////////////////////////////////////////////////////


/// Constructor
ac_tlm_bus::ac_tlm_bus(sc_module_name module_name):
  sc_module(module_name),
  target_export("iport"),
  MEM_port("MEM_port", MEMSIZE), // This is the memory port, assigned for 5MB
  PERIPHERAL_port("PERIPHERAL_port", LOCK_SIZE), // This is the peripheral 
  TRIGONOMETRIC_port("TRIGONOMETRIC_port", TRIGONOMETRIC_SIZE),  
  FLOATING_port("FLOATING_port", FLOATING_SIZE)
{
	/// Binds target_export to the memory
	target_export(*this);

}

/// Destructor
ac_tlm_bus::~ac_tlm_bus()
{
}

/// This is the transport method. Everything should go through this file.
/// To connect more components, you will need to have an if/then/else or a switch
/// statement inside this method. Notice that ac_tlm_req has an address field.
ac_tlm_rsp ac_tlm_bus::transport(const ac_tlm_req &request){
	ac_tlm_rsp response;
	
	if(request.addr < MEMSIZE) {

	  response = MEM_port->transport(request);

	} else 
	if(request.addr == MEMSIZE){
	  response = PERIPHERAL_port->transport(request);
	} else 
	if(FLOATING_BASE <= request.addr and request.addr < FLOATING_BASE + FLOATING_SIZE){
		#ifdef DEBUG
		cout << "bus floating transport request addr " << request.addr<< endl;
		cout << "(addr - base)/4 = " << (request.addr - FLOATING_BASE)/4 << endl;
		#endif
		response = FLOATING_port->transport(request);
	}else 
	if(TRIGONOMETRIC_BASE <= request.addr and request.addr < TRIGONOMETRIC_BASE + TRIGONOMETRIC_SIZE){
		#ifdef DEBUG
		cout << "bus trigonometric transport request addr " << request.addr<< endl;
		cout << "(addr - base)/4 = " << (request.addr - TRIGONOMETRIC_BASE)/4 << endl;
		#endif
		response = TRIGONOMETRIC_port->transport(request);
	} else {
		#ifdef DEBUG
		cout << "other " << request.addr << endl;
		#endif
	}
	return response;
}
