#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "HD44780.h"

#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

uint8_t getKey()
{
	char text[20];		// zmienna przechowujaca tekst do wyswietlania

	// ustawiam stan wysoki dla pinow PD4 do PD7
	// ustawiam PULL-UP dla pinow PD0 do PD3
	sbi(DDRD, PD4);
	sbi(DDRD, PD5);
	sbi(DDRD, PD6);
	sbi(DDRD, PD7);
	cbi(DDRD, PD0);
	cbi(DDRD, PD1);
	cbi(DDRD, PD2);
	cbi(DDRD, PD3);
	PORTD = 0xff;

	uint16_t delay = 50;		// delay do klawiszy
	uint8_t flag = 0;			// zmienna pomocnicza


	LCD_GoTo(0, 1);			// wyswietlamy na drugim wierszu
	LCD_Clear();			// czyszczenie wyswietlacza

	// sprawdzamy ktory przycisk jest wcisnienty
	// podajemy stan niski i szukamy stanu niskiego, dzieki temu wiemy ze przycisk zostal wcisniety
	for(int i = 1, j = 7; i < 5; i++, j--)
	{
		cbi(PORTD, j);			// 0 na PORTD
		if(bit_is_clear(PIND,PD0))
		{
			sprintf(text, "Wcisnieto: %d", i);		// tworzymy odpowiedni tekst
			LCD_WriteText(text);			// wyswietlamy tekst
			_delay_ms(delay);			// delay
			flag = i;			// ustawiamy flage na liczbe odpowiadajaca przyciskowi
		}
		else if(bit_is_clear(PIND,PD1))
				{
			sprintf(text, "Wcisnieto: %d", i+4);
			LCD_WriteText(text);
			_delay_ms(delay);
			flag = i+4;
		}
		else if(bit_is_clear(PIND,PD2))
		{
			sprintf(text, "Wcisnieto: %d", i+8);
			LCD_WriteText(text);
			_delay_ms(delay);
			flag = i+8;
		}
		else if(bit_is_clear(PIND,PD3))
		{
			sprintf(text, "Wcisnieto: %d", i+12);
			LCD_WriteText(text);
			_delay_ms(delay);
			flag = i+12;
		}
		sbi(PORTD, j);
	}
	return flag;			// zwracamy flage
}

uint8_t getKey2()
{
	// ustawiam stan wysoki dla pinow PD4 do PD7
	// ustawiam PULL-UP dla pinow PD0 do PD3
	sbi(DDRD, PD4);
	sbi(DDRD, PD5);
	sbi(DDRD, PD6);
	sbi(DDRD, PD7);
	cbi(DDRD, PD0);
	cbi(DDRD, PD1);
	cbi(DDRD, PD2);
	cbi(DDRD, PD3);
	PORTD = 0xff;

	uint16_t delay = 200;		// delay do klawiszy
	uint8_t flag = 0;			// zmienna pomocnicza

	// sprawdzamy ktory przycisk jest wcisnienty
	// podajemy stan niski i szukamy stanu niskiego, dzieki temu wiemy ze przycisk zostal wcisniety
	for(int i = 1, j = 7; i < 5; i++, j--)
	{
		cbi(PORTD, j);
		if(bit_is_clear(PIND,PD0))
		{
			_delay_ms(delay);
			flag = i;
		}
		else if(bit_is_clear(PIND,PD1))
				{
			_delay_ms(delay);
			flag = i+4;
		}
		else if(bit_is_clear(PIND,PD2))
		{
			_delay_ms(delay);
			flag = i+8;
		}
		else if(bit_is_clear(PIND,PD3))
		{
			_delay_ms(delay);
			flag = i+12;
		}
		sbi(PORTD, j);
	}
	return flag;			// zwracamy flage
}



int main() {
	LCD_Initalize();
	LCD_Home();


	//Zadanie 1
	/*uint8_t h = 23;			// ilosc godzin
	uint8_t m = 59;			// ilosc minut
	uint8_t s = 20;			// ilosc sekund
	char index[20];			// nasze indeksy do wyswietlania
	char text[20];			// godzina zapisana do wyswietlania

	while (1) {
		LCD_Clear();			// czyscimy wyswietlacz


		// Sprawdzamy ktore jednostki czasowe sa jednocyfrowe, a ktore dwucyfrowe.
		// Jest nam to potrzebne do poprawnego wyswietlania zegara.
		if(s < 10 && m < 10 && h < 10)
		{
			LCD_GoTo(0, 0);				// piszemy w pierwszej linijce wyswietlacza
			sprintf(text, "0%d:0%d:0%d", h,m,s);			// dodajemy zera bo wszystkie sa jednocyfrowe
			LCD_WriteText(text);			// wyswietlamy tekst
		} else if(s < 10 && m < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:0%d:0%d", h,m,s);
			LCD_WriteText(text);
		} else if(s < 10 && h < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "0%d:%d:0%d", h,m,s);
			LCD_WriteText(text);
		} else if(m < 10 && h < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "0%d:0%d:%d", h,m,s);
			LCD_WriteText(text);
		} else if(h < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "0%d:%d:%d", h,m,s);
			LCD_WriteText(text);
		} else if(m < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:0%d:%d", h,m,s);
			LCD_WriteText(text);
		} else if(s < 10)
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:%d:0%d", h,m,s);
			LCD_WriteText(text);
		} else
		{
			LCD_GoTo(0, 0);
			sprintf(text, "%d:%d:%d", h,m,s);
			LCD_WriteText(text);
		}


		s = ++s%60;			// zwiekszamy liczbe sekund o 1 z kazda iteracja whila

		if(s == 0)			// jezeli s sie zeruje to wzrasta liczba minut
		{
			m = ++m%60;

			if(m == 0)			// jezeli m sie zeruje to wzrasta liczba godzin
			{
				h = ++h%24;
			}
		}





		LCD_GoTo(0, 1);			// idziemy do drugiej linijki wyswietlacza
		sprintf(index, "%s, %s", "252904", "252915");			// tworzymy stringa
		LCD_WriteText(index);			// wyswietlamy

		_delay_ms(1000);			// delay do odmierzania czasuS
	}*/



	//Zadanie 2
	/*uint8_t key;
	char text[3];

	while(1)
	{
		key = getKey();

		if(key == 0)
		{
			sprintf(text, "-1");
			LCD_WriteText(text);
			_delay_ms(50);
		}
	}*/



	//Zadanie 3
	uint8_t key;			// Zmienna przechowujaca ostatni wcisniety klawisz
	uint8_t flag1 = 0;		// Pomocnicze zmienne
	uint8_t flag2 = 0;
	uint8_t flag3 = 0;
	char text[3];
	uint8_t numTab[4] = {0};			// Tablica przechowujaca cyfry do utworzenia czterocyfrowej liczby
	uint16_t num1 = -1, num2 = -1;		// Utworzone liczby do wyrazenia
	char oper;		// Zmienna przechowujaca operator dodawania lub odejmowania


	// Petla dzialajaca nieprzerwanie az do wylaczenia symulacji
	while(1)
	{
		LCD_Clear();
		// Petla uzupelniajaca tablice numTab cyframi z klawiatury, oraz podstawiajaca + lub - pod oper
		while(1)
		{
			key = getKey2();	// Funkcja podstawiajaca wcisniety klawisz pod key
			LCD_GoTo(flag1, 0);

			if(key == 14) // Sprawdzanie ktory klawisz jest wcisniety
			{
				sprintf(text, "0");
				LCD_WriteText(text);
				numTab[flag1++] = 0; // Podstawienie odpowiedniej cyfry pod element tablicy oraz postinkrementacja indeksu
			} else if(key == 9)		 // Analogicznie dla reszty klawiszy
			{
				sprintf(text, "1");
				LCD_WriteText(text);
				numTab[flag1++] = 1;
			} else if(key == 10)
			{
				sprintf(text, "2");
				LCD_WriteText(text);
				numTab[flag1++] = 2;

			} else if(key == 11)
			{
				sprintf(text, "3");
				LCD_WriteText(text);
				numTab[flag1++] = 3;
			} else if(key == 5)
			{
				sprintf(text, "4");
				LCD_WriteText(text);
				numTab[flag1++] = 4;
			} else if(key == 6)
			{
				sprintf(text, "5");
				LCD_WriteText(text);
				numTab[flag1++] = 5;
			} else if(key == 7)
			{
				sprintf(text, "6");
				LCD_WriteText(text);
				numTab[flag1++] = 6;
			} else if(key == 1)
			{
				sprintf(text, "7");
				LCD_WriteText(text);
				numTab[flag1++] = 7;
			} else if(key == 2)
			{
				sprintf(text, "8");
				LCD_WriteText(text);
				numTab[flag1++] = 8;
			} else if(key == 3)
			{
				sprintf(text, "9");
				LCD_WriteText(text);
				numTab[flag1++] = 9;
			} else if(key == 8)		// Sprawdzenie czy wcisniety zostal operator +
			{
				sprintf(text, "+");
				LCD_WriteText(text);
				oper = '+';			// Podstawienie wcisnietego klawisza pod oper
				break;				// Wyjscie z pierwszego while
			} else if(key == 4)		// Analogicznie dla operatora -
			{
				sprintf(text, "-");
				LCD_WriteText(text);
				oper = '-';
				break;
			}

			// W przypadku wypelnienia tablicy numTab czterema cyframi mozna wybrac juz tylko operator
			if(flag1 == 4)
			{
				while(1)
				{
					key = getKey2();

					if(key == 8)
					{
						sprintf(text, "+");
						LCD_WriteText(text);
						oper = '+';
						break;
					} else if(key == 4)
					{
						sprintf(text, "-");
						LCD_WriteText(text);
						oper = '-';
						break;
					}
				}
				break;
			}
		}

		// Tworzenie pierwszej liczby na podstawie cyfr w tablicy
		// oraz ilosci wpisanych cyfr
		if(flag1 == 1){
			num1 = numTab[0];
		} else if(flag1 == 2){
			num1 = numTab[0]*10 + numTab[1];
		} else if(flag1 == 3){
			num1 = numTab[0]*100 + numTab[1]*10 + numTab[2];
		} else if(flag1 == 4){
			num1 = numTab[0]*1000 + numTab[1]*100 + numTab[2]*10 + numTab[3];
		}

		memset(numTab, 0, sizeof numTab);	// Wyzerowanie tablicy dla drugiej liczby
		flag3 = flag1;

		while(1)
		{
			key = getKey2();
			LCD_GoTo(flag2+flag3+1, 0);		// Wyswietlanie kolejnych cyfr za operatorem

			// Analogiczne dzialanie jak w pierwszym while
			// ale bez sprawdzania operatorow + i -
			if(key == 14)
			{
				sprintf(text, "0");
				LCD_WriteText(text);
				numTab[flag2++] = 0;
			} else if(key == 9)
			{
				sprintf(text, "1");
				LCD_WriteText(text);
				numTab[flag2++] = 1;
			} else if(key == 10)
			{
				sprintf(text, "2");
				LCD_WriteText(text);
				numTab[flag2++] = 2;
			} else if(key == 11)
			{
				sprintf(text, "3");
				LCD_WriteText(text);
				numTab[flag2++] = 3;
			} else if(key == 5)
			{
				sprintf(text, "4");
				LCD_WriteText(text);
				numTab[flag2++] = 4;
			} else if(key == 6)
			{
				sprintf(text, "5");
				LCD_WriteText(text);
				numTab[flag2++] = 5;
			} else if(key == 7)
			{
				sprintf(text, "6");
				LCD_WriteText(text);
				numTab[flag2++] = 6;
			} else if(key == 1)
			{
				sprintf(text, "7");
				LCD_WriteText(text);
				numTab[flag2++] = 7;
			} else if(key == 2)
			{
				sprintf(text, "8");
				LCD_WriteText(text);
				numTab[flag2++] = 8;
			} else if(key == 3)
			{
				sprintf(text, "9");
				LCD_WriteText(text);
				numTab[flag2++] = 9;
			} else if(key == 16)		// Sprawdzanie czy wcisnieto znak =
			{
				sprintf(text, "=");
				LCD_WriteText(text);
				break;
			}

			// W przypadku wypelnienia tablicy numTab czterema cyframi mozna wybrac juz tylko znak =
			if(flag2 == 4)
			{
				while(1)
				{
					key = getKey2();

					if(key == 16)
					{
						sprintf(text, "=");
						LCD_WriteText(text);
						break;
					}
				}
				break;
			}
		}

		// Tworzenie drugiej liczby na podstawie cyfr w tablicy
		// oraz ilosci wpisanych cyfr
		if(flag2 == 1){
			num2 = numTab[0];
		} else if(flag2 == 2){
			num2 = numTab[0]*10 + numTab[1];
		} else if(flag2 == 3){
			num2 = numTab[0]*100 + numTab[1]*10 + numTab[2];
		} else if(flag2 == 4){
			num2 = numTab[0]*1000 + numTab[1]*100 + numTab[2]*10 + numTab[3];
		}


		memset(numTab, 0, sizeof numTab);	// Wyzerowanie tablicy dla nastepnego wyrazenia
		LCD_GoTo(0,1);		// Pokazanie wyniku w drugiej linii wyswietlacza

		//Wyswietlenie wyniku w zaleznosci od wybranego operatora
		if(oper == '+'){
			sprintf(text, "%d", num1 + num2);
			LCD_WriteText(text);
		} else if(oper == '-'){
			sprintf(text, "%d", num1 - num2);
			LCD_WriteText(text);
		}
		// Wyzerowanie zmiennych pomocniczych
		flag1 = 0;
		flag2 = 0;

		// Skasowanie aktualnego wyrazenia w przypadku ponownego wcisniecia znaku =
		while(1){
			key =getKey2();
			if(key == 16)
			{
				break;
			}
		}
	}


}
