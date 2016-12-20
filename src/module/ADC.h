/** \file ADC.h */

#define ADC_ON()            ADCSRA |= (1<<ADEN)
#define ADC_OFF()           ADCSRA &= ~(1<<ADEN)
#define ACOMP_ON()          ACSR |= (1<<ACIE)|(1<<ACI)
#define ACOMP_OFF()         ACSR &= ~(1<<ACIE)

#define ACOMP_SETUP() {\
	ACSR = (1<<ACIE)|(1<<ACIS1)|(1<<ACIS0); /* trigger interrupt on rising output edge */\
	/* DIDR1 = 3; // Disable PD7&PD6 analog comp, saves power consumption */\
}

/**
 *	Setup ADC conversion. CHANNEL must be a number between 0 and 8.
 */ 
#define ADC_SELECT_CHANNEL(channel) {\
    ADMUX |= (channel & 0x0F);/* Only writing channel bits, to protect the MSGs of the ADMUX register.*/\
}

/**
 *	Set start conversion bit to 1.
 */ 
#define ADC_START_CONVERSION() ADCSRA |= (1<<ADSC) 

/**
 *	Wait until conversion finished.
 */
#define ADC_FINISH_CONVERSION() while ( (ADCSRA & (1 << ADSC)) ) /* ADSC bit returns to 0 when conversion is complete */

/**
 *	Macro that calls other macros to setup the ADC entirely at boot.
 */
#define ADC_SETUP() {\
	ADCSRA |= (1 << ADPS2);/* Set ADC clock to 8 MHz/16 = 500 kHz (>> 200 kHz). Setting this frequency lower throws off the ACOMP interrupt. */\
	ADMUX |= (1 << REFS0);\
	ADMUX &= ~(1 << REFS1);/* Set the voltage reference of the ADC to be AVCC.*/\
	ADCSRB |= (1 << ADTS0);\
	ADCSRB &= ~( (1 << ADTS2) | (1 << ADTS1) );/* Set ADC auto trigger source to free running mode (always on) */\
	ADCSRA |= (1 << ADATE);/* ADC conversion Auto Trigger Enabled. */\
	ADCSRA |= (1 << ADEN);/* Power up the ADC*/\
	ADC_SELECT_CHANNEL(0);/* IRRXCOM signal is on channel 0 (cell rev A) */\
    ADC_START_CONVERSION();\
}

#define ADC_TRIGGER_STOP() ADCSRA &= ~(1<<ADATE)