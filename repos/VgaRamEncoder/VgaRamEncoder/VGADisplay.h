#pragma once

#include <fstream>
#include <windows.h>

using namespace std;

class VGADisplay {
	HANDLE hConsole;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	
	ifstream sourceFile;
	string sourcePath;
	int charCounter;

	void drawChar(char char_code, char color, ostream &outStream);

public:
	VGADisplay(string source, HANDLE handle);
	int display(int &row, ostream &outStream);

	enum error_codes {
		VGADISPLAY_SUCCESS = 0,
		VGADISPLAY_EMPTY_LINE = 100,
		VGADISPLAY_MISSING_START_CODE,
		VGADISPLAY_BYTE_COUNT_NOT_2,
		VGADISPLAY_DUPLICATE_ADDRESS,
		VGADISPLAY_DECENDING_ADDRESS,
		VGADISPLAY_UNEXPECTED_RECORD_TYPE,
		VGADISPLAY_PREMATURE_EOF,
		VGADISPLAY_INVALID_ADDRESS,
		VGADISPLAY_INVALID_COLOR_CODE,
		VGADISPLAY_INVALID_CHARACTER_CODE,
		VGADISPLAY_INVALID_CHECKSUM_FORMAT,
		VGADISPLAY_CHECKSUM_INVALID,
		VGADISPLAY_CANNOT_OPEN_FILE,
		VGADISPLAY_CANNOT_OBTAIN_CON_INFO
	};
};