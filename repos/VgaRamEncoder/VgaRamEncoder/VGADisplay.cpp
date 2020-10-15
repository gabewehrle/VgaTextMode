#include "VGADisplay.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

VGADisplay::VGADisplay(string source, HANDLE handle) {
	sourcePath = source;
	hConsole = handle;
	charCounter = 0;
}

int VGADisplay::display(int &row, ostream &outStream) {
	// 0 - Success
	// 1 - Empty line
	// 2 - Missing start code ':'
	// 3 - Byte count should be 2
	// 4 - Duplicate address in a row
	// 5 - Decending address
	// 6 - Unexpected record type
	// 7 - Premature EOF
	// 8 - Invalid address
	// 9 - Invalid color code
	// 10 - Invalid character code
	// 11 - Invalid checksum (format)
	// 12 - Checksum does not match calculated checksum
	// 13 - Could not open file
	// 14 - Could not obtain console information (consoleapi2.h)
	sourceFile.open(sourcePath);
	if (!sourceFile.is_open()) {
		return VGADISPLAY_CANNOT_OPEN_FILE;
	}
	
	string line;
	line.reserve(15);
	int addr = -1;
	int status = VGADISPLAY_SUCCESS;

	//:               02        0000   00     7020      6e
	//start code | byte count | addr | type | data | checksum
	outStream << '\n';
	charCounter = 0;
	row = 0;
	if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo) == 0) {
		row = GetLastError();
		return VGADISPLAY_CANNOT_OBTAIN_CON_INFO;
	}
	while (getline(sourceFile, line)) {
		if (line.empty()) {
			status = VGADISPLAY_EMPTY_LINE;
			break;
		}
		if (line[0] != ':') {
			status = VGADISPLAY_MISSING_START_CODE;
			break;
		}
		
		//Check type
		int type = 65535;
		try {
			type = stoul(line.substr(7, 2), nullptr, 16);
		} catch (invalid_argument) {
			status = VGADISPLAY_UNEXPECTED_RECORD_TYPE;
			break;
		}
		if (type == 0x01) {
			//EOF
			if (getline(sourceFile, line) && !(line.empty() || line == "\n")) {
				status = VGADISPLAY_PREMATURE_EOF;
				break;
			}
			status = VGADISPLAY_SUCCESS;
			break;
		}
		else if (type != 0x00) {
			status = VGADISPLAY_UNEXPECTED_RECORD_TYPE;
			break;
		}

		//Check byte size
		try {
			if (stoul(line.substr(1, 2), nullptr, 16) != 0x02) {
				status = VGADISPLAY_BYTE_COUNT_NOT_2;
				break;
			}
		}
		catch (invalid_argument) {
			status = VGADISPLAY_BYTE_COUNT_NOT_2;
			break;
		}

		//Verify address
		int curAddr = -1;
		try {
			curAddr = stoul(line.substr(3, 4), nullptr, 16);
		} catch (invalid_argument) {
			status = VGADISPLAY_INVALID_ADDRESS;
			break;
		}
		if (curAddr == addr) {
			status = VGADISPLAY_DUPLICATE_ADDRESS;
			break;
		}
		else if (curAddr < addr) {
			status = VGADISPLAY_DECENDING_ADDRESS;
			break;
		}
		for (int i = addr + 1; i < curAddr; ++i) {
			//Fill 0s
			drawChar(0, 0, outStream);
		}
		addr = curAddr;

		//Get data
		char color, char_code;
		try {
#pragma warning( suppress : 4244)
			color = stoul(line.substr(9, 2), nullptr, 16);
		}
		catch (invalid_argument) {
			status = VGADISPLAY_INVALID_COLOR_CODE;
			break;
		}
		try {
#pragma warning( suppress : 4244)
			char_code = stoul(line.substr(11, 2), nullptr, 16);
		}
		catch (invalid_argument) {
			status = VGADISPLAY_INVALID_CHARACTER_CODE;
			break;
		}

		//Get checksum
		int checksum;
		try {
			checksum = stoul(line.substr(13, string::npos), nullptr, 16);
		} catch (invalid_argument) {
			status = VGADISPLAY_INVALID_CHECKSUM_FORMAT;
			break;
		}

		//Verify checksum
		int checksumCalc = (~(0x02 + (addr & 0xFF) + ((addr >> 8) & 0xFF) + 0x00 + +color + +char_code) + 1) & 0xFF;
		if (checksum != checksumCalc) {
			status = VGADISPLAY_CHECKSUM_INVALID;
			break;
		}

		drawChar(char_code, color, outStream);
		//cout << char_code;
		++row;
	}
	SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
	outStream << '\n';
	return status;
}

void VGADisplay::drawChar(char char_code, char color, ostream &outStream) {
	WORD colorAttribute = 0;

	switch ((color & 0xF0) >> 4) {
		case 1:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_BLUE;
			break;
		case 2:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_GREEN;
			break;
		case 3:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case 4:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_RED;
			break;
		case 5:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case 6:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case 7:
			colorAttribute = colorAttribute | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case 8:
			colorAttribute = colorAttribute | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case 9:
			colorAttribute = colorAttribute | FOREGROUND_BLUE;
			break;
		case 10:
			colorAttribute = colorAttribute | FOREGROUND_GREEN;
			break;
		case 11:
			colorAttribute = colorAttribute | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case 12:
			colorAttribute = colorAttribute | FOREGROUND_RED;
			break;
		case 13:
			colorAttribute = colorAttribute | FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case 14:
			colorAttribute = colorAttribute | FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case 15:
			colorAttribute = colorAttribute | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
	}
	switch (color & 0x0F) {
		case 1:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_BLUE;
			break;
		case 2:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_GREEN;
			break;
		case 3:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case 4:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_RED;
			break;
		case 5:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
			break;
		case 6:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
			break;
		case 7:
			colorAttribute = colorAttribute | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case 8:
			colorAttribute = colorAttribute | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case 9:
			colorAttribute = colorAttribute | BACKGROUND_BLUE;
			break;
		case 10:
			colorAttribute = colorAttribute | BACKGROUND_GREEN;
			break;
		case 11:
			colorAttribute = colorAttribute | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case 12:
			colorAttribute = colorAttribute | BACKGROUND_RED;
			break;
		case 13:
			colorAttribute = colorAttribute | BACKGROUND_RED | BACKGROUND_BLUE;
			break;
		case 14:
			colorAttribute = colorAttribute | BACKGROUND_RED | BACKGROUND_GREEN;
			break;
		case 15:
			colorAttribute = colorAttribute | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
	}
	SetConsoleTextAttribute(hConsole, colorAttribute);

	if (char_code == 0) {
		outStream << ' ';
	}
//	else if (char_code >= 32) {
	else {
		outStream << char_code;
	}
//	else {
//		outStream << (char) 0x3F; //Question mark
//	}

	if (charCounter >= 79) {
		outStream << '\n';
		charCounter = -1;
	}

	++charCounter;
}