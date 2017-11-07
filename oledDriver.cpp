//Code by
//Chinmay Khopde and Viraj Sonawne
//Alot of improve ments can be done here for example the 
#include <TinyWireM.h>
#include <USI_TWI_Master.h>
#include "oledDriver.h"

//
void oledDriver::init(){

	TinyWireM.begin();

	delay(5); //Wait for SSD1306 to start

	//set display off
	sendCommand(setDisplaySleep);

	/*****************Addressing Mode**********************/

	sendCommand(setMemoryAddressingMode);
	sendCommand(horizontalMode); 						//Horizontal Mode
	
	/*Set Address location for the display in the RAM and 
	  the collums and pages in RAM to be sent to the display 
	*/

	//used to set the first page and last page to be used 
	//only for horizontal and vertical addressing modes
	sendCommand(setPageAddress);
	sendCommand(0x00);
	sendCommand(0x07);

	//used to set the first collum and last collum to be used 
	//only for horizontal and vertical addressing modes
	sendCommand(setColumnAddress);
	sendCommand(0x00);
	sendCommand(0x7F); 

	//set display to normal or inverted
	sendCommand(disableInversion);

	//Set mux ratio
	sendCommand(setMuxRatio);
	sendCommand(0x3F); 						//MUX ratio 64

	//Set display offset
	sendCommand(setDisplayOffset);
	sendCommand(0x00);

	//Set display startline
	sendCommand(0x40);
	
	//Set contrast control
	sendCommand(setContrastControl);
	sendCommand(0x7F);

	//Set segment remap to A0
	//Set according to hardware specs for our module
	sendCommand(0xA0);

	//Set COM output scan direction
	sendCommand(setCOMOutputScanDirectionNormal);

	//Set pre-charge period
	sendCommand(setPrechargePeriod);
	sendCommand(0xF1);

	//Set com pins hardware configuration to 0x12
	sendCommand(setCOMHardwareConfig);
	sendCommand(0x12);

	//Set Vcomh (What is Vcomh ???????????)
	sendCommand(setVCOMHDeselectLevel);
	sendCommand(0x30);

	//Set oscillator frequency
	sendCommand(setClockDivider);
	sendCommand(0x80);

	//Disable scroll
	sendCommand(deactivateScroll);

	//Enable charge pump regulator
	sendCommand(setChargePumpSetting);
	sendCommand(enableChargePump);

	//Set entire display on
	sendCommand(setDisplayOn);

}

void oledDriver::sendData(char data) {
	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(dataMode);
	TinyWireM.send(data);
	TinyWireM.endTransmission();
}

void oledDriver::sendDataArray(char *charDataArray, int length) {

	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(dataMode);
	if (length <= 16) {
		for (int i=0; i<length; i++) {
			TinyWireM.send(*charDataArray);
			charDataArray++;
		}
		TinyWireM.endTransmission();
	}

	else {
		int remainderOfLength = length%16;
		length = length / 16;

		TinyWireM.begin();
		TinyWireM.beginTransmission(SlaveAddress);
		TinyWireM.send(dataMode);

		for (int i =0; i < length; i++){
			for(int j=0; j < 16; j++){
				TinyWireM.send(*charDataArray);
				charDataArray++;
			}
		TinyWireM.endTransmission();
		}

		TinyWireM.begin();
		TinyWireM.beginTransmission(SlaveAddress);
		TinyWireM.send(dataMode);
		
		for (int i=0; i < remainderOfLength; i++){
			TinyWireM.send(*charDataArray);
			charDataArray++;
		}

		TinyWireM.endTransmission();	
	}
}

void oledDriver::sendCommand(char command) {
	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(commandMode);
	TinyWireM.send(command);
	TinyWireM.endTransmission();
}

void oledDriver::setAreaToUpdate(char startPage, char endPage, char startCollum, char endCollum){
	sendCommand(setPageAddress);
	sendCommand(startPage);
	sendCommand(endPage);

	sendCommand(setColumnAddress);
	sendCommand(startCollum);
	sendCommand(endCollum); 
}

void oledDriver::displayFill() {

	setAreaToUpdate(0x00,0x07,0x00, 0x7F);

	for (int i=0; i<(128*8)/16 ; i++) {		
		TinyWireM.begin();
		TinyWireM.beginTransmission(SlaveAddress);
		TinyWireM.send(dataMode);
			for (int i =0; i<16; i++) { 
				TinyWireM.send(0xFF);
			}
		TinyWireM.endTransmission();
		}
}

void oledDriver::clearScreen() {

	// Set the Screen area to update
	setAreaToUpdate(0x00, 0x07, 0x00, 0x7F);
	
	//16 is the limit of tinywire i2c data frame
	//Optimised clearscreen
	
	for (int i=0; i<(128*8)/16 ; i++) {		
	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(dataMode);
		for (int i =0; i<16; i++) { 
			TinyWireM.send(0x00);
		}
	TinyWireM.endTransmission();
	}

}

//For 5*8 bitmaps
void oledDriver::display5x8Char(unsigned char asciiChar, char posX, char posY) {				//Page is Y
	
	unsigned char asciiAddress = (unsigned char) our5x8Font + asciiChar*5;
	setAreaToUpdate(posY, posY, posX, posX + 4);
	
	char asciiData[5];

	for(int i=0; i<5; i++) {
		asciiData[i] = pgm_read_word_near(asciiAddress+i);
	}
	sendDataArray(asciiData ,5);
}

void oledDriver::displayHome(DateTime& dt) {

	clearScreen();
	//Day
	uint8_t day = dt.day();
	uint8_t tensDay = day /10;
	day = day %10;

	display5x8Char(tensDay, homeDateStartLoc, 0x00);
	//Space  2
	display5x8Char(day, homeDateStartLoc + 6, 0x00);
	// :
	display5x8Char(numberOffset5x8Font + 26, homeDateStartLoc + 11, 0x00);
	
	//Month
	switch (dt.month()) {
		case 1:
			// J
			display5x8Char(19, homeDateStartLoc + 16, 0x00);
			// Space A
			display5x8Char(10, homeDateStartLoc + 22, 0x00);
			// Space N
			display5x8Char(23, homeDateStartLoc + 28, 0x00);
			break;
		case 2:
			// F
			display5x8Char(15, homeDateStartLoc + 16, 0x00);
			// Space E
			display5x8Char(14, homeDateStartLoc + 22, 0x00);
			// Space B
			display5x8Char(11, homeDateStartLoc + 28, 0x00);
			break;
		case 3:
			// M
			display5x8Char(22, homeDateStartLoc + 16, 0x00);
			// Space A
			display5x8Char(10, homeDateStartLoc + 22, 0x00);
			// Space R
			display5x8Char(27, homeDateStartLoc + 28, 0x00);
			break;
		case 4:
			// A
			display5x8Char(10, homeDateStartLoc + 16, 0x00);
			// Space P
			display5x8Char(25, homeDateStartLoc + 22, 0x00);
			// Space R
			display5x8Char(27, homeDateStartLoc + 28, 0x00);
			break;
		case 5:
			// M
			display5x8Char(22, homeDateStartLoc + 16, 0x00);
			// Space A
			display5x8Char(10, homeDateStartLoc + 22, 0x00);
			// Space Y
			display5x8Char(34, homeDateStartLoc + 28, 0x00);
			break;
		case 6:
			// J
			display5x8Char(19, homeDateStartLoc + 16, 0x00);
			// Space U
			display5x8Char(30, homeDateStartLoc + 22, 0x00);
			// Space N
			display5x8Char(23, homeDateStartLoc + 28, 0x00);
			break;
		case 7:
			// J
			display5x8Char(19, homeDateStartLoc + 16, 0x00);
			// Space U
			display5x8Char(30, homeDateStartLoc + 22, 0x00);
			// Space L
			display5x8Char(21, homeDateStartLoc + 28, 0x00);
			break;
		case 8:
			// A
			display5x8Char(10, homeDateStartLoc + 16, 0x00);
			// Space U
			display5x8Char(30, homeDateStartLoc + 22, 0x00);
			// Space G
			display5x8Char(18, homeDateStartLoc + 28, 0x00);
			break;
		case 9:
			// S
			display5x8Char(28, homeDateStartLoc + 16, 0x00);
			// Space E
			display5x8Char(14, homeDateStartLoc + 22, 0x00);
			// Space P
			display5x8Char(25, homeDateStartLoc + 28, 0x00);
			break;
		case 10:
			// O
			display5x8Char(24, homeDateStartLoc + 16, 0x00);
			// Space C
			display5x8Char(13, homeDateStartLoc + 22, 0x00);
			// Space T
			display5x8Char(29, homeDateStartLoc + 28, 0x00);
			break;
		case 11:
			// N
			display5x8Char(23, homeDateStartLoc + 16, 0x00);
			// Space O
			display5x8Char(24, homeDateStartLoc + 22, 0x00);
			// Space V
			display5x8Char(31, homeDateStartLoc + 28, 0x00);
			break;
		case 12:
			// D
			display5x8Char(13, homeDateStartLoc + 16, 0x00);
			// Space E
			display5x8Char(14, homeDateStartLoc + 22, 0x00);
			// Space C
			display5x8Char(13, homeDateStartLoc + 28, 0x00);
			break;
	}


	displayLine(0x01);

	displayLine(0x06);

	//Year
	uint8_t year = (uint8_t)(dt.year() - 2000);

	uint8_t yearAr[3] = {};
	
	for (char i=0; i<3; i++){
		yearAr[i] = year % 10;
		year = year / 10;
	}
	// 2
	display5x8Char(2, homeYearStartLoc, 0x07);
	// Space 0
	display5x8Char(yearAr[2], homeYearStartLoc + 6, 0x07);
	// Space 1
	display5x8Char(yearAr[1], homeYearStartLoc + 12, 0x07);
	//Space 6
	display5x8Char(yearAr[0], homeYearStartLoc + 18, 0x07);

	//Time
	uint8_t hours = dt.hour();
	uint8_t minutes = dt.minute();

	uint8_t tensMinute = (minutes / 10);
	minutes = minutes % 10;

	uint8_t tensHours = hours / 10;
	hours = hours % 10;
	//1
	display20x32Char(tensHours, homeTimeStartLoc, 0x02);
	//Space 2
	display20x32Char(hours, homeTimeStartLoc + 20 + 3, 0x02);
	// :
	display20x32Char(10, homeTimeStartLoc + 43, 0x02);
	//5
	display20x32Char(tensMinute, homeTimeStartLoc + 63, 0x02);
	// Space 6
	display20x32Char(minutes, homeTimeStartLoc + 86, 0x02);

}

void oledDriver::displayLine(char page) {
	char line[31] = {};
	setAreaToUpdate(page, page, 0x00, 0x7F);
	//Making array to send 
	//only max 31 size arrey suported so two loops used
	for (int i=0; i<31; i++){
		line[i] = 0x18;
	}
	
	for (int i=0; i <4; i++) {
		sendDataArray(line, 31);
		sendData(0x18);
	}

}

void oledDriver::horizontalLine(char collum) {

	char data [8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	setAreaToUpdate(0x00, 0x07, collum, collum);
	sendDataArray(data, 8);
}

void oledDriver::displayTempPressure(long temprature, long pressure) {
	clearScreen();

	//Thermometer sprite
	display5x8Char(37, 31, 0x01);
	display5x8Char(38, 31, 0x02);
	display5x8Char(39, 31, 0x03);

	//Thermometer data

	printTemprature(temprature, 20, 0x06);
	//Altitude Sprite
	printPressure(pressure , 84, 0x06);
	//Altidude data
	// printNumber(pressure, 90, 0x06);
	horizontalLine(63);
	
	horizontalLine(64);

}

void oledDriver::display20x32Char(unsigned char asciiChar, char posX, char posY) {				//Page is Y
	
	unsigned int asciiAddress = (unsigned char) our20x32Font + asciiChar*80;
	setAreaToUpdate(posY, posY + 3, posX, posX + 19);
	
	char asciiData[20];
	for (int j=0; j < 4; j++){
		for(int i=0; i<20; i++) {
			asciiData[i] = pgm_read_word_near(asciiAddress+i+j*20);
		}
	sendDataArray(asciiData ,20);
	}
}

void oledDriver::printNumber(long number, char posX, char posY){								//Page is Y
	char tmp[5] = {};

	setAreaToUpdate(posY, posY, posX, posX + 22);

	unsigned char asciiAddress = (unsigned char) our5x8Font + 40*5;

	for (int i=0; i<5; i++) {
		tmp[i] = number % 10;
		number = number / 10;
	}
	for (int i=0; i<5; i++) {
		display5x8Char(tmp[4-i], i*5 , posY);
		// if (i==2) {
		// 	for(int i=0; i<3; i++) {
		// 		tmp[i] = pgm_read_word_near(asciiAddress + i);
		// 	}
		// 	sendDataArray(tmp ,4);
		// }
	}
}

void oledDriver::printTemprature(long temp, char posX, char posY) {
	unsigned char temprature[3] = {};
	unsigned char decimal [3] = {0x00, 0x40, 0x00};
	unsigned char degree [3] = {0x00, 0x02, 0x00};

	// setAreaToUpdate(posY, posY, posX, posX + 19);

		for(char i = 0; i<3; i++) {
				temprature[i] = temp % 10;
				temp = temp / 10;
				// decimal[i] = pgm_read_word_near(asciiAddress);
				// degree[i] = pgm_read_word_near(asciiAddress + 3);
		}

	display5x8Char(temprature[2], posX, posY);
	display5x8Char(temprature[1], posX + 6, posY);
	
	setAreaToUpdate(posY, posY, posX + 11, posX + 11 +2);
	sendDataArray(decimal, 3);

	display5x8Char(temprature [0],  posX + 14, posY);
	
	setAreaToUpdate(posY, posY, posX + 19, posX + 19 + 2 );
	sendDataArray(degree, 3);

	display5x8Char (numberOffset5x8Font + 2, posX + 22, posY);
}

void oledDriver::printPressure(long pressure, char posX, char posY) {
	unsigned char press [4] = {};
	unsigned char decimal [3] = {0x00, 0x40, 0x00};
	unsigned char degree [3] = {0x00, 0x02, 0x00};


	for(char i = 0; i<4; i++) {
		press[i] = pressure % 10;
		pressure = pressure / 10;
	}

	display5x8Char(press[3], posX, posY);

	setAreaToUpdate(posY, posY, posX + 5, posX + 5 +2);
	sendDataArray(decimal, 3);

	display5x8Char(press[2], posX + 8, posY);


	display5x8Char(press[1],  posX + 13, posY);

	display5x8Char(press[0],  posX + 19, posY);

	display5x8Char (numberOffset5x8Font + 1, posX + 25, posY);
}
