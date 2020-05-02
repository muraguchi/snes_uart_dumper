all: compile upload
compile:
	avr-gcc -o main.elf main.c -mmcu=atmega164p -Os -g0
	avr-objcopy -I elf32-avr -O ihex main.elf main.hex
	avr-size -C main.elf --mcu=atmega164p
upload:
	#firmware uploading command HERE for your environment. 
	sudo avrdude -V -c avrispmkII -p m164p -U flash:w:main.hex
	
clean:
	rm -rf main.elf main.hex 

fuse:
	sudo avrdude -V -c avrispmkII -p m164p  -U lfuse:w:0xdf:m -U hfuse:w:0x99:m -U efuse:w:0xfc:m

