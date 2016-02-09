#include <avr/io.h>
#include <avr/delay.h>
#include "rf73_spi.h"


#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));



UINT8 rx_buf[MAX_PACKET_LEN];

#define COLORDDRPORT1 	DDRB
#define COLORPORT1 		PORTB
#define COLORDDRPORT2 	DDRA
#define COLORPORT2 		PORTA
#define COLORRED 	0x01
#define COLORGREEN 	0x01
#define COLORBLUE	0x02

#define LEDPORT 	PORTC
#define LEDDDRPORT	DDRC
#define ALLLEDON	0xFF
#define ALLLEDOFF	0x00

inline void shutDownLeds(){
	COLORPORT1 = COLORPORT1 & (~(COLORRED));
	COLORPORT1 = COLORPORT1 & (~(COLORBLUE));
	COLORPORT2 = COLORPORT2 & (~(COLORGREEN));

	LEDPORT = ALLLEDOFF;
}


inline void showPower(char num, char power){

	if (!num){
		shutDownLeds();
		return;
	}


	// set COLOR
	switch (num){
		case 1:
		case 4:
		{
			COLORPORT1 |= COLORRED;
			break;
		}
		case 2:
		case 5:
		{
			COLORPORT2 |= COLORGREEN;
			break;
		}
		case 3:
		case 6:
		{
			COLORPORT1 |= COLORBLUE;
			break;
		}
		default:
		{
			COLORPORT1 |= (COLORRED | COLORBLUE);
			COLORPORT2 |= COLORGREEN;
			break;
		}
	}	
	


	// set POWER
	if (num <= 3)
		for (char i=0; i<power; i++)
			UPBIT(LEDPORT, i);	

	if (num >= 4)
		for (char i=0; i<power; i++)
			UPBIT(LEDPORT, i+4);	

}

#define POINTNUMBER 1
#define POINTPOWER 2


inline void wowImAlive(){

	static char point = 4;

	if(point++ >= 4)
		point = 0;

	switch (point){
		case 0:
		{
			COLORPORT1 |= COLORRED;
			break;				
		}
		case 1:
		{
			COLORPORT2 |= COLORGREEN;
			break;				
		}
		case 2:
		{
			COLORPORT1 |= COLORBLUE;
			break;				
		}
		case 3:
		{
//			COLORPORT1 |= (COLORRED | COLORBLUE);
			COLORPORT2 |= COLORGREEN;
			break;				
		}
		default:
			break;
	}

	UPBIT(LEDPORT, point);
	UPBIT(LEDPORT, point+4);
}


int main ()
{
	Init_Spi();
	RFM73_Initialize();
	SwitchToRxMode();

	COLORDDRPORT1 |= (COLORRED | COLORBLUE);
	COLORPORT1 |= (COLORRED | COLORBLUE);
	COLORDDRPORT2 |= COLORGREEN;
	COLORPORT2 |= COLORGREEN;


	LEDDDRPORT = ALLLEDON;
	LEDPORT = ALLLEDOFF;


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



	while (1)
	{
		char showed_point = 0;
		char less_power[6];
		for (char j=0; j<6; j++) less_power[j] = 5;



		// get statistic 
		for(char i = 0; i < 96; i++){
			_delay_ms(10);

			UINT8 len = Receive_Packet(rx_buf, MAX_PACKET_LEN);
			if (len > 0){
				if (rx_buf[POINTPOWER] < less_power[rx_buf[POINTNUMBER]])
					less_power[rx_buf[POINTNUMBER]] = rx_buf[POINTPOWER];
			}
		}

		showPower(ALLLEDOFF, ALLLEDOFF);

		for (char n=0; n<6; n++){
			if (less_power[n] != 5){
				showPower(n+1, 4-less_power[n]);
				showed_point++;
			}
		}

		if(!showed_point)
			wowImAlive();
	}

	return 0;

}


