#include <QFileInfo>
#include <QCryptographicHash>
#include "Calc.h"

unsigned int Calc::CRC32[256];
bool Calc::hasInited = false;

void Calc::InitCRC32()
{
	int   i,j;
	uint   crc;
	for(i = 0;i < 256;i++)
	{
		crc = i;
		for(j = 0;j < 8;j++)
		{
			if(crc & 1)
			{
				crc = (crc >> 1) ^ 0xEDB88320;
				//crc = (crc >> 1) ^ 0x04C11DB7;
				//crc = (crc >> 1) ^ 0x1EDC6F41;
				//crc = (crc >> 1) ^ 0x7BD11C40;
				//crc = (crc >> 1) ^ 0x04C10DB7;
				//crc = (crc >> 1) ^ 0x7BD01C40;
			}
			else
			{
				crc = crc >> 1;
			}
		}
		CRC32[i] = crc;
	}
}

Calc::Calc(QString file, int options /*= CALC_OPTION_LOWER*/)
{
	this->file = file;
	this->option = options;
}

Calc::~Calc(void)
{
}

int Calc::Run()
{
	if(!hasInited)
		InitCRC32();

	if(file.isNull() ||
		file.isEmpty())
		return -1;
	
	QFile inFile(file);
	if(inFile.open(QIODevice::ReadOnly))
	{		
		QCryptographicHash chMd5(QCryptographicHash::Md5);
		QCryptographicHash chSHA1(QCryptographicHash::Sha1);
		qint64 fileSize = inFile.size();
		qint64 readSize = 0;
		QByteArray buf;
		quint32 checkSum = 0;
		int len = 0,bufLen = 0;
		char *dat;
		unsigned int crc32Ret = 0xFFFFFFFF;
		while(readSize < fileSize)
		{
			buf = inFile.read(1<<20);
			bufLen = buf.length();
			chMd5.addData(buf);
			chSHA1.addData(buf);
			dat = buf.data();
			while(len < bufLen)
			{
				checkSum += (unsigned char)(*(dat + len));

				crc32Ret = CRC32[((crc32Ret & 0xFF) ^ (unsigned char)(*(dat + len)))] ^ (crc32Ret >> 8);

				len ++;
			}
			len = 0;
			readSize += buf.length();
		}
		crc32Ret = ~crc32Ret;
		inFile.close();
		md5 = chMd5.result().toHex().toLower();
		sha1 = chSHA1.result().toHex().toLower();
		sum = QString::number(checkSum, 16).toLower();
		crc32 = QString::number(crc32Ret, 16).toLower();
		if(option & CALC_OPTION_UPPER)
		{
			md5 = md5.toUpper();
			sha1 = sha1.toUpper();
			sum = sum.toUpper();
			crc32 = crc32.toUpper();
		}	
		return 0;	
	}
	return -2;
}
