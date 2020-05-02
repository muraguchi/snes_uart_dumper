/*
main.c

Copyright (c) 2020 muraguchi 

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

// 
// Target device: ATMEGA164P
//

//
// Fuse setting
// 
// -U lfuse:w:0xdf:m -U hfuse:w:0x99:m -U efuse:w:0xfc:m
//
// Low
//  bit7: CKDIV8: Unprogrammed
//  bit6: CKOUT : Unprogrammed
//  bit5: SUT1  : Programmed
//  bit4: SUT0  : Unprogrammed
//  bit3-0: CKSEL3-0: Unprogrammed
// High
//  bit7: OCDEN : Unprogrammed
//  bit6: JTAGEN: Programmed
//  bit5: SPIEN : Programmed
//  bit4: WDTON : UnProgrammed
//  bit3-2: BOOTSZ1-0: Programmed
//  bit0: BOOTRST : Programmed
// Extended
//  bit2: BODLEVEL2: Programmded
//  bit1: BODLEVEL1: Unprogrammed
//  bit0: BODLEVEL0: Unprogrammed



// 18.432MHz
#define F_CPU 18432000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// UART baudrate :460.8kbps
// #define BAUDRATE 115200
// #define BAUDRATE 230400
#define BAUDRATE 460800
// #define BAUDRATE 921600 

// receive buffer 2^USART_RX_SZ_BITS = USART_RX_LEN
#define USART_RX_SZ_BITS 2
#define USART_RX_LEN     4
// transmit buffer 2^USART_TX_SZ_BITS = USART_TX_LEN
#define USART_TX_SZ_BITS 2 
#define USART_TX_LEN     4


void USART_init();
char USART_rx_pop(unsigned char *rd);
void USART_tx_push(unsigned char wr);
unsigned char hex_to_bin(unsigned char hex);
unsigned char nibble_to_hex(unsigned char bin);

volatile unsigned char read_mode=0;

static volatile unsigned char USART_rx_wr_ptr;
static volatile unsigned char USART_rx_rd_ptr;
static volatile unsigned char USART_rx_b[USART_RX_LEN];
static volatile unsigned char USART_tx_wr_ptr;
static volatile unsigned char USART_tx_rd_ptr;
static volatile unsigned char USART_tx_b[USART_TX_LEN];

static volatile unsigned char rd_hist[256];

void L_set(unsigned char ch,unsigned char val);
void single_write_a(unsigned char addr_high,unsigned char addr_mid,unsigned char addr_low,unsigned char write_data);
unsigned char single_read_a(unsigned char addr_high,unsigned char addr_mid,unsigned char addr_low);
unsigned char page_read_a(unsigned char addr_high,unsigned char addr_mid);


ISR(USART0_RX_vect){
  unsigned char next_rx_wr_ptr;
  next_rx_wr_ptr = ( USART_rx_wr_ptr + 1 ) & ( USART_RX_LEN - 1 );
  // If FIFO is full, discarded.
  if (next_rx_wr_ptr==USART_rx_rd_ptr)
    return;
  // else [Aread data and progress write pointer
  else {
    USART_rx_b[USART_rx_wr_ptr]=UDR0;
    USART_rx_wr_ptr=next_rx_wr_ptr;
  }
}

ISR(USART0_UDRE_vect){
  // If fifo is empty, disable UDR intrrupt
  if (USART_tx_wr_ptr==USART_tx_rd_ptr){
    //UCR = (1 << RXCIE) | (0<< TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) |(0<<CHR9) ;
    UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 0<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
  }
  // pop tx data
  else {
    UDR0 = USART_tx_b[USART_tx_rd_ptr];
    USART_tx_rd_ptr = ( USART_tx_rd_ptr + 1 ) & ( USART_TX_LEN - 1 );
  }
}

#define DDRLC   DDRD
#define DDRLD   DDRA
#define PORTLC  PORTD
#define PORTLD  PORTA
#define PLLE0   PD2
#define PLLE1   PD3
#define PLLE2   PD4
#define PLLE3   PD5
#define PLLE4   PD6
#define PLOEB   PD7

#define DDRDATA  DDRB
#define PINDATA  PINB
#define PORTDATA PORTB

#define PC_RD      PC0
#define PC_WR      PC1
#define PC_PARD    PC2
#define PC_PAWR    PC3
#define PL3_WRAM    PA4
#define PL3_REFRESH PA5
#define PL3_CART    PA6
#define PL3_PPUCLK  PA7


#define LALL_LE_TH 0x7C
void L_init();

void L_init(){
  // LOAD ALL HIGH
  DDRA  = 0xff;
  PORTA = 0xff;
  DDRD  = 0xff;

  PORTC = 0xff;
  DDRC  = 0x0f;
  // through
  PORTLC = 1 << PLOEB ;

  // latch
  PORTLC = 1 << PLOEB | 1 << PLLE4 | 1 << PLLE3 |  1 << PLLE2 |  1 << PLLE1 | 1 << PLLE0 ;  

  // output enable
  PORTLC = 0 << PLOEB | 1 << PLLE4 | 1 << PLLE3 |  1 << PLLE2 |  1 << PLLE1 | 1 << PLLE0 ;
  // PL3 /CART low, /RD low
  L_set(3,(1 << PL3_PPUCLK)|(1 << PL3_CART)|(0 << PL3_REFRESH)|(1 << PL3_WRAM));
}

void L_set(unsigned char ch,unsigned char val){
  unsigned char th_ctrl = 0 << PLOEB | 1 << PLLE4 | 1 << PLLE3 |  1 << PLLE2 |  1 << PLLE1 | 1 << PLLE0 ;
  // PORTLC =  0 << PLOEB | 1 << PLLE4 | 1 << PLLE3 |  1 << PLLE2 |  1 << PLLE1 | 1 << PLLE0 ;
  PORTLD = val;
  if (ch==0){
    th_ctrl = 0 << PLOEB | 0 << PLLE4 | 0 << PLLE3 |  0 << PLLE2 |  0 << PLLE1 | 1 << PLLE0 ;
  } else if (ch==1){
    th_ctrl = 0 << PLOEB | 0 << PLLE4 | 0 << PLLE3 |  0 << PLLE2 |  1 << PLLE1 | 0 << PLLE0 ;
  } else if (ch==2){
    th_ctrl = 0 << PLOEB | 0 << PLLE4 | 0 << PLLE3 |  1 << PLLE2 |  0 << PLLE1 | 0 << PLLE0 ;
  } else if (ch==3){
    th_ctrl = 0 << PLOEB | 0 << PLLE4 | 1 << PLLE3 |  0 << PLLE2 |  0 << PLLE1 | 0 << PLLE0 ;
  } else if (ch==4){
    th_ctrl = 0 << PLOEB | 1 << PLLE4 | 0 << PLLE3 |  0 << PLLE2 |  0 << PLLE1 | 0 << PLLE0 ;
  }
  PORTLC = th_ctrl;
  PORTLC = 0 << PLOEB | 0 << PLLE4 | 0 << PLLE3 |  0 << PLLE2 |  0 << PLLE1 | 0 << PLLE0 ;
}



void USART_init(){
  // Stop Interrupt
  cli();

  // Init pointers
  USART_rx_wr_ptr=0;
  USART_rx_rd_ptr=0;
  USART_tx_wr_ptr=0;
  USART_tx_rd_ptr=0;
  // Init UBRR
  UBRR0H = (unsigned char)((F_CPU/8/BAUDRATE-1)>>8);
  UBRR0L = (unsigned char)((F_CPU/8/BAUDRATE-1)&0xff);
  UCSR0A = 1 << U2X0 | 0<<MPCM0;
  UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 0<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
  UCSR0C = 0 << UMSEL00 | 0 << UPM01  | 0<<UPM00  | 0<< USBS0 | 1<<UCSZ01|1<<UCSZ00|0<<UCPOL0;

  // Enable Interrupt
  sei();  
}

// pop rxdata
char USART_rx_pop(unsigned char * rd)
{
  // If fifo is empty, return 0
  if (USART_rx_wr_ptr==USART_rx_rd_ptr){
    return 0;
  }
  // get read data and return 1
  else {
    *rd = USART_rx_b[USART_rx_rd_ptr];
    USART_rx_rd_ptr = ( USART_rx_rd_ptr + 1 ) & ( USART_RX_LEN - 1 );
    return 1;
  }
}

// push txdata
void USART_tx_push(unsigned char wr)
{
  unsigned char next_tx_wr_ptr;
  next_tx_wr_ptr = ( USART_tx_wr_ptr + 1 ) & ( USART_TX_LEN - 1 );
  // If FIFO is full,
  while (next_tx_wr_ptr==USART_tx_rd_ptr){;}

  USART_tx_b[USART_tx_wr_ptr]=wr;
  USART_tx_wr_ptr = next_tx_wr_ptr;
  // Enable empty interrupt
  //UCR = (1 << RXCIE) | (0<< TXCIE) | (1<<UDRIE) | (1<<RXEN) | (1<<TXEN) |(0<<CHR9) ;
  UCSR0B = 1 << RXCIE0 | 0 << TXCIE0 | 1<<UDRIE0 | 1<< RXEN0 | 1<< TXEN0 | 0<<UCSZ02;
}

unsigned char hex_to_bin(unsigned char hex){
  if (hex>='0' && hex <='9')
    return hex - '0';
  else if (hex>='A' && hex <='F')
    return (hex - 'A') + 10;
  else if (hex>='a' && hex <='f')
    return (hex - 'a') + 10;
  else
    return 0;
}
unsigned char nibble_to_hex(unsigned char bin){
  if (bin<10)
    return '0'+bin;
  else if (bin<16)
    return 'A'-10+bin;
  else
    return 0;
}
void single_write_a(unsigned char addr_high,unsigned char addr_mid,unsigned char addr_low,unsigned char write_data)
{
  // PL3 /CART low, /RD high
  L_set(3,(1 << PL3_PPUCLK)|(0 << PL3_CART)|(0 << PL3_REFRESH)|(1 << PL3_WRAM));
  PORTC = ( (1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(1 << PC_RD));

  // PL2 ADDR HIGH
  L_set(1, addr_high);

  // PL1 ADDR MID
  L_set(0, addr_mid);

  // ADDR LOW
  PORTA = addr_low;

  // output data
  PORTDATA = write_data;
  // output direction
  DDRDATA = 0xff;
  asm volatile (
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		::);
  _delay_ms(1);
  PORTC = ( (1 << PC_PAWR)|(1 << PC_PARD)|(0 << PC_WR)|(1 << PC_RD));
  _delay_ms(1);
  asm volatile (
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		::);
  PORTC = ( (1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(1 << PC_RD));
  _delay_ms(1);
  asm volatile (
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		::);
  // input direction
  DDRDATA = 0x00;
  PORTDATA = 0x00;
}

unsigned char single_read_a(unsigned char addr_high,unsigned char addr_mid,unsigned char addr_low){
  unsigned char retval;
  unsigned char maxcnt;
  unsigned char maxchr;
  // DATA Bus direction : input
  DDRDATA = 0x00;
  
  // PL3 /CART low, /RD low
  L_set(3,(1 << PL3_PPUCLK)|(0 << PL3_CART)|(0 << PL3_REFRESH)|(1 << PL3_WRAM));

  PORTC = ((1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(0 << PC_RD));
  
  // PL2 ADDR HIGH
  L_set(1, addr_high);

  // PL1 ADDR MID
  L_set(0, addr_mid);

  // ADDR LOW
  PORTA = addr_low;
  asm volatile (
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		::);
  if(read_mode==0){
    retval = PINDATA;
    PORTC=((1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(1 << PC_RD));
    return retval;
  }else {
    // zero clear rd_hist
    for (int i=0;i<256;i++){
      rd_hist[i]=0;
    }
    maxcnt=0;
    maxchr=0xff;
    // Get hist
    for (int i=0;i<255;i++){
      retval = PINDATA;
      rd_hist[retval]=rd_hist[retval]+1;
      if(rd_hist[retval]>maxcnt){
	maxcnt=rd_hist[retval];
	maxchr=retval;
      }
    }
  
    // Disp hist
    USART_tx_push('\r');
    USART_tx_push('\n');
  
    for (int i=1;i<256;i++){
      for(int l=0;l<256;l++){
	if (rd_hist[l]==i) {
	  USART_tx_push(nibble_to_hex(l>>4));
	  USART_tx_push(nibble_to_hex(l&0xf));
	  USART_tx_push(':');
	  USART_tx_push(nibble_to_hex(i>>4));
	  USART_tx_push(nibble_to_hex(i&0xf));
	  USART_tx_push('\r');
	  USART_tx_push('\n');
	  retval = l;
	}
      }
    }
    USART_tx_push('\r');
    USART_tx_push('\n');
    return maxchr;
  } 
}
unsigned char page_read_a(unsigned char addr_high,unsigned char addr_mid){
  unsigned long int trans_bytes;
  unsigned char addr_low;
  unsigned char val;
  // DATA Bus direction : input
  DDRDATA = 0x00;
  
  // PL3 /CART low, /RD low
  L_set(3,(1 << PL3_PPUCLK)|(0 << PL3_CART)|(0 << PL3_REFRESH)|(1 << PL3_WRAM));
  PORTC=((1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(0 << PC_RD));
  // Fixed addr high
  L_set(1,addr_high);

  // Set addr mid
  L_set(0,addr_mid);
  
  trans_bytes=0;
  addr_low=0;

  while(trans_bytes<32*1024UL){
    PORTA = addr_low;
    asm volatile (
		  "nop\n\t"
		  "nop\n\t"
		  "nop\n\t"
		  "nop\n\t"
		  "nop\n\t"
		  "nop\n\t"
		  ::);
    
    val = PINDATA;
    USART_tx_push(nibble_to_hex(val>>4));
    USART_tx_push(nibble_to_hex(val&0xf));

    addr_low++;
    if (addr_low==0) {
      addr_mid++;
      L_set(0,addr_mid);
    }
    trans_bytes++;
  }


  // set idle
  PORTC=((1 << PC_PAWR)|(1 << PC_PARD)|(1 << PC_WR)|(1 << PC_RD));
}

void main ()
{
  // RX data
  unsigned char rxd;
  unsigned char state;

  unsigned char ch;
  unsigned char val;

  unsigned char address_high;
  unsigned char address_mid ;
  unsigned char address_low ;
  unsigned char write_data  ;

  
  state=0;
  L_init();
  
  // Wait 100ms
  for(int l=0;l<100;l++) {
    _delay_ms(1);
  }

  // 
  // S 0 (next 20) SRA (single read)
  // P 0 (next 21) PRA (32KB block read)
  // R 20 (next 30)
  // R 21 (next 31)
  // A 30 (next 40)
  // A 31 (next 51)
  // SRA address high higher 4 bit 40 next 41
  // SRA address high lower  4 bit 41 next 42
  // SRA address mid  higher 4 bit 42 next 43
  // SRA address mid  lower  4 bit 43 next 44 
  // SRA address low  higher 4 bit 44 next 45
  // SRA address low  lower  4 bit 45 next 0
  // PRA address high h 50 next 51
  // PRA address high l 51 next 52
  // PRA address mid  h 52 next 53
  // PRA address mid  l 53 next 0  


  

  
  USART_init();
  while(1)
    {
      while (USART_rx_pop(&rxd)) {
	switch(state){
	case(0):
	  if(rxd=='L'){
	    state=1;
	  }
	  else if (rxd=='S'){
	    state=20;
	  }
	  else if (rxd=='P'){
	    state=21;
	  }
	  else if (rxd=='X'){
	    read_mode=1;
	    USART_tx_push(nibble_to_hex(1));
	  }
	  else if (rxd=='x'){
	    read_mode=0;
	    USART_tx_push(nibble_to_hex(0));
	  }
	  break;
	case(1):
	  ch = hex_to_bin(rxd);
	  state=2;
	  break;
	case(2):
	  val = hex_to_bin(rxd)<<4;
	  state=3;
	  break;
	case(3):
	  val |= hex_to_bin(rxd);
	  L_set(ch,val);
	  USART_tx_push('L');
	  USART_tx_push(nibble_to_hex(ch&0xf));
	  USART_tx_push('V');
	  USART_tx_push(nibble_to_hex(val>>4));
	  USART_tx_push(nibble_to_hex(val&0xf));
	  state=0;
	  break;
	case(20):
	  if(rxd=='R'){
	    state=30;
	  } else if (rxd=='W') {
	    state=32;
	  } else {
	    state = 0;
	  }
	  break;
	case(21):
	  if(rxd=='R'){
	    state=31;
	  } else {
	    state = 0;
	  }
	  break;
	case(30):
	  if(rxd=='A'){
	    state=40;
	  } else {
	    state=0;
	  }
	  break;
	case(31):
	  if(rxd=='A'){
	    state=50;
	  } else {
	    state=0;
	  }
	  break;
	case(32):
	  if(rxd=='A'){
	    state=60;
	  } else {
	    state=0;
	  }
	  break;
	case(40):
	  address_high = hex_to_bin(rxd)<<4;
	  state=41;
	  break;
        case(41):
	  address_high |= hex_to_bin(rxd);
	  state=42;
	  break;
        case(42):
	  address_mid  = hex_to_bin(rxd)<<4;
	  state=43;
	  break;
        case(43):
	  address_mid |= hex_to_bin(rxd);
	  state=44;
	  break;
        case(44):
	  address_low  = hex_to_bin(rxd)<<4;
	  state=45;
	  break;
        case(45):
	  address_low |= hex_to_bin(rxd);
          val = single_read_a(address_high,address_mid,address_low);
          USART_tx_push(nibble_to_hex(val>>4));
          USART_tx_push(nibble_to_hex(val&0xf));
          state=0;
	  break;
	case(50):
	  address_high = hex_to_bin(rxd)<<4;
	  state=51;
	  break;
        case(51):
	  address_high |= hex_to_bin(rxd);
	  state=52;
	  break;
        case(52):
	  address_mid  = hex_to_bin(rxd)<<4;
	  state=53;
	  break;
        case(53):
	  address_mid |= hex_to_bin(rxd);
	  state=0;
	  page_read_a(address_high,address_mid);
	  break;
	case(60):
	  address_high = hex_to_bin(rxd)<<4;
	  state=61;
	  break;
        case(61):
	  address_high |= hex_to_bin(rxd);
	  state=62;
	  break;
        case(62):
	  address_mid  = hex_to_bin(rxd)<<4;
	  state=63;
	  break;
        case(63):
	  address_mid |= hex_to_bin(rxd);
	  state=64;
	  break;
        case(64):
	  address_low  = hex_to_bin(rxd)<<4;
	  state=65;
	  break;
        case(65):
	  address_low |= hex_to_bin(rxd);
          state=66;
	  break;
        case(66):
	  write_data  = hex_to_bin(rxd)<<4;
	  state=67;
	  break;
        case(67):
	  write_data |= hex_to_bin(rxd);
	  single_write_a(address_high,address_mid,address_low,write_data);
	  USART_tx_push(nibble_to_hex(write_data>>4));
          USART_tx_push(nibble_to_hex(write_data&0xf));
          state=0;
	  break;






	}
	
    } // RX POP
    } // INF
}

