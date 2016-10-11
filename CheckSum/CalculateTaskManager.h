#ifndef CALCULATETASKMANAGER_H
#define CALCULATETASKMANAGER_H

#include <QObject>
#include <QMap>

#include "CalculateFileCheckThread.h"

class CalculateTaskManager : public QObject
{
	Q_OBJECT

public:
	CalculateTaskManager(QObject *parent = 0);
	~CalculateTaskManager();

	void SetThreadNum(const unsigned int num);
	int ThreadNum();
	
	bool AddCalculateTask(const QString &filePath);
	bool AddCalculateTask(const FileCheckSum &fileCheckSum);
	void Start();

	//删除指定任务,默认删除全部任务
	void DeleteCalculateTask(QString task = QString());

	//获取未完成的任务（包括正在计算的任务和任务队列中的任务）
	QList<QString> UnCompletedTask();

signals:
	void sig_CalculateSuccess(const bool &success,const FileCheckSum &fileCheckSum);
	void sig_Completed();

private:
	int maxThreadNum;;
	QMap<QString,CalculateFileCheckThread*> curTaskMap;//当前正在计算的任务
	QList<QString> taskList;//还处于任务队列中的任务，还未开始计算
	void InitCRC32Table(unsigned int crc32[256]);

	private slots:
		void slot_TaskFinished(const QString  &filePath);
		void slot_CalculateCompleted(const bool &success, const FileCheckSum &fileCheckSum);
};

#endif // CALCULATETASKMANAGER_H
