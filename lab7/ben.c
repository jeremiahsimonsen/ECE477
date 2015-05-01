// Ben Carlson
// Jasmine Despres
// Lab 7 - Part A
#define F_CPU 8000000U

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

__attribute__ ((section (".bootloader"))); 
int main()
{

	DDRD |= 0x2;

	int i = 0, page_addr;
	const int pages = 2;	// modify the first 4 pages

	/* clear the first 4 pages */
	// memset(0x00, 0, 128 * 4);
	cli();

	eeprom_busy_wait();
	boot_page_erase((unsigned long)page_addr);
	boot_spm_busy_wait();

	boot_rww_enable();
	while(boot_rww_busy());

	for (page_addr = 0; page_addr < pages * SPM_PAGESIZE; page_addr+=SPM_PAGESIZE)
	{
		for (i = 0; i < SPM_PAGESIZE; i+=2)
		{
			// boot_page_erase(i);
			boot_page_fill(page_addr + i, (( (i/2) % 32)<< 8) | (((i/2) + page_addr/SPM_PAGESIZE) / 32));
		}

		boot_page_write(page_addr);
		boot_spm_busy_wait();
	}
	/* iterate over words */



	boot_rww_enable();

	sei();

	
	while(1)
	{
		if (i)
		{
			PORTD |= 0x2;
		} else {
			PORTD &= ~(0x2);
		}

		i = !i;
		_delay_ms(500);
	}

	return 0;

}