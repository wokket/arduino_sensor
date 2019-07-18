/*####################################################################
 FILE: dht11.cpp
 VERSION: 0.4.5
 PURPOSE: DHT11 Temperature & Humidity Sensor library for Arduino
 LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 DATASHEET: http://feed.virtuabotix.com/wp-content/uploads/2011/11/DHT11.pdf
 GET UPDATES: http://feed.virtuabotix.com/?239
     --##--##--##--##--##--##--##--##--##--##--  
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
     ##  ##  ##  ## DHT11 SENSOR ##  ##  ##  ##
     ##  ##  ##  ##  ##FRONT ##  ##  ##  ##  ##
     | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     | ##  ##  ##  ##  ##  ##  ##  ##  ##  ## |
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
     --##--##--##--##--##--##--##--##--##--##--
         ||       ||          || (Not    || 
         ||       ||          || Used)   || 
       VDD(5V)   Readout(I/O)          Ground
                 
 HISTORY:
 Mod by Joseph Dattilo (Virtuabotix LLC) - Version 0.4.5 (11/11/11)
 Mod by Joseph Dattilo (Virtuabotix LLC) - Version 0.4.0 (06/11/11)
 Mod by Rob Tillaart - Version 0.3 (28/03/2011)
 Mod by SimKard - Version 0.2 (24/11/2010)
 George Hadjikyriacou - Original version (??)
#######################################################################*/

#include "dht11.h"

// returnvalues:
//  0 : OK
// -1 : checksum error
// -2 : timeout

void dht11::attach(int pin)
{
	sensorPin = pin; //and there was much rejoicing! Pin attached.
}

int dht11::read()
{
	return read(sensorPin); //reads from attached sensorPin (attach command must be run first or this will use the default pin 2)
}

int dht11::read(int pin)
{
	// BUFFER TO RECEIVE
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;

	// EMPTY BUFFER
	for (int i = 0; i < 5; i++)
		bits[i] = 0;

	// REQUEST SAMPLE
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(18);
	digitalWrite(pin, HIGH);
	delayMicroseconds(40);
	pinMode(pin, INPUT);

	// ACKNOWLEDGE or TIMEOUT
	unsigned int loopCnt = 10000;
	while (digitalRead(pin) == LOW)
		if (loopCnt-- == 0)
			return -2;

	loopCnt = 10000;
	while (digitalRead(pin) == HIGH)
		if (loopCnt-- == 0)
			return -2;

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (int i = 0; i < 40; i++)
	{
		loopCnt = 10000;
		while (digitalRead(pin) == LOW)
		{
			if (loopCnt-- == 0)
			{
				return -2;
			}
		}

		unsigned long t = micros();

		loopCnt = 10000;
		while (digitalRead(pin) == HIGH)
		{
			if (loopCnt-- == 0)
			{
				return -2;
			}
		}

		if ((micros() - t) > 40)
		{
			bits[idx] |= (1 << cnt);
		}

		if (cnt == 0) // next byte?
		{
			cnt = 7; // restart at MSB
			idx++;   // next byte!
		}
		else
			cnt--;
	}

	// WRITE TO RIGHT VARS
	// as bits[1] and bits[3] are allways zero they are omitted in formulas.
	//TT: Not on my machine!
	humidity = bits[0];
	humidity += ((double)bits[1] / 10);
	temperature = bits[2];
	temperature += ((double)bits[3] / 10);

	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];

	/* Serial.print("bits[0]:");
	Serial.println(bits[0]);
	Serial.print("bits[1]:");
	Serial.println(bits[1]);
	Serial.print("bits[2]:");
	Serial.println(bits[2]);
	Serial.print("bits[3]:");
	Serial.println(bits[3]);
	Serial.print("bits[4]:");
	Serial.println(bits[4]);*/

	if (bits[4] != sum)
	{
		return -1;
	}

	return 0;
}

double dht11::getTemp()
{
	read(); //make sure the temp has been read
	return temperature;
}

double dht11::getHumidity()
{
	read(); //make sure the humidity has been read
	return humidity;
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dht11::getDewPoint()
{
	read(); //make sure the temp has been read

	double a = 17.271;
	double b = 237.7;
	double temp = (a * temperature) / (b + temperature) + log((double)humidity / 100);
	double Td = (b * temp) / (a - temp);
	return Td;
}