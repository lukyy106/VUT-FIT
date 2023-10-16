
//#define _XOPEN_SOURCE
//#define __USE_XOPEN
//#define _GNU_SOURCE
#include "MK60D10.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_SIZE 255
#define AUTO_POSTPONE 30
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

/* Mapping of LEDs and buttons to specific port pins: */
#define LED_D9  0x20      // Port B, bit 5
#define LED_D10 0x10      // Port B, bit 4
#define LED_D11 0x8       // Port B, bit 3
#define LED_D12 0x4       // Port B, bit 2

#define BTN_SW2 0x400     // Port E, bit 10
#define BTN_SW3 0x1000    // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800     // Port E, bit 11

#define SPK 0x10          // Speaker is on PTA4

int pressed_up = 0, pressed_down = 0;

char buffer[BUFFER_SIZE];
struct tm t; // setup time
struct tm a;// time of alarm
unsigned int alarm; // time of alarm in seconds
bool alarm_flag = 0; // is alarm set up
int postpones = 0; // number of postpones
int postponed_delay = 0; // seconds of delay
int sound = 0; // effect of buzzer
int light = 0; // light effect


/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(){
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK; // turn off watchdog
}


void UART5Init(){
    UART5->C2  &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    UART5->BDH =  0x00;
    UART5->BDL =  0x1A; // Baud rate 115 200 Bd, 1 stop bit
    UART5->C4  =  0x0F; // Oversampling ratio 16, match address mode disabled
    UART5->C1  =  0x00; // 8 data bitu, bez parity
    UART5->C3  =  0x00;
    UART5->MA1 =  0x00; // no match address (mode disabled in C4)
    UART5->MA2 =  0x00; // no match address (mode disabled in C4)
    UART5->S2  |= 0xC0;
    UART5->C2  |= ( UART_C2_TE_MASK | UART_C2_RE_MASK ); // Zapnout vysilac i prijimac
}

void PortsInit(void){
	// Enable CLOCKs for: UART5, RTC, PORT-A, PORT-B, PORT-E
	SIM->SCGC1 = SIM_SCGC1_UART5_MASK;
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;
	SIM->SCGC6 = SIM_SCGC6_RTC_MASK;

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    PORTE->PCR[8]  = PORT_PCR_MUX(0x03); // UART0_TX
    PORTE->PCR[9]  = PORT_PCR_MUX(0x03); // UART0_RX
    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    PORTA->PCR[4] = PORT_PCR_MUX(0x01);  // Speaker

    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD(0x3C);     // LED ports as outputs
    PTA->PDDR = GPIO_PDDR_PDD(SPK);     // Speaker as output
    PTB->PDOR |= GPIO_PDOR_PDO(0x3C);    // turn all LEDs OFF
    PTA->PDOR &= GPIO_PDOR_PDO(~SPK);   // Speaker off, beep_flag is false
}



void delay(long long unsigned int bound){
	for(long long unsigned int i = 0; i < bound; i++);
}

void RTCInit() {

    RTC_CR |= RTC_CR_SWR_MASK;  // SWR = 1, reset all RTC's registers
    RTC_CR &= ~RTC_CR_SWR_MASK; // SWR = 0

    RTC_TCR = 0x0000; // reset CIR and TCR

    RTC_CR |= RTC_CR_OSCE_MASK; // enable 32.768 kHz oscillator

    delay(0x600000);

    RTC_SR &= ~RTC_SR_TCE_MASK; // turn OFF RTC

    RTC_TSR = 0x00000000; // MIN value in 32bit register
    RTC_TAR = 0xFFFFFFFF; // MAX value in 32bit register

    RTC_IER |= RTC_IER_TAIE_MASK;

    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);

    RTC_SR |= RTC_SR_TCE_MASK; // turn ON RTC
}



void beep(){
	for(unsigned int i = 0; i < 500; i++){
		PTA->PDOR = GPIO_PDOR_PDO(0x0010);
		        delay(500);
		        PTA->PDOR = GPIO_PDOR_PDO(0x0000);
		        delay(500);
	}
}

void send_char(char c){
    while( !(UART5->S1 & UART_S1_TDRE_MASK) && !(UART5->S1 & UART_S1_TC_MASK) );
    UART5->D = c;
}


void send_str(char* s){
    unsigned int i = 0;
    while (s[i] != '\0'){
    	send_char(s[i++]);
    }
}

unsigned char receive_char(){
    while( !(UART5->S1 & UART_S1_RDRF_MASK) );
    return UART5->D;
}

void receive_str(){
    unsigned char c;
    unsigned int i = 0;

    memset(&buffer, 0, sizeof(buffer));

    while (i < (BUFFER_SIZE - 1) ) {
        c = receive_char();
        send_char(c);

        if(c == '\r'){
            break;
        }

        buffer[i] = c;
        i++;
    }

    buffer[i] = '\0';
    send_str("\r\n");
}

struct tm parse_time(){
	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	int ret = 0;
	while(1){
		receive_str();
		ret = sscanf(buffer, "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon , &tm.tm_mday, &tm.tm_hour, &tm.tm_min , &tm.tm_sec);
		if(ret != 6){
			send_str("Invalid input! \r\n");
			continue;
		}
		if(tm.tm_year < 1970 || 2038 < tm.tm_year){
			send_str("Invalid input! \r\n");
			continue;
		}
		if(tm.tm_mon < 1 || 12 < tm.tm_mon ){
			send_str("Invalid input! \r\n");
			continue;
		}
		if(tm.tm_mon==1 || tm.tm_mon==3 || tm.tm_mon==5 || tm.tm_mon==7 || tm.tm_mon==8 || tm.tm_mon==10 || tm.tm_mon==12){
			if(tm.tm_mday < 1 || 31 < tm.tm_mday){
				send_str("Invalid input! \r\n");
				continue;
			}
		 }
		 if(tm.tm_mon==4 || tm.tm_mon==6 || tm.tm_mon==9 || tm.tm_mon==11){
			 if(tm.tm_mday < 1 || 30 < tm.tm_mday){
				send_str("Invalid input! \r\n");
				continue;
			}
		 }
		 if(tm.tm_mon==2){
		         if( !(tm.tm_year % 4) ){
		             if(tm.tm_mday < 1 || 29 < tm.tm_mday){
		 				send_str("Invalid input! \r\n");
		 				continue;
		 			}
		         }
		         else{
		             if(tm.tm_mday < 1 || 28 < tm.tm_mday){
		 				send_str("Invalid input! \r\n");
		 				continue;
		 			}
		         }
		     }
		 break;
	}
	tm.tm_year  = tm.tm_year - 1900;
	tm.tm_mon   = tm.tm_mon  - 1;
	tm.tm_isdst = -1;
	return tm;
}



void set_new_time(struct tm *tm){
	*tm = parse_time();
	time_t time = mktime(tm);
	RTC_SR &= ~RTC_SR_TCE_MASK; // turn OFF RTC
	RTC_TSR = (long unsigned int)time;
	RTC_SR |= RTC_SR_TCE_MASK; // turn ON RTC
}

void lights(int l){
	switch(l){
	case 1:
		GPIOB_PDOR ^= LED_D10 | LED_D9 | LED_D11 | LED_D12;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		break;
	case 2:
		GPIOB_PDOR ^= LED_D9;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		GPIOB_PDOR ^= LED_D10 ;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		GPIOB_PDOR ^= LED_D11;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		GPIOB_PDOR ^= LED_D12;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		break;
	case 3:
		GPIOB_PDOR ^= LED_D10 | LED_D11;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		GPIOB_PDOR ^= LED_D9 | LED_D12;
		PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
		delay(300000);
		PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		break;
	}
}

void music(int s){
	switch(s){
	case 1:
		beep();
		delay(300000);
		break;
	case 2:
		beep();
		delay(100000);
		beep();
		delay(100000);
		beep();
		delay(100000);

		break;
	case 3:
		beep();
		delay(50000);
		beep();
		delay(50000);
		beep();
		delay(50000);
		beep();
		delay(50000);
		beep();
		delay(50000);
		break;
	}
}

int main(void)
{
	MCUInit();
	PortsInit();
	UART5Init();
	RTCInit();
	send_str("Enter current time in format YYYY-MM-DD HH:MM:SS\r\n");
	set_new_time(&t);
	time_t c_time = RTC_TSR;
	while(1){
		char buf[50];
		memset(&buf, 0, sizeof(buf));
		if(c_time != RTC_TSR){
			c_time = RTC_TSR;
			strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", localtime(&c_time));
			send_str(buf);
			send_str("\r\n");
		}
		if( !(GPIOE_PDIR & BTN_SW2) ){ // setup new time
			send_str("Enter current time in format YYYY-MM-DD HH:MM:SS\r\n");
			set_new_time(&t);
		}
		if( !(GPIOE_PDIR & BTN_SW3) ){ // setup alarm
			send_str("Enter alarm time in format YYYY-MM-DD HH:MM:SS\r\n");
			a = parse_time();
			alarm = (long unsigned int)mktime(&a);
			alarm_flag = 1;
			send_str("Enter number of postpones (max 9)\r\n");
			while(1){
				receive_str();
				if( !isdigit(buffer[0]) ){
					send_str("Invalid input! \r\n");
					continue;
				}
				postpones = atoi(buffer);
				if(postpones >= 0 && postpones < 10) break;
				send_str("Invalid input \r\n");
			}
			send_str("Enter seconds between postpones  (min 60, max 1000)\r\n");
			while(1){
				receive_str();
				postponed_delay = atoi(buffer);
				if(postponed_delay >= 60 && postponed_delay < 1001) break;
				send_str("Invalid input! \r\n");
			}

			send_str("Pick sound of buzzer 1-3 \r\n");
			while(1){
				receive_str();
				sound = atoi(buffer);
				if(sound > 0 && sound < 4) break;
				send_str("Invalid input! \r\n");
			}

			send_str("Pick light effect 1-3 \r\n");
			while(1){
				receive_str();
				light = atoi(buffer);
				if(light > 0 && light < 4) break;
				send_str("Invalid input! \r\n");
			}
		}

		if( !(GPIOE_PDIR & BTN_SW4) && alarm_flag ){ // turn off alarm
			alarm_flag = 0;
			sound = 0;
			light = 0;
			postpones = -1;
			postponed_delay = 0;
      send_str("Alarm is OFF\r\n");
		}
		if(alarm_flag){
			unsigned int tmp = RTC_TSR;
			if(alarm < tmp){ // is time for alarm
				lights(light);
				music(sound);

				if (!(GPIOE_PDIR & BTN_SW6) && postpones){
							alarm += postponed_delay;
							postpones--;
				}
				if(alarm+AUTO_POSTPONE < tmp){
					if(postpones){
						alarm += postponed_delay;
						postpones--;
					}else{
						sound = 0;
						light = 0;
						postpones = -1;
						postponed_delay = 0;
						alarm_flag = 0;
					}
				}
			}else{
				GPIOB_PDOR ^= LED_D9;
				PTB->PDOR &= ~GPIO_PDOR_PDO(0x1);
			}
		}else{
			PTB->PDOR |= GPIO_PDOR_PDO(0x3C);
		}
	}

    return 0;
}
