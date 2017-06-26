/**
 * @file      memory.h
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
 * @brief     Defines a ac_tlm memory.
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

#ifndef _BUS_H_
#define _BUS_H_

//////////////////////////////////////////////////////////////////////////////

// Standard includes
// SystemC includes
#include <systemc>
// ArchC includes
#include "ac_tlm_protocol.H"
#include  "ac_tlm_port.H"

#define NUM_PROCS 2



#define MEMBASE 0
#define MEMSIZE 67108864U //536870912U
#define LOCK_BASE MEMSIZE
#define LOCK_SIZE 4U

#define FLOATING_BASE (LOCK_BASE + LOCK_SIZE)
#define FLOATING_ARG1 (FLOATING_BASE + 0*4U)
#define FLOATING_ARG2 (FLOATING_BASE + 1*4U)
#define FLOATING_ADD  (FLOATING_BASE + 2*4U)
#define FLOATING_SUB  (FLOATING_BASE + 3*4U)
#define FLOATING_MULT (FLOATING_BASE + 4*4U)
#define FLOATING_DIVI (FLOATING_BASE + 5*4U)
#define FLOATING_LOG2 (FLOATING_BASE + 6*4U)
#define FLOATING_SQRT (FLOATING_BASE + 7*4U)
#define FLOATING_SIZE (NUM_PROCS * (2+6)*4U)

#define TRIGONOMETRIC_BASE (FLOATING_BASE + FLOATING_SIZE)
#define COS_ADD (TRIGONOMETRIC_BASE + 0*4U)
#define SIN_ADD (TRIGONOMETRIC_BASE + 1*4U)
#define TRIGONOMETRIC_SIZE (NUM_PROCS * 8U)

//////////////////////////////////////////////////////////////////////////////

// using statements
using tlm::tlm_transport_if;

//////////////////////////////////////////////////////////////////////////////

//#define DEBUG


/// A TLM memory
class ac_tlm_bus :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
public:
  /// Exposed port with ArchC interface
  sc_export<ac_tlm_transport_if> target_export;
  ac_tlm_port MEM_port;
  ac_tlm_port PERIPHERAL_port;
  ac_tlm_port TRIGONOMETRIC_port;
  ac_tlm_port FLOATING_port;


  /**
   * Implementation of TLM transport method that
   * handle packets of the protocol doing apropriate actions.
   * This method must be implemented (required by SystemC TLM).
   * @param request is a received request packet
   * @return A response packet to be send
  */
  ac_tlm_rsp transport(const ac_tlm_req &request);


  /**
   * Default constructor.
   */
  ac_tlm_bus(sc_module_name module_name);

  /**
   * Default destructor.
   */
  ~ac_tlm_bus();

};

#endif //_BUS_H_
