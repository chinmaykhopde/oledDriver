
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
	setAreaToUpdate(posY, posY, posX, posX + 5);
	
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
	uint8_t tensMinute = (minutes / 10) + numberOffset;
	minutes = minutes % 10 + numberOffset;

	
	uint8_t tensHours = hours / 10 + numberOffset;
	hours = hours % 10 + numberOffset;

	// display5x8Char()
}

void oledDriver::displayHome() {
	
	//Display DAY:MON
	splay5x8Char(numberOffset5x8Font + )
}

void oledDriver::displayTempPressure() {

}