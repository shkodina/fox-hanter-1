#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "rf73_spi.h"

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

#define UNITNUMBER 5 // change this from 0 to 5 [0..5]
#define FRAMEBORDER '*'
#define POWERPOSITION  2
#define POWERCOUNT  4

const UINT8 POWER[POWERCOUNT] = {0x00, 0x01, 0x02, 0x03};

UINT8 tx_buf[4] = {	FRAMEBORDER, UNITNUMBER, 0x00, FRAMEBORDER};

UINT8 rx_buf[MAX_PACKET_LEN];


int main ()
{

	Init_Spi();

	DDRC |= 0x01;
	PORTC |= 0x01;

	RFM73_Initialize();

	RFM73_SetPower(POWER[0]);
	SwitchToRxMode();

	DDRD = 0b00000000;
	PORTD = 0b00000100;


	GIMSK=0b01000000; // init int0 by interrupt
	MCUCR=0b00001000; // init int0 by 1->0


	
	while (1)
	{
		SwitchToTxMode();
		for(char i=0;i<POWERCOUNT;i++)
		{
			RFM73_SetPower(POWER[i]);
			tx_buf[POWERPOSITION] = POWER[i];

			SwitchToTxMode();
			Send_Packet(W_TX_PAYLOAD_NOACK_CMD,tx_buf,4);

			SwitchToRxMode();  //switch to Rx mode

			_delay_ms(150);
			INVBIT(PORTC, 0);
		}
		
	}

	return 0;

}

