#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "HD44780.h"


#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif

#ifndef inb
#define	inb(addr)			(addr)
#endif

#ifndef outb
#define	outb(addr, data)	addr = (data)
#endif

#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#ifndef tbi
#define tbi(reg,bit)		reg ^= (_BV(bit))
#endif


 /*Gotowe zaimplementowane:*/
 #define 	bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
 #define 	bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))
 #define 	loop_until_bit_is_set(sfr, bit)   do { } while (bit_is_clear(sfr, bit))
 #define 	loop_until_bit_is_clear(sfr, bit)   do { } while (bit_is_set(sfr, bit))



// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

void ADC_init(){
	//Konfiguracja napiecia referencyjnego
	sbi(ADMUX, REFS0);
	cbi(ADMUX, REFS1);

	//Konfiguracja podzielnika czestotliwosci
	sbi(ADCSRA, ADPS0);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS2);

	//Uruchomienie ukladu przetworinika
	sbi(ADCSRA, ADEN);

	//Wybor pinu do wykonywania pomiaru
	cbi(ADMUX, MUX0);
	cbi(ADMUX, MUX1);
	cbi(ADMUX, MUX2);
	cbi(ADMUX, MUX3);
	cbi(ADMUX, MUX4);
}

uint16_t ADC_10bit(){

	char text[20];

	//Uruchomienie pojedynczego pomiaru
	sbi(ADCSRA, ADSC);

	//Oczekiwanie na zakonczenie pomiaru
	loop_until_bit_is_clear(ADCSRA, ADSC);

	//Wynik pomiaru
	uint16_t pomiar = ADC;

	/*LCD_GoTo(0, 0);
	sprintf(text, "%d", pomiar);
	LCD_WriteText(text);*/

	return pomiar;
}

uint16_t ADC_measure(int input)
{
	float temp = 0;
	uint16_t result = 0;

	//Poniewaz wartosci zwracane przez ADC_10bit sa od 0 do 1023, a spodziewane napiecie jest od 0 do 5V,
	//to tworzymy konwersje na volty.
	temp = 0.00489*input;

	//Zamiana na liczby calkowite miedzy 0 a 500.
	temp = floor(100*temp)/100;
	result = temp*100;

	return result;
}


int main() {
	LCD_Initalize();
	LCD_Home();
	LCD_Clear();
	ADC_init();

	char text[20];


	//Zadanie 3 i 4
	/*while(1)
	{
		LCD_Clear();

		//Uzyskujemy wynik i przeliczamy do wyswietlania
		int temp = ADC_measure(ADC_10bit());
		int temp2 = temp/100;
		temp = temp%100;

		if ( temp < 10 )	//Jezeli jest mniejszy od 10 to wyswietlamy z 0 po przecinku
		{
			LCD_GoTo(0, 1);
			sprintf(text, "%d.0%d", temp2, temp);
			LCD_WriteText(text);
			_delay_ms(100);
		} else
		{
			LCD_GoTo(0, 1);
			sprintf(text, "%d.%d", temp2, temp);
			LCD_WriteText(text);
			_delay_ms(100);
		}

	}*/

	//Komparator to uklad sluzacy do porownywania dwoch napiec (jest to komparator analogowy).
	//W naszym przykladzie zwraca 1 w przypadku wlaczenia klimatyzacji (przekroczenie 2,5V),
	//lub 0 w przypadku wylaczenia klimatyzacji



	//Zadanie 4,5
	sbi(DDRC, PC3);
	char text1[20];
	while(1)
	{
		LCD_Clear();

		//Uzyskujemy wynik i przeliczamy do wyswietlania
		int temp = ADC_measure(ADC_10bit());
		int temp2 = temp/100;
		int temp3 = temp%100;

		if ( temp3 < 10 ) //Jezeli jest mniejszy od 10 to wyswietlamy z 0 po przecinku
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d.0%d", temp2, temp3);
			LCD_WriteText(text);
		} else
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d.%d", temp2, temp3);
			LCD_WriteText(text);
		}

		//Realizacja komparatora, jezeli wartosc napiecia jest wieksza od 2,5V
		//to klimatyzacja wlacza sie, zaswieca sie dioda oraz widac napis On.
		if (temp > 250)
		{
			sbi(PORTC,PC3);
			LCD_GoTo(0, 1);
			sprintf(text1, "On");
			LCD_WriteText(text1);
		} else
		{
			cbi(PORTC,PC3);
			LCD_GoTo(0, 1);
			sprintf(text1, "Off");
			LCD_WriteText(text1);
		}
		_delay_ms(100);

	}


}
