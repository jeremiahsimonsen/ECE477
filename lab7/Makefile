CC=avr-gcc
# should either be .text=0x1800 or 0x7000
CFLAGS= -Wl,--section-start=.text=0x7000 -g -O0 -Wall -mcall-prologues -mmcu=atmega88
OBJ2HEX=avr-objcopy
UISP=/usr/local/bin/uisp
TARGET=lab7
#ADFLAGS=-p m8 -c FT232 -P /dev/tty.usbserial-A8003POq 
ADFLAGS=-p m88 -c FT232 -P /dev/ttyUSB0


.PHONY: fuses prog erase 
# .PHONY means that these are not a targetav


prog: $(TARGET).hex $(TARGET).eeprom  
#this programs the chip, depends on Target.hex and Target.eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i -F
	# avrdude $(ADFLAGS) -U eeprom:w:$(TARGET).eeprom:i

%.obj: %.o   
# make .obj (any obj) if corresponding .o is newer
	 $(CC) $(CFLAGS) $< -o $@  
	 # $@ means target of rule, $< means prerequisite of rule

%.hex: %.obj  
# similarly, make any .hex if corresponding .obj is newer
	$(OBJ2HEX) -R .eeprom -O ihex $< $@  
	# remove  eeprom output ihex translate .obj into hex file

%.eeprom: %.obj  
# any eeprom depends on same .obj
	$(OBJ2HEX) -j .eeprop -O ihex $< $@ 

# erase:   
# # this target will just erase avr
# 	avrdude $(ADFLAGS) -e

clean:  
# this target will remove all non-source files (will cause a complete re-build on next make)
	rm -f *.hex *.obj *.o

fuses: 
# program the fuses (NOT done by regular make)
	avrdude $(ADFLAGS) -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xf8:m
	#www.engbedded.com/fusecalc

term:
	avrdude $(ADFLAGS) -t