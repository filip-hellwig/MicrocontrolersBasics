#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

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

/*
 *  Gotowe zaimplementowane:
 #define 	bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
 #define 	bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))
 #define 	loop_until_bit_is_set(sfr, bit)   do { } while (bit_is_clear(sfr, bit))
 #define 	loop_until_bit_is_clear(sfr, bit)   do { } while (bit_is_set(sfr, bit))

 */

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

volatile uint8_t minuty, sekundy;
volatile uint16_t liczba7Seg;

volatile char znaki[4];

char cyfra[10] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
		0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011 };

//Inicjalizacja Timer1 do wywolywania przerwania z czêstotliwoœci¹ 2Hz
void TimerInit() {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12);
	//Wybranie dzielnika czestotliwosci
	sbi(TCCR1B, CS12);
	//Zapisanie do OCR1A wartosci odpowiadajacej 0,5s
	OCR1A = 15625;
	/*OCR1A = 31250;*/
	//Uruchomienie przerwania OCIE1A
	sbi(TIMSK, OCIE1A);
}

//Inicjalizacja portow do obs³ugi wyswietlacza 7 segmentowego
void seg7Init() {
	//Inicjalizacja kolumn
	DDRC = 0xff;
	//Inicjalizacja segmentu
	PORTC = 0x00;
}

//Wyswietla na wyswietlaczu 7 segmentowym cyfre z argumentu
void seg7ShowCyfra(uint8_t cyfraDoWyswietlenia) {
	PORTC = cyfra[cyfraDoWyswietlenia];

	/*
	 * Funkcja ta sluzy do ustawiania pinu wykorzystywanego do zapalania diody odpowiedniego segmentu na stan wysoki lub zostawianiu go na stanie niskim.
	 * Kazda cyfra ma przypisany inne watosci binarne, gdyz kazda potrzebuje zapalic inne segmenty.
	 */

}


int main() {

	TimerInit();
	seg7Init();
	sei(); //funkcja uruchamia globalne przerwania


	//Zadanie 1


	/*
	 * Przerwania - jest to mechanizm ktory pozwala mikrokontrolerowi przeywac bierzace zadania w przypadku zarejestrowania jakiegos zdarzenia.
	 * Przerwania sa uzyteczne gdyz zwalniaja procesor z ciaglego szukania zdarzen. Mikrokontroler wylacznie biernie oczekuje informacji.
	 * Zrodlem przerwan moga byc: licznik, modul komunikacji, przycisk, enkoder. W naszym programie przerwanie dziala na zasadzie licznika, czyli petli która
	 * inkrementuje sie az do pewnej zadanej wartosci. W momencie osiagniecia tej wartosci petla zeruje sie i nastepuje przerwanie. Przerwania moga byc
	 * wykorzystane jako funkcje awaryjne, zegar.
	 */

	 /*sbi(DDRB, PB0);

	 while(1)
	 {

	 }*/



	//Zadanie 2

	/*
	 * Jezeli nie korzystamy z diody dp, to wykorzystujemy 7 linii mikrokontrolera.
	 */


	 while (1) {
		for (uint8_t i = 0; i < 10; i++) {
			seg7ShowCyfra(i);
			_delay_ms(1000);
		}
	 }




	//NIE DZIALA
	//Zadanie 3
	/*TimerInit();
	seg7Init();
	sei(); //funkcja uruchamia globalne przerwania

	cbi(DDRD, PD0);
	sbi(PORTD, PD0);
	cbi(DDRA, PA0);

	while(1)
	{
		while(1)
		{
			if(bit_is_clear(PIND, PD0))
			{
				break;
			}
		}

		for (uint8_t i = 9; i > -1; i--) {
			while(1)
			{
				if (bit_is_clear(PINA, PA0))
				{
					break;
				}
			}
			seg7ShowCyfra(i);
			_delay_ms(500);
		}
		sbi(PORTB, PB0);
		_delay_ms(5000);
		cbi(PORTB, PB0);

	}*/


	return 0;
}

//Funkcja uruchamiana z przerwaniem po przepelnieniu licznika w timer1
ISR(TIMER1_COMPA_vect) {
	tbi(PORTB, PB0);
}
