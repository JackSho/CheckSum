#include <QCryptographicHash>
#include <QDebug>
#include <QFileIconProvider>

#include "CalculateFileCheckThread.h"

CalculateFileCheckThread::CalculateFileCheckThread(QObject *parent)
	: QThread(parent),taskSuccess(false),taskExist(false),taskValid(false),stopTask(false)
{
	connect(this,SIGNAL(finished()),SLOT(slot_Finished()),Qt::QueuedConnection);//�������ź������߳����淢��
	//connect(this,SIGNAL(started()),SLOT(slot_Started()),Qt::QueuedConnection);//�������ź������߳����淢��
	qRegisterMetaType<FileCheckSum>("FileCheckSum"); //д�ڹ��캯����  ,ע������ͣ����� QT ����ʶ�������ͣ����źŵĲ������д�����ʱ���źŷ��Ͳ��ɹ�
}

CalculateFileCheckThread::~CalculateFileCheckThread()
{
	wait();
}

bool CalculateFileCheckThread::StartTask()
{
	if(fileCheckSum.fileInfo.filePath().isEmpty())
		return false;
	if(fileCheckSum.fileInfo.isFile() && fileCheckSum.fileInfo.exists())
	{
		taskValid = true;
	}
	start();
	return true;
}

bool CalculateFileCheckThread::StartTask(const QString &filePath)
{
	return StartTask(FileCheckSum(filePath));
}

bool CalculateFileCheckThread::StartTask(const FileCheckSum &fileCheckSum)
{
	SetTask(fileCheckSum);
	return StartTask();
}

bool CalculateFileCheckThread::StopTask()
{
	stopTask = true;
	wait();
	return true;
}

bool CalculateFileCheckThread::SetTask(const QString &filePath)
{
	return SetTask(FileCheckSum(filePath));
}

bool CalculateFileCheckThread::SetTask(const FileCheckSum &fileCheckSum)
{
	this->fileCheckSum = fileCheckSum;
	taskExist = true;
	return true;
}

void CalculateFileCheckThread::run()
{
	if(taskExist && taskValid && !stopTask)
	{
		QFile inFile(fileCheckSum.fileInfo.filePath());
		if(inFile.open(QIODevice::ReadOnly))
		{
			QCryptographicHash chMd5(QCryptographicHash::Md5);
			QCryptographicHash chSHA1(QCryptographicHash::Sha1);
			QCryptographicHash chSHA256(QCryptographicHash::Sha256);
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
				chSHA256.addData(buf);
				dat = buf.data();
				while(len < bufLen)
				{
					if(stopTask)//��Ҫ������ֹ�߳�
					{
						emit sig_CalculateCompleted(taskSuccess,fileCheckSum);
						return ;
					}
					checkSum += (unsigned char)(*(dat + len));

					crc32Ret = CRC32[((crc32Ret & 0xFF) ^ (unsigned char)(*(dat + len)))] ^ (crc32Ret >> 8);

					len ++;
				}
				len = 0;
				readSize += buf.length();
			}
			crc32Ret = ~crc32Ret;
			inFile.close();
			fileCheckSum.md5 = chMd5.result().toHex().toUpper();
			fileCheckSum.sha1 = chSHA1.result().toHex().toUpper();
			fileCheckSum.sha256 = chSHA256.result().toHex().toUpper();
			fileCheckSum.checkSum = QString::number(checkSum,16).toUpper();
			fileCheckSum.crc32 = QString::number(crc32Ret,16).toUpper();
			QFileIconProvider fileIP;
			fileCheckSum.icon = fileIP.icon(fileCheckSum.fileInfo);
			fileCheckSum.bOK = true;
			taskSuccess = !stopTask;
		}
	}
	emit sig_CalculateCompleted(taskSuccess,fileCheckSum);
}

void CalculateFileCheckThread::slot_Finished()
{
	QString filePath = fileCheckSum.fileInfo.filePath();
	qDebug()<<QString("Thread has finished, reason: taskExist=%1,taskValid=%2,stopTask=%3, calculate file is : %4").arg(taskExist).arg(taskValid).arg(stopTask).arg(filePath);
	emit sig_Finished(filePath);
}

void CalculateFileCheckThread::slot_Started()
{	
	QString filePath = fileCheckSum.fileInfo.filePath();
	qDebug()<<QString("Thread has started, calculate file is : %1").arg(filePath);
}