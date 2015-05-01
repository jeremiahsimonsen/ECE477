#include <avr/boot.h>

int main(void) {
	int addr = 0x0000;
	char low = 0x00;
	char high = 0x00;
	// int i = 0;

	// Verify using avrdude -t
	// dump flash 0 128

	// Erase the first 4 pages of memory
	// 1 word = 2 Bytes
	// 1 page = 32 words = 64 bytes
	boot_page_erase_safe(0x0);
	// boot_page_erase_safe(0x40);
	// boot_page_erase_safe(0x80);
	// boot_page_erase_safe(0x100);

	// Fill the page buffer
	for (addr = 0x0000; addr <= 0x0040; addr +=2) {
		// boot_page_fill	(address,data);
		boot_page_fill(addr, ((high<<8)|low));
		low++;
	}
	// Write the page buffer
	boot_page_write_safe(0x0000);

// boot_page_write_safe	(address)	

	return 0;
}