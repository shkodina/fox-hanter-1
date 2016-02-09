#include <avr/io.h>
#include <avr/delay.h>
#include "rf73_spi.h"


#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));



UINT8 rx_buf[MAX_PACKET_LEN];

#define COLORYELOWDDRPORT 	DDRA
#define COLORYELOWPORT 		PORTA
#define COLORYELOW 			0xF0

#define COLORGREENDDRPORT 	DDRC
#define COLORGREENPORT 		PORTC
#define COLORGREEN 			0xF0

#define COLORBLUEDDRPORT 	DDRC
#define COLORBLUEPORT 		PORTC
#define COLORBLUE			0x0F

#define ALLLEDON	0xFF
#define ALLLEDOFF	0x00

inline void shutDownLeds(){
	COLORYELOWPORT 		&= (~COLORYELOW);
	COLORGREENPORT 		&= (~COLORGREEN);
	COLORBLUEPORT 		&= (~COLORBLUE);
}

char fitPowerInByte(char power){
	return ( ( (power << 4) & 0xF0 ) | power );
}

#define POINTNUMBER 1
#define POINTPOWER 2

int main ()
{
	Init_Spi();
	RFM73_Initialize();
	SwitchToRxMode();

	COLORYELOWDDRPORT 	|= COLORYELOW;
	COLORYELOWPORT 		|= COLORYELOW;
	COLORGREENDDRPORT 	|= COLORGREEN;
	COLORGREENPORT 		|= COLORGREEN;
	COLORBLUEDDRPORT 	|= COLORBLUE;
	COLORBLUEPORT 		|= COLORBLUE;



_delay_ms(2000);

/*
showPower(2,1);
_delay_ms(2000);
showPower(3,2);
_delay_ms(2000);
showPower(4,3);
_delay_ms(2000);
showPower(5,4);
_delay_ms(2000);
*/

#define POWERCOUNT  4
#define NOSIGNAL 5
#define UNITCOUNT 6

	while (1)
	{
		char less_power[UNITCOUNT];
		for (char j=0; j<UNITCOUNT; j++) less_power[j] = NOSIGNAL;

		// get statistic of unit power 
		for(char i = 0; i < 96; i++){
			_delay_ms(10);

			UINT8 len = Receive_Packet(rx_buf, MAX_PACKET_LEN);
			if (len > 0){
				if (rx_buf[POINTPOWER] < less_power[rx_buf[POINTNUMBER]])
					less_power[rx_buf[POINTNUMBER]] = rx_buf[POINTPOWER];
			}
		}

		// show statistic
		shutDownLeds();

		for (char n=0; n < (UNITCOUNT / 2); n++){
			if (less_power[n*2] < less_power[n*2 + 1])
				less_power[n] = less_power[n*2];
			else
				less_power[n] = less_power[n*2 +1]; 
		}

		char power = fitPowerInByte(POWERCOUNT - less_power[0]);
		COLORYELOWPORT &= (power | (~COLORYELOW));

		power = fitPowerInByte(POWERCOUNT - less_power[1]);
		COLORGREENPORT &= (power | (~COLORGREEN));

		power = fitPowerInByte(POWERCOUNT - less_power[2]);
		COLORBLUEPORT &= (power | (~COLORBLUE));
	}

	return 0;

}


