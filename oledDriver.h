#ifndef __OLEDDRIVER_H__
#define __OLEDDRIVER_H__

#include <Arduino.h>
#include <TinyWireM.h>
#include <USI_TWI_Master.h>

//#defines

//Slave address
#define SlaveAddress 				0x3c

//Commands
#define commandMode 				0x80
#define dataMode 					0x40

//Hardware setup commands
#define setContrastControl 			0x81 	//Contrast from 0-0xFF
#define entireDisplayOn 			0xA5
#define resumeDisplayToRAM 			0xA4
#define enableInversion 			0xA7
#define disableInversion 			0xA6
#define setDisplayOn 				0xAF 			//use for wake up from sleep mode
#define setDisplaySleep 			0xAE 		//put to sleep Mode for power saving

//Addressing mode commands
#define setMemoryAddressingMode 	0x20
#define horizontalMode 				0x00
#define verticalMode 				0x01
#define setColumnAddress 			0x21
#define setPageAddress 				0x22
#define setMuxRatio 				0xA8			//0d-63d
#define setDisplayOffset 			0xD3		//0-63d

#define setCOMOutputScanDirectionNormal 0xC0
#define setCOMOutputScanDirectionRemapped 0xC8
#define setCOMHardwareConfig 		0xDA	//Use 0x12 for our Display
#define setClockDivider 			0xD5		//Use 0xF0 for fast updates and 0x80 is tested
#define setPrechargePeriod 			0xD9
#define setVCOMHDeselectLevel 		0xDB


//Scrolling mode commands

#define Right_Horizontal_Scroll_Cmd           0x26
#define Left_Horizontal_Scroll_Cmd            0x27
#define Vertical_Right_Horizontal_Scroll_Cmd  0x29
#define Vertical_Left_Horizontal_Scroll_Cmd   0x2A
#define Activate_Scroll_Cmd                   0x2F
#define Deactivate_Scroll_Cmd                 0x2E
#define Set_Vertical_Scroll_Area_Cmd          0xA3


//Charge Pump Settings
#define setChargePumpSetting 			0x8D
#define enableChargePump 				0x14
#define disableChargePump 				0x10

//Class Declaration

class oledDriver {
public:
	//Initialises the OLED
	void init();

	//Send one command 
	void sendCommand(char);

	//Send one byte of data
	void sendData(char);

	//Send an arrey of data
	void sendDataArray(char* , int);

	//Generic Test funcion for Display
	void displayTest();

	//Dsplay the time given (incomplete)
	void displayTime();

	//Fill the screen with all zeroes
	void clearScreen();

	///Select area to be changed
	void setAreaToUpdate(char, char, char, char);
	
};



#endif