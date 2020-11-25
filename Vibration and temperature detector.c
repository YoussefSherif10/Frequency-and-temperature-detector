/*
 * Embedded_project.c
 *
 * Created: 6/14/2020 1:36:18 PM
 *  Author: Yossef
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define VIPRATIONCHANNEL	0x00
#define TEMPCHANNEL			0x01

// Function prototypes
void ADCInit(void);
unsigned short ADCRead(void);
void ADC_Select(unsigned char);
// Global Variables
unsigned short samples[200];
unsigned int zeroCrossing = 0;
unsigned int index = 0; 
unsigned char counter = 0;
unsigned char temp = 0;
int main(void)
{
	DDRA &= ~0x03;
	DDRC |= 0xC3;
	ADCInit();
    while(1)
    {
		ADC_Select(TEMPCHANNEL);
        temp = ((ADCRead() * 0.5) - 2);
		if(temp < 80)
		{
			PORTC &= ~0x40; //Red off
			PORTC |= 0x80; //Green on
		}
		else
		{
			PORTC &= ~0x80; //Green off
			PORTC |= 0x40;	// Red on
		}
			
		ADC_Select(VIPRATIONCHANNEL);
		samples[index] = ADCRead();
		index++;
		_delay_ms(5);
		if(index == 200)
		{
			for(counter = 0; counter < index; counter++)
			{
				if(samples[counter] >= 532 && samples[counter + 1] <= 532)
				{	
					zeroCrossing++;
				}
			}
			if(zeroCrossing < 10)
			{
				PORTC &= ~0x01;	// Red off
				PORTC |= 0x02; //Green on
			}
			else
			{
				PORTC &= ~0x02; // Green off
				PORTC |= 0x01;	// Red on
			}
			index = 0;
			zeroCrossing = 0;
		}			
						
    }
}




void ADCInit(void)
{
	ADMUX = 0x00;
	ADMUX |= (1 << 6); // AVCC with external cap at AREF
	ADCSRA = 0x00;
	ADCSRA |= (1 << 2) | (1 << 1) | (1 << 0); // prescaler = clock /128
}

void ADC_Select(unsigned char channel)
{
	ADCSRA &= ~(1 << 7); // ADC Disable
	ADMUX = (ADMUX & 0xF8) | channel; // choose channel;
	ADCSRA |= (1 << 7); // ADC enable
}

unsigned short ADCRead(void)
{
	unsigned short dValue;
	ADCSRA |= (1 << 6); // Start Conversion	
	while(ADCSRA & (1 << ADSC));
	dValue = ADCL;
	dValue |= (ADCH << 8);
	return dValue;
}
