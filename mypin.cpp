#include "mypin.h"
#include "pins_arduino.h"

// Note: these are modded to make it easy to change pin direction and set level atomically

void myPinMode(uint8_t pin, uint8_t mode)
{
	volatile uint32_t *config;

	if (pin >= CORE_NUM_DIGITAL) return;
	config = portConfigRegister(pin);

	if (mode == OUTPUT || mode == OUTPUT_OPENDRAIN) {
#ifdef KINETISK
		*portModeRegister(pin) = 1;
#else
		*portModeRegister(pin) |= digitalPinToBitMask(pin); // TODO: atomic
#endif
		*config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
		if (mode == OUTPUT_OPENDRAIN) {
		    *config |= PORT_PCR_ODE;
		} else {
		    *config &= ~PORT_PCR_ODE;
                }
	} else {
		if (mode == INPUT) {
			*config = PORT_PCR_MUX(1);
		} else if (mode == INPUT_PULLUP) {
			*config = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
		} else if (mode == INPUT_PULLDOWN) {
			*config = PORT_PCR_MUX(1) | PORT_PCR_PE;
		} else { // INPUT_DISABLE
			*config = 0;
		}
#ifdef KINETISK
		*portModeRegister(pin) = 0;
#else
		*portModeRegister(pin) &= ~digitalPinToBitMask(pin);
#endif
	}
}

void myDigitalWrite(uint8_t pin, uint8_t val)
{
	if (pin >= CORE_NUM_DIGITAL) return;
#ifdef KINETISK
//	if (*portModeRegister(pin)) {
	if (1) {
		if (val) {
			*portSetRegister(pin) = 1;
		} else {
			*portClearRegister(pin) = 1;
		}
#else
//	if (*portModeRegister(pin) & digitalPinToBitMask(pin)) {
	if (1) {
		if (val) {
			*portSetRegister(pin) = digitalPinToBitMask(pin);
		} else {
			*portClearRegister(pin) = digitalPinToBitMask(pin);
		}
#endif
	} else {
		volatile uint32_t *config = portConfigRegister(pin);
		if (val) {
			// TODO use bitband for atomic read-mod-write
			*config |= (PORT_PCR_PE | PORT_PCR_PS);
			//*config = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
		} else {
			// TODO use bitband for atomic read-mod-write
			*config &= ~(PORT_PCR_PE);
			//*config = PORT_PCR_MUX(1);
		}
	}
}

