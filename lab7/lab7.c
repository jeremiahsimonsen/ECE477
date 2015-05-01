#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>

void main(void) {
	uint8_t sreg;
	int i,j;

	sreg = SREG;
	cli();
	boot_spm_busy_wait();

	for (j = 0; j < 4; j++) {
		boot_page_erase_safe(j<<6);
		boot_spm_busy_wait();

		for (i = 0; i < 64; i+=2) {
			boot_page_fill_safe(i, (j)|(i<<7));
		}

		boot_page_write_safe(j<<6);
		boot_spm_busy_wait();
		boot_rww_enable();
	}	

	SREG = sreg;
	while(1);

	return 0;
}