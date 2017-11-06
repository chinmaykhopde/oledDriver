
#include <TinyWireM.h>
#include <USI_TWI_Master.h>
#include "oledDriver.h"

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

#pragma regin
//Draw 10x16 font
// void oledDriver::display10x16Char(unsigned char asciiChar, char posX, char posY) {				//Page is Y
	
// 	unsigned char asciiAddress = (unsigned char) ourFonts + asciiChar*5;
// 	setAreaToUpdate(posY, posY, posX, posX + 5);
	
// 	char asciiData[5];

// 	for(int i=0; i<5; i++) {
// 		asciiData[i] = pgm_read_word_near(asciiAddress+i);
// 	}
// 	sendDataArray(asciiData, 5);
// }
// void oledDriver::drawLine(char posX1, char posY2, char)
#pragma endregion

void oledDriver::displayTime(uint8_t hours, uint8_t minutes) {
	uint8_t tensMinute = (minutes / 10);
	minutes = minutes % 10;

	
	uint8_t tensHours = hours / 10;
	hours = hours % 10;

	// display5x8Char()
}

void oledDriver::displayHome() {

	clearScreen();
	//Display DAY:MON
	display5x8Char(2, homeDateStartLoc, 0x00);
	//Space
	setAreaToUpdate(0x00, 0x00, homeDateStartLoc + 5, homeDateStartLoc + 5 + 1);
	sendData(0x00);
	// 2
	display5x8Char(6, homeDateStartLoc + 6, 0x00);
	// :
	display5x8Char(numberOffset5x8Font + 26, homeDateStartLoc + 11, 0x00);
	// J
	display5x8Char(19, homeDateStartLoc + 16, 0x00);
	// Space
	setAreaToUpdate(0x00, 0x00, homeDateStartLoc + 21, homeDateStartLoc + 21 + 1);
	sendData(0x00);
	// U
	display5x8Char(30, homeDateStartLoc + 22, 0x00);
	// Space
	setAreaToUpdate(0x00, 0x00, homeDateStartLoc + 27, homeDateStartLoc + 27 + 1);
	sendData(0x00);
	// N
	display5x8Char(23, homeDateStartLoc + 28, 0x00);

	displayLine(0x01);

	displayLine(0x06);

	// 2
	display5x8Char(2, homeYearStartLoc, 0x07);
	// Space
	setAreaToUpdate(0x07, 0x07, homeDateStartLoc + 5, homeDateStartLoc + 5 + 1);
	sendData(0x00);
	// 0
	display5x8Char(0, homeYearStartLoc + 6, 0x07);
	// Space
	setAreaToUpdate(0x07, 0x07, homeDateStartLoc + 11, homeDateStartLoc + 11 + 1);
	sendData(0x00);
	// 1
	display5x8Char(1, homeYearStartLoc + 12, 0x07);
	//Space
	setAreaToUpdate(0x07, 0x07, homeDateStartLoc + 17, homeDateStartLoc + 17 + 1);
	sendData(0x00);
	// 6
	display5x8Char(6, homeYearStartLoc + 18, 0x07);

	//Time
	display20x32Char(1, homeTimeStartLoc, 0x02);

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

void oledDriver::displayTempPressure() {
	clearScreen();

	//Thermometer sprite
	display5x8Char(37, 31, 0x01);
	display5x8Char(38, 31, 0x02);
	display5x8Char(39, 31, 0x03);
	

	horizontalLine(63);
	horizontalLine(64);

}

void oledDriver::display20x32Char(unsigned char asciiChar, char posX, char posY) {				//Page is Y
	
	unsigned char asciiAddress = (unsigned char) our20x32Font + asciiChar*5;
	setAreaToUpdate(posY, posY + 3, posX, posX + 19);
	
	char asciiData[20];
	for (int j=0; j < 4; j++){
		for(int i=0; i<20; i++) {
			asciiData[i] = pgm_read_word_near(asciiAddress+i);
		}
	sendDataArray(asciiData ,20);
	}
}

void oledDriver::printNumber(long number){
	clearScreen();
char tmp[5] = {};

	for (int i=0; i<5; i++){
		tmp[i] = number % 10;
		number = number / 10;
	}
	for (int i=0; i<5; i++){
		display5x8Char(tmp[4-i],i*5 , 0x00);

	}
}
