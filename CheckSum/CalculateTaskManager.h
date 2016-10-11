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

	//ɾ��ָ������,Ĭ��ɾ��ȫ������
	void DeleteCalculateTask(QString task = QString());

	//��ȡδ��ɵ����񣨰������ڼ�����������������е�����
	QList<QString> UnCompletedTask();

signals:
	void sig_CalculateSuccess(const bool &success,const FileCheckSum &fileCheckSum);
	void sig_Completed();

private:
	int maxThreadNum;;
	QMap<QString,CalculateFileCheckThread*> curTaskMap;//��ǰ���ڼ��������
	QList<QString> taskList;//��������������е����񣬻�δ��ʼ����
	void InitCRC32Table(unsigned int crc32[256]);

	private slots:
		void slot_TaskFinished(const QString  &filePath);
		void slot_CalculateCompleted(const bool &success, const FileCheckSum &fileCheckSum);
};

#endif // CALCULATETASKMANAGER_H
