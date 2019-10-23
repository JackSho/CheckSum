
#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QTextCodec>

#include "Calc.h"
char usage[]="\nusage: ChSum file [options]\n\nfile\tmust be specified.\noption\tcan be the following values\n\t-u\tresult is uppercase characters\n";

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	QString argLine;
	if(argc == 1)
	{
        printf("%s", usage);
		return 0;
	}
	for(int i = 1; i < argc; i ++)
	{
		QString ar(QTextCodec::codecForLocale()->toUnicode(argv[i]));
		if(ar.contains(' '))
			ar = QString("\"%1\"").arg(ar);
		argLine.append(ar + " ");
	}
	
	//QRegExp rx("-([fFoO])\\s(\"(.+)?\"|\\S+)"); 
	//QRegExp rx("(\"(.+)?\"|\S+)(\\s-u)?"); 
	QRegExp rx("(\"(.+)?\"|\\S+)(\\s-([uU]))?"); 
	int pos = 0;
	QString inputFile;
	bool hasFile = false;
	int calcOption = CALC_OPTION_LOWER;
	bool hasOption = false;
	if(rx.indexIn(argLine) != -1)
	{
		QStringList ll = rx.capturedTexts();
		pos += rx.matchedLength(); 
		if(ll[1].size())
		{
			hasFile = true;
			inputFile = ll[1];
			if(inputFile.startsWith('\"') &&
				inputFile.endsWith('\"'))
			{
				inputFile = inputFile.mid(1, inputFile.size() -2);
			}
		}
		if(ll[4].size())
		{
			if(ll[4].compare("u", Qt::CaseInsensitive) == 0)
			{
				hasOption = true;
				calcOption = CALC_OPTION_UPPER;
			}
		}
	}
	if(!hasFile)
	{
        printf("%s", usage);
		return 0;
	}
	Calc calc(inputFile, calcOption);
	if(calc.Run())
	{
		qDebug() << "An error occurred during the process of calculation";
		return 0;
	}
	else
	{
		printf("MD5=%s\n", calc.md5.toLatin1().data());
		printf("SHA1=%s\n", calc.sha1.toLatin1().data());
		printf("SHA256=%s\n", calc.sha256.toLatin1().data());
		printf("CRC32=%s\n", calc.crc32.toLatin1().data());
		printf("SUM=%s\n", calc.sum.toLatin1().data());
	}
	return 0;
	//return a.exec();
}
