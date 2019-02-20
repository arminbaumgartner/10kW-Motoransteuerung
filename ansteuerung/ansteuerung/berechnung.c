/*
 * berechnung.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur Berechnung der Fahrdaten
 *
 *	Geschwindgkeits / Umdrehungen pro min berrechnung durch Timer 1
 *	Erkennung des Stilstandes durch Timer 1
 *
 *	Berechnungsfunktionen die f�r das Hauptprogramm n�tig sind
 *
 *
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "berechnung.h"
#include "lcd.h"


volatile uint16_t steps;	//Timer 1 Schrittweite (3km/h - 100km/h)

int timer1_teiler_mult = 4;		//Timer 1 Teilerzeit
int motor_teiler = 3;			//Elektrische Teilung vom Motor
float uebersetzung = 1;			//�bersetzung
float raddurchmesser = 0.2;		//In Meter

char overflow;

/*
volatile uint16_t step_dauer;
volatile uint16_t step_dauer_help;
volatile uint16_t drehzahl_pro_sekunde;
volatile float geschwindigkeit_help;
volatile uint16_t geschwindigkeit;
volatile uint16_t drehzahl;
*/

volatile float step_dauer;
volatile float step_dauer_help;
volatile float drehzahl_pro_sekunde;
volatile float geschwindigkeit_help;
volatile float geschwindigkeit;
volatile float drehzahl;


volatile uint16_t geschwindigkeit_ausgabe;
volatile uint16_t drehzahl_ausgabe_01;

char ausgabe[10];


void Init_Timer1 (void)
{
	TCCR1B = TCCR1B | (1<<CS10);		// Teiler 256 (16MHz / 64 = 4�s)
	TCCR1B = TCCR1B | (1<<CS11);		//Kleiner Schritt 4�s		(1*4�s)
	TCCR1B = TCCR1B &~ (1<<CS12);		//Gr��ter Schritt 262ms	(65535*4�s)
	
	TIMSK1 = TIMSK1 | (1<<TOIE1);		//OVERFLOW-Interrupt aktivieren
}
void geschwindigkeit_auslesen(void)
{
	
	overflow = 0;
	steps = TCNT1;
	TCNT1 = 0;
	

}
void drehzahl_berechnung (void)
{
	if(overflow)
	{
		steps = 0;
		drehzahl = 0;
		drehzahl_pro_sekunde = 0;
		step_dauer = 0;
	}
	else
	{
		if(steps <= 25)			//Geschwindigkeits �berhohung abfangen -> 25*4 = 100
		{
			steps = 25;
		}
		
		
		if(steps >= 15500)		//Geschwindigkeits unterschreitung -> 13.750 n�tig um 1 U/s zu generieren
		{
			steps=15500;
		}
		

		step_dauer = steps*timer1_teiler_mult;		//Werte von max 55.000�s um auf 1U/s zu kommen
		step_dauer = step_dauer/100;				//Werte von 1 bis 550				///////ab hier korriegieren /// Teiler vieleicht auf 64 statt 256
		
		
		step_dauer_help = (step_dauer*6*motor_teiler);	//Werte von 18 bis 9900
		step_dauer_help = step_dauer_help/10;			//Werte von 1 bis 990
		
		drehzahl_pro_sekunde = 1000/step_dauer_help;	//Werte von 1 bis 1000
		
		drehzahl = drehzahl_pro_sekunde*60;
	
	}
}
void geschwindigkeit_berechnung(void)
{
	
	if(overflow)
	{
		steps = 0;
		geschwindigkeit = 0;
		drehzahl = 0;
		drehzahl_pro_sekunde = 0;
		step_dauer = 0;
	}
	else
	{
		/*
			
		if(steps <= 25)			//Geschwindigkeits �berhohung abfangen -> 25*4 = 100
		{
			steps = 25;
		}
	
	
		if(steps >= 15500)		//Geschwindigkeits unterschreitung -> 13.750 n�tig um 1 U/s zu generieren
		{
			steps=15500;
		}
	

		step_dauer = steps*timer1_teiler_mult;		//Werte von max 55.000�s um auf 1U/s zu kommen
		step_dauer = step_dauer/100;				//Werte von 1 bis 550				///////ab hier korriegieren /// Teiler vieleicht auf 64 statt 256
	
	
		step_dauer_help = (step_dauer*6*motor_teiler);	//Werte von 18 bis 9900
		step_dauer_help = step_dauer_help/10;			//Werte von 1 bis 990
	
		drehzahl_pro_sekunde = 1000/step_dauer_help;	//Werte von 1 bis 1000
	
		drehzahl = drehzahl_pro_sekunde*60;
		
		*/
		
		drehzahl_ausgabe_01 = (uint16_t)drehzahl;
	
		geschwindigkeit_help = ((drehzahl_pro_sekunde*raddurchmesser*3.14)/uebersetzung) * 3.6;
	
	
		//drehzahl = ceil(drehzahl);
	
		geschwindigkeit_help = ((drehzahl_pro_sekunde * raddurchmesser * 3.14) / uebersetzung) * 3.6;
		geschwindigkeit = ceil (geschwindigkeit_help);
		
		geschwindigkeit_ausgabe = (uint16_t)geschwindigkeit;
	
		}
	
	
}

void drehzahl_ausgabe (void)
{
	sprintf(ausgabe,"    ");
	LCD_cmd(0x8b);   //gehe zu 1. Zeile, 25. Position
	LCD_string(ausgabe);
	
	sprintf(ausgabe,"%d",drehzahl_ausgabe_01);
	LCD_cmd(0x8a);   //gehe zu 1. Zeile, 25. Position
	LCD_string(ausgabe);
}

void geschwindigkeits_ausgabe (void)
{
	sprintf(ausgabe,"    ");
	LCD_cmd(0xcb);   //gehe zu 2. Zeile, 25. Position
	LCD_string(ausgabe);
	
	sprintf(ausgabe,"%d",geschwindigkeit_ausgabe);
	LCD_cmd(0xca);   //gehe zu 2. Zeile, 25. Position
	LCD_string(ausgabe);
}
void preset_drehzahl_gesch (void)
{
	drehzahl = 0;
	geschwindigkeit = 0;
}
char umschalt_null (void)
{
	if (drehzahl == 0)
	{
		return 1;
	}
	else
	{
		return 0;		//Geschwindigkeit zu hoch
	}
	
}

ISR(TIMER1_OVF_vect)			//Motor steht
{
	
	overflow = 1;
	
	steps = 0;
	geschwindigkeit = 0;
	geschwindigkeit_ausgabe = 0;
	drehzahl = 0;
	drehzahl_ausgabe_01 = 0;
	drehzahl_pro_sekunde = 0;
	step_dauer = 0;
	
}