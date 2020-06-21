/*
 * Copyright (C) 2017 Obermaier Johannes
 *
 * This Source Code Form is subject to the terms of the MIT License.
 * If a copy of the MIT License was not distributed with this file,
 * you can obtain one at https://opensource.org/licenses/MIT
 */

#include <string.h>
#include "main.h"
#include "uart.h"
#include <core_pins.h>
#include <usb_serial.h>



#define UART_WAIT_TRANSMIT do {} while (0);
#define UART_BUFFER_LEN (12u)

static const char chrTbl[] = "0123456789ABCDEF";
uint8_t uartStr[UART_BUFFER_LEN] = {0u};
uint8_t uartStrInd = 0u;

static void uartExecCmd( uint8_t const * const cmd, uartControl_t * const ctrl );

void uartInit( void )
{
        Serial.begin(9600);
	return ;
}


static void uartExecCmd( uint8_t const * const cmd, uartControl_t * const ctrl )
{
	uint8_t i = 1u;
	uint8_t c = 0u;
	uint32_t hConv = 0u;

	switch (cmd[0])
	{
		case 'a':
		case 'A':

		case 'l':
		case 'L':
			hConv = 0u;

			for (i = 1; i < (UART_BUFFER_LEN - 1u); ++i)
			{
				c = cmd[i];
				if ((c <= '9') && (c >= '0'))
				{
					c -= '0';
				}
				else if ((c >= 'a') && (c <= 'f'))
				{
					c -= 'a';
					c += 0x0A;
				}
				else if ((c >= 'A') && (c <= 'F'))
				{
					c -= 'A';
					c += 0x0A;
				}
				else
				{
					break;
				}
				hConv <<= 4u;
				hConv |= c;
			}


			if ((cmd[0] == 'a') || (cmd[0] == 'A'))
			{
				/* Enforce 32-bit alignment */
				while ((hConv & 0x00000003u) != 0x00u)
				{
					--hConv;
				}

				ctrl->readoutAddress = hConv;
				uartSendStr("Start address set to 0x");
				uartSendWordHexBE(hConv);
				uartSendStr("\r\n");
			}
			else /* l or L */
			{
				/* Enforce 32-bit alignment */
				while ((hConv & 0x00000003u) != 0x00u)
				{
					++hConv;
				}

				ctrl->readoutLen = hConv;
				uartSendStr("Readout length set to 0x");
				uartSendWordHexBE(hConv);
				uartSendStr("\r\n");
			}

			break;

		case 'b':
		case 'B':
			ctrl->transmitHex = 0u;
			uartSendStr("Binary output mode selected\r\n");
			break;

		case 'e':
			ctrl->transmitLittleEndian = 1u;
			uartSendStr("Little Endian mode enabled\r\n");
			break;

		case 'E':
			ctrl->transmitLittleEndian = 0u;
			uartSendStr("Big Endian mode enabled\r\n");
			break;

		case 'h':
		case 'H':
			ctrl->transmitHex = 1u;
			uartSendStr("Hex output mode selected\r\n");
			break;

		case 'p':
		case 'P':
			printExtractionStatistics();
			break;

		case 's':
		case 'S':
			ctrl->active = 1u;
			uartSendStr("Flash readout started!\r\n");
			break;

		case '\n':
		case '\r':
		case '\0':
			/* ignore */
			break;

		default:
			uartSendStr("ERROR: unknown command\r\n");
			break;

	}
}


void uartReceiveCommands( uartControl_t * const ctrl )
{
	uint8_t uartData = 0u;
  if (Serial.available() > 0)
	{
		uartData = Serial.read();

		switch (uartData)
		{
			/* ignore \t */
			case '\t':
				break;

			/* Accept \r and \n as command delimiter */
			case '\r':
			case '\n':
				/* Execute Command */
				uartExecCmd(uartStr, ctrl);
				uartStrInd = 0u;
				memset(uartStr, 0x00u, sizeof(uartStr));
				break;

			default:
				if (uartStrInd < (UART_BUFFER_LEN - 1u))
				{
					uartStr[uartStrInd] = uartData;
					++uartStrInd;
				}
				break;
		}
	}

	return ;
}


void uartSendWordBin( uint32_t const val, uartControl_t const * const ctrl )
{
	if (ctrl->transmitLittleEndian)
	{
		uartSendWordBinLE( val );
	}
	else
	{
		uartSendWordBinBE( val );
	}
}


void uartSendWordHex( uint32_t const val, uartControl_t const * const ctrl )
{
	if (ctrl->transmitLittleEndian)
	{
		uartSendWordHexLE( val );
	}
	else
	{
		uartSendWordHexBE( val );
	}
}


void uartSendWordBinLE( uint32_t const val )
{
	uint8_t i = 0u;
	uint32_t tval = val;

	for (i = 0u; i < 4u; ++i)
	{
		Serial.write((uint8_t)( tval & 0xFFu));
		tval >>= 8u;
		UART_WAIT_TRANSMIT;
	}

	return ;
}


void uartSendWordBinBE( uint32_t const val )
{
	uint8_t i = 0u;
	uint32_t tval = val;

	for (i = 0u; i < 4u; ++i)
	{
		Serial.write((uint8_t)( (tval >> ((3u - i) << 3u)) & 0xFFu ));
		UART_WAIT_TRANSMIT;
	}

	return ;
}


void uartSendWordHexLE( uint32_t const val )
{
	uint8_t i = 0u;
	uint32_t tval = val;

	for (i = 0u; i < 4u; ++i)
	{
		uartSendByteHex( tval & 0xFFu );
		tval >>= 8u;
	}

	return;
}


void uartSendWordHexBE( uint32_t const val )
{
	uint8_t i = 0u;
	uint32_t tval = val;

	for (i = 0u; i < 4u; ++i)
	{
		uartSendByteHex((tval >> ((3u - i) << 3u)) & 0xFFu);
		UART_WAIT_TRANSMIT;
	}

	return ;
}


void uartSendByteHex( uint8_t const val )
{
	char sendstr[3] = {0};

	sendstr[0] = chrTbl[(val >> 4u) & 0x0Fu];
	sendstr[1] = chrTbl[val & 0x0Fu];
	sendstr[2] = '\0';

	uartSendStr( sendstr );

	return ;
}

void uartSendStr( const char * const str )
{
	Serial.print(str);
}
