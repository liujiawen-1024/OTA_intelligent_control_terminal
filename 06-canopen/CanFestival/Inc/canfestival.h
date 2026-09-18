/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN
AVR Port: Andreas GLAUSER and Peter CHRISTEN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef __CAN_CANFESTIVAL__
#define __CAN_CANFESTIVAL__

#include "applicfg.h"
#include "data.h"
#include <stdint.h>

typedef struct
{
	uint32_t StdId; 		/*!< Specifies the standard identifier. This parameter can be a value between 0 to 0x7FF. */
	uint32_t ExtId; 		/*!< Specifies the extended identifier. This parameter can be a value between 0 to 0x1FFFFFFF. */
	uint8_t IDE; 				/*!< Specifies the type of identifier for the message that will be transmitted. This parameter can be a value of @ref CAN_identifier_type */
	uint8_t RTR; 				/*!< Specifies the type of frame for the message that will be transmitted. This parameter can be a value of @ref CAN_remote_transmission_request */
	uint8_t DLC;	 			/*!< Specifies the length of the frame that will be transmitted. This parameter can be a value between 0 to 8 */
	uint8_t Data[8]; 		/*!< Contains the data to be transmitted. It ranges from 0to 0xFF. */
	} CanTxMsg;

// --------- to be called by user app ---------
	
void initTimer(void);
UNS8 canSend(CAN_PORT notused, Message *m);
UNS8 canChangeBaudRate(CAN_PORT port, char* baud);
uint8_t CanFestival_Can_Init(void);

#endif
