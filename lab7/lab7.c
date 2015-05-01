#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>

void main(void) {
	uint16_t buf;
	int i, address = 0;

	// Disable interrupts
	cli();

	eeprom_busy_wait();

	// Erase the page
	boot_page_erase_safe(address);
	// Wait until the memory is erased
	boot_spm_busy_wait();

	// Enable the RWW-section
	boot_rww_enable();
	while(boot_rww_busy());		// Wait until it's enabled

	// For the first 4 pages...
	for(address = 0; address < 4*SPM_PAGESIZE; address += SPM_PAGESIZE) {

		// For each word (2B) in the current page
		for (i = 0; i < SPM_PAGESIZE; i += 2) {
			// Set up little-endian word
			buf = (i/2)<<8;		// high byte
			buf |= (((i/2) + address/SPM_PAGESIZE) / 32));	// low byte
		}
	}

	// Enable the RWW-section
	boot_rww_enable();

	// Enable interrupts
	sei();

	while(1);

	return 0;
}