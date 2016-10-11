#include <QDebug>

#include "CalculateFileCheckThread.h"
#include "CalculateTaskManager.h"

unsigned int CalculateFileCheckThread::CRC32[256];

CalculateTaskManager::CalculateTaskManager(QObject *parent)
	: QObject(parent),maxThreadNum(4)
{
	InitCRC32Table(CalculateFileCheckThread::CRC32);
}

CalculateTaskManager::~CalculateTaskManager()
{
	DeleteCalculateTask();
}

void CalculateTaskManager::InitCRC32Table(unsigned int crc32[256])
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
		crc32[i] = crc;
	}
}

void CalculateTaskManager::SetThreadNum(const unsigned int num)
{
	maxThreadNum = num;
}

int CalculateTaskManager::ThreadNum()
{
	return maxThreadNum;
}

bool CalculateTaskManager::AddCalculateTask(const FileCheckSum &fileCheckSum)
{
	QString filePath = fileCheckSum.fileInfo.filePath();
	return AddCalculateTask(filePath);
}

bool CalculateTaskManager::AddCalculateTask(const QString &filePath)
{
	if(filePath.isEmpty())
		return false;
	if(!taskList.contains(filePath))
	{
		if(!curTaskMap.contains(filePath))
		{
			taskList.append(filePath);
		}
	}
	Start();
	return true;
}

void CalculateTaskManager::slot_TaskFinished(const QString  &filePath)
{
	qDebug()<<QString("One task has finished, task is : %1").arg(filePath);
	CalculateFileCheckThread *thread = curTaskMap.take(filePath);
	delete (thread);
	Start();
}

void CalculateTaskManager::Start()
{
	while(curTaskMap.count() < maxThreadNum && !taskList.isEmpty())
	{
		QString filePath = taskList.takeFirst();
		CalculateFileCheckThread *thread = new CalculateFileCheckThread(this);
		connect(thread,SIGNAL(sig_Finished(const QString  &)),SLOT(slot_TaskFinished(const QString  &)),Qt::DirectConnection);
		connect(thread,SIGNAL(sig_CalculateCompleted(const bool &, const FileCheckSum &)),this,SLOT(slot_CalculateCompleted(const bool &, const FileCheckSum &)),Qt::QueuedConnection);
		curTaskMap.insert(filePath,thread);
		thread->StartTask(filePath);
	}
	if(curTaskMap.isEmpty() && taskList.isEmpty())
	{
		emit sig_Completed();
	}
}

void CalculateTaskManager::slot_CalculateCompleted(const bool &success, const FileCheckSum &fileCheckSum)
{
	emit sig_CalculateSuccess(success,fileCheckSum);
}

QList<QString> CalculateTaskManager::UnCompletedTask()
{
	QList<QString> retTaskList;
	retTaskList.append(taskList);	
	QMap<QString,CalculateFileCheckThread*>::const_iterator it = curTaskMap.begin();
	while(it != curTaskMap.end())
	{
		if(!retTaskList.contains(it.key()))
		{
			retTaskList.push_front(it.key());
		}
		it++;
	}
	return retTaskList;
}

void CalculateTaskManager::DeleteCalculateTask(QString task)
{
	if(task.length())//删除指定任务，如果指定任务正在计算，则立即停止计算并删除
	{
		CalculateFileCheckThread *thread = curTaskMap.take(task);
		if(thread)
		{
			thread->StopTask();
			//delete (thread);
		}
		taskList.removeOne(task);
	}
	else//删除全部任务
	{		
		QMap<QString,CalculateFileCheckThread*>::const_iterator it = curTaskMap.begin();
		while(it != curTaskMap.end())
		{
			it.value()->StopTask();
			//delete (it.value());
			it++;
		}
		taskList.clear();
	}
}