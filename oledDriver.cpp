
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

	//set display offset
	sendCommand(setDisplayOffset);
	sendCommand(0x00);

	//set display startline
	sendCommand(0x40);
	
	//set contrast control
	sendCommand(setContrastControl);
	sendCommand(0x7F);

	//set segment remap to A0
	//set according to hardware specs for our module
	sendCommand(0xA0);

	//set COM output scan direction
	sendCommand(setCOMOutputScanDirectionNormal);


	//Set pre-charge period
	sendCommand(setPrechargePeriod);
	sendCommand(0xF1);

	//set com pins hardware configuration to 0x12
	sendCommand(setCOMHardwareConfig);
	sendCommand(0x12);
		
	//set Vcomh
	sendCommand(setVCOMHDeselectLevel);
	sendCommand(0x30);

	//set oscillator frequency
	sendCommand(setClockDivider);
	sendCommand(0x80);

	//Disable scroll
	sendCommand(Deactivate_Scroll_Cmd);

	//enable charge pump regulator
	sendCommand(setChargePumpSetting);
	sendCommand(enableChargePump);

	//set entire display on
	sendCommand(setDisplayOn);

}

void oledDriver::sendData(char data) {
	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(dataMode);
	TinyWireM.send(data);
	TinyWireM.endTransmission();
}

void oledDriver::sendCommand(char command) {
	TinyWireM.begin();
	TinyWireM.beginTransmission(SlaveAddress);
	TinyWireM.send(commandMode);
	TinyWireM.send(command);
	TinyWireM.endTransmission();
}

void oledDriver::displayTest() {
	sendCommand(setPageAddress);
	sendCommand(0x00);
	sendCommand(0x07);

	sendCommand(setColumnAddress);
	sendCommand(0x00);
	sendCommand(0x7F); 

	for (char i = 0; i<=60; i++) {
		for (char j=0; j<=16; j++)
		sendData(j);
	}
}

void oledDriver::displayTime() {
	//8*25
	//
}