/*
 * Copyright (C) 2017 Obermaier Johannes
 *
 * This Source Code Form is subject to the terms of the MIT License.
 * If a copy of the MIT License was not distributed with this file,
 * you can obtain one at https://opensource.org/licenses/MIT
 */

#include "target.h"
#include "clk.h"
#include "wiring.h"
#include "mypin.h"

void targetSysCtrlInit( void )
{
	//RCC->AHBENR |= RCC_AHBENR_GPIO_RESET;
	//RCC->AHBENR |= RCC_AHBENR_GPIO_POWER;

	//GPIO_RESET->MODER |= (0x01u << (PIN_RESET << 1u));
	//GPIO_POWER->MODER |= (0x01u << (PIN_POWER << 1u));

	//GPIO_RESET->OSPEEDR |= (0x03u << (PIN_RESET << 1u));
	//GPIO_POWER->OSPEEDR |= (0x03u << (PIN_POWER << 1u));

	myDigitalWrite(PIN_RESET, LOW);
	myDigitalWrite(PIN_POWER, HIGH);

	myPinMode(PIN_RESET, OUTPUT);
	myPinMode(PIN_POWER, OUTPUT);

	targetSysOff();
	targetSysReset();

	return ;
}

void targetSysReset( void )
{
	//GPIO_RESET->BSRR = (0x01u << (PIN_RESET + BSRR_CLEAR));
	myDigitalWrite(PIN_RESET, LOW);

	return ;
}

void targetSysUnReset( void )
{
	//GPIO_RESET->BSRR = (0x01u << (PIN_RESET + BSRR_SET));
	myDigitalWrite(PIN_RESET, HIGH);
	return ;
}


void targetSysOff( void )
{
	//GPIO_POWER->BSRR = (0x01u << (PIN_POWER + BSRR_CLEAR));
	myDigitalWrite(PIN_POWER, HIGH);
	return ;
}

void targetSysOn( void )
{
	//GPIO_POWER->BSRR = (0x01u << (PIN_POWER + BSRR_SET));
	myDigitalWrite(PIN_POWER, LOW);
	return ;
}
