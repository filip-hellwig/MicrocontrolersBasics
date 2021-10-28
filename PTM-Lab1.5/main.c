#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
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


 #define 	bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
 #define 	bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))
 #define 	loop_until_bit_is_set(sfr, bit)   do { } while (bit_is_clear(sfr, bit))
 #define 	loop_until_bit_is_clear(sfr, bit)   do { } while (bit_is_set(sfr, bit))


// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))



// Cyfry od 0 do 9 dla wyswietlacza 7-seg
char cyfra[10] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
		0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011 };

// - wyswietlacza 7-seg
char kreska = 0b0000001;

// wylaczanie wyswietlacza 7-seg
char nothing = 0b0000000;

//Inicjalizacja portow do obslugi wyswietlacza 7 segmentowego
void seg7Init() {
	//Inicjalizacja kolumn
	DDRC = 0xff;
	//Inicjalizacja segmentu
	PORTC = 0x00;
}

//Wyswietla na wyswietlaczu 7 segmentowym cyfre z argumentu
void seg7ShowCyfra(uint8_t cyfraDoWyswietlenia) {
	if(cyfraDoWyswietlenia > -1 && cyfraDoWyswietlenia < 10)
	{
		PORTC = cyfra[cyfraDoWyswietlenia];
	} else if(cyfraDoWyswietlenia == 99)
	{
		PORTC = nothing;
	} else
	{
		PORTC = kreska;
	}

	/*
	 * Funkcja ta sluzy do ustawiania pinu wykorzystywanego do zapalania diody odpowiedniego segmentu na stan wysoki lub zostawianiu go na stanie niskim.
	 * Kazda cyfra ma przypisany inne watosci binarne, gdyz kazda potrzebuje zapalic inne segmenty.
	 */

}



// Funkcja wyswietlajaca ekran startowy
void start()
{
	LCD_Clear();			// czyscimy wyswietlacz

	char index[20];			// nasze indeksy do wyswietlania
	char text[20];			// tekst powitalny

	LCD_GoTo(0, 0);			// idziemy do pierwszej linijki wyswietlacza
	sprintf(text, "PROGRAM PTM 2021");			// tworzymy stringa
	LCD_WriteText(text);

	LCD_GoTo(0, 1);			// idziemy do drugiej linijki wyswietlacza
	sprintf(index, "%s, %s", "252904", "252915");			// tworzymy stringa
	LCD_WriteText(index);			// wyswietlamy

	_delay_ms(4000);

	LCD_Clear();
}



// Funkcja ustalajaca wcisniecie klawisza
uint8_t getKey()
{
	// ustawiam stan wysoki dla pinow PD3 do PD6
	// ustawiam PULL-UP dla pina PD7
	sbi(DDRD, PD3);
	sbi(DDRD, PD4);
	sbi(DDRD, PD5);
	sbi(DDRD, PD6);
	cbi(DDRD, PD7);

	sbi(PORTD, PD3);
	sbi(PORTD, PD4);
	sbi(PORTD, PD5);
	sbi(PORTD, PD6);
	sbi(PORTD, PD7);

	uint16_t delay = 200;		// delay do klawiszy
	uint8_t flag = -1;			// zmienna pomocnicza

	// sprawdzamy ktory przycisk jest wcisnienty
	// podajemy stan niski i szukamy stanu niskiego, dzieki temu wiemy ze przycisk zostal wcisniety
	for(int j = 3; j < 7; j++)
	{
		cbi(PORTD, j);
		if(bit_is_clear(PIND, PD7))
		{
			_delay_ms(delay);
			flag = j-3;
		}
		sbi(PORTD, j);
	}

	return flag;			// zwracamy flage
}

//Funkcja pobiera tylko przycisk x.
uint8_t getKeyX()
{
	// ustawiam stan wysoki dla pina PD5
	// ustawiam PULL-UP dla pina PD7
	sbi(DDRD, PD5);
	cbi(DDRD, PD7);

	cbi(PORTD, PD5);
	sbi(PORTD, PD7);

	uint16_t delay = 200;		// delay do klawiszy
	uint8_t flag = -1;			// zmienna pomocnicza

	// sprawdzamy ktory przycisk jest wcisnienty
	// podajemy stan niski i szukamy stanu niskiego, dzieki temu wiemy ze przycisk zostal wcisniety

	if(bit_is_clear(PIND, PD7))
	{
		_delay_ms(delay);
		flag = 2;
	}

	return flag;			// zwracamy flage
}




// Funkcja inicializujaca rejestr ADC
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

// Funkcja zczytujca wartosc z rejestru ADC
uint16_t ADC_10bit(){

	char text[20];

	//Uruchomienie pojedynczego pomiaru
	sbi(ADCSRA, ADSC);

	//Oczekiwanie na zakonczenie pomiaru
	loop_until_bit_is_clear(ADCSRA, ADSC);

	//Wynik pomiaru
	uint16_t pomiar = ADC;

	LCD_GoTo(0, 0);
	sprintf(text, "%d", pomiar);
	LCD_WriteText(text);

	return pomiar;
}

// Funkcja przeksztalcajaca wartosci z rejestru na volty
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




//Inicjalizacja Timer1 do wywolywania przerwania z czêstotliwoœci¹ 2Hz
void TimerInit() {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12);
	//Wybranie dzielnika czestotliwosci
	sbi(TCCR1B, CS12);
	//Zapisanie do OCR1A wartosci odpowiadajacej 0,5s
	/*OCR1A = 15625;*/
	OCR1A = 31250;
	//Uruchomienie przerwania OCIE1A
	sbi(TIMSK, OCIE1A);
}

//Funkcja uruchamiana z przerwaniem po przepelnieniu licznika w timer1
ISR(TIMER1_COMPA_vect) {
	tbi(PORTB, PB7);
}



void info() { start(); }

void liczby()
{
	uint8_t key = -1;
	int liczba = 0;
	char text[20];
	uint8_t pierwsza[15] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47}; // liczby pierwsze

	sbi(DDRD, PD0);
	sbi(DDRD, PD1);
	sbi(DDRD, PD2);

	LCD_Clear();

	// Wyswietlamy odpowiednia liczbe od 0 do 50
	LCD_GoTo(0, 0);
	sprintf(text, "%d", liczba);
	LCD_WriteText(text);

	while(key != 2)
	{
		cbi(PORTD, PD0);
		cbi(PORTD, PD1);
		cbi(PORTD, PD2);

		// Wyswietlamy liczbe na 7-seg
		seg7ShowCyfra(liczba);

		// Uruchamianie odpowiednich diod dla odpowiednich liczb
		if(liczba%2 == 0)
		{
			sbi(PORTD, PD0);
		} else
		{
			sbi(PORTD, PD1);
		}
		for(int i = 0; i < 15; i++)
		{
			if(liczba == pierwsza[i])
			{
				sbi(PORTD, PD2);
			}
		}

		// Zwiekszamy lub zmiejszamy liczbe na liczniku
		key = getKey();
		if(key == 0)
		{
			liczba++;
			liczba = liczba%51;

			LCD_Clear();

			// Wyswietlamy odpowiednia liczbe od 0 do 50
			LCD_GoTo(0, 0);
			sprintf(text, "%d", liczba);
			LCD_WriteText(text);

		} else if (key == 1)
		{
			liczba--;
			if(liczba == -1)
			{ liczba = 50; }

			LCD_Clear();

			LCD_GoTo(0, 0);			// idziemy do drugiej linijki wyswietlacza
			sprintf(text, "%d", liczba);			// tworzymy stringa
			LCD_WriteText(text);
		}
	}

	// Czyscimy ekran i diody po zakonczeniu funkcji
	cbi(PORTD, PD0);
	cbi(PORTD, PD1);
	cbi(PORTD, PD2);
	seg7ShowCyfra(99);

}


void stoper() {}

void zegar()
{
	sei();		// Uruchamianie przerwan

	uint8_t m = 59;			// ilosc minut
	uint8_t s = 20;			// ilosc sekund
	uint8_t end = 0;
	char text[20];			// godzina zapisana do wyswietlania

	LCD_Clear();			// czyscimy wyswietlacz

	sbi(DDRB, PB7);
	cbi(PORTB, PB7);
	sbi(DDRD, PD2);

	while(1)
	{
		// Sprawdzamy ktore jednostki czasowe sa jednocyfrowe, a ktore dwucyfrowe.
		// Jest nam to potrzebne do poprawnego wyswietlania zegara.
		if(s < 10 && m < 10)
		{
			LCD_GoTo(0, 0);				// piszemy w pierwszej linijce wyswietlacza
			sprintf(text, "0%d:0%d", m,s);			// dodajemy zera bo wszystkie sa jednocyfrowe
			LCD_WriteText(text);			// wyswietlamy tekst
		}
		else if(s < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:0%d", m,s);
			LCD_WriteText(text);
		} else if(m < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "0%d:%d", m,s);
			LCD_WriteText(text);
		} else
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:%d", m,s);
			LCD_WriteText(text);
		}
		seg7ShowCyfra(s%10);

		s = ++s%60;			// zwiekszamy liczbe sekund o 1 z kazda iteracja while'a

		if(s == 0)			// jezeli s sie zeruje to wzrasta liczba minut
		{
			m = ++m%60;
		}

		// Wlaczanie diody na 200ms
		sbi(PORTD, PD2);
		_delay_ms(200);
		cbi(PORTD, PD2);

		// Setujemy przycisk po jednym przejsciu
		if(bit_is_clear(PINB, PB7))
		{
			tbi(PORTB, PB7);
		}

		// Sprawdzamy czy nastapilo przerwanie
		while(bit_is_clear(PINB, PB7) != 1)
		{
			// Sprawdzamy czy wychodzimy z programu
			if(getKeyX() == 2)
			{
				end = 1;
				break;
			}
		}

		if(end) { break; }
	}

	// Czyscimy ekrany
	LCD_Clear();
	seg7ShowCyfra(99);
}

void miernik()
{
	char text[20];

	while(getKeyX() != 2)
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

	}
	LCD_Clear();
}


int main()
{
	// Inicjalizacje elementow ukladu
	seg7Init();
	LCD_Initalize();
	LCD_Home();
	ADC_init();
	TimerInit();

	// Ekran powitalny
	start();

	uint8_t licznik = 0;		// licznik do obslugi menu
	uint8_t key = 0;		// ktory klawisz wcisniety
	char text[20];

	while(1)
	{
		// Oczekujemy az uzytkownik wcisnie jakis klawisz
		while(1)
		{
			key = -1;
			key = getKey();
			if(key != -1)
			{
				if( key == 0 || key == 1)
				{
					LCD_Clear();
				}
				break;
			}
		}

		// Obsluga klawiszy Up i Down
		if(key == 0)
		{
			licznik++;
		} else if (key == 1)
		{
			licznik--;
		}

		// Wybor odpowiedniej opcji z menu
		switch(licznik%5)
		{
			case 0:
				LCD_GoTo(0, 0);			// idziemy do drugiej linijki wyswietlacza
				sprintf(text, "INFO");			// tworzymy stringa
				LCD_WriteText(text);

				if(key == 3)
				{
					info();
					LCD_Clear();
				}
				break;
			case 1:
				LCD_GoTo(0, 0);			// idziemy do pierwszej linijki wyswietlacza
				sprintf(text, "LICZBY");			// tworzymy stringa
				LCD_WriteText(text);

				if(key == 3)
				{
					liczby();
					LCD_Clear();
				}
				break;
			case 2:
				LCD_GoTo(0, 0);
				sprintf(text, "STOPER");
				LCD_WriteText(text);

				if(key == 3)
				{
					stoper();
					LCD_Clear();
				}
				break;
			case 3:
				LCD_GoTo(0, 0);
				sprintf(text, "ZEGAR");
				LCD_WriteText(text);

				if(key == 3)
				{
					zegar();
					LCD_Clear();
				}
				break;
			case 4:
				LCD_GoTo(0, 0);
				sprintf(text, "MIERNIK");
				LCD_WriteText(text);

				if(key == 3)
				{
					miernik();
					LCD_Clear();
				}
				break;
		}
	}
}
