#pragma once
#include <QString>

#define		CALC_OPTION_LOWER		0x0
#define		CALC_OPTION_UPPER		0x1

class Calc
{
public:

	Calc(QString file, int options = CALC_OPTION_LOWER);
	~Calc(void);

	int Run();

	static unsigned int CRC32[256];
	static void InitCRC32();
	static bool hasInited;

public:
	QString file;
	QString md5;
	QString sha1;
	QString sum;
	QString crc32;

	int option;
};

