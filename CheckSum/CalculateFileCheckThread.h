#ifndef CALCCHECKSUMTHREAD_H
#define CALCCHECKSUMTHREAD_H

#include <QThread>
#include <QFileInfo>
#include <QIcon>

struct FileCheckSum
{
	QFileInfo fileInfo;;
	QString md5;
	QString checkSum;
	QString crc32;
	QString sha1;
	bool bOK;
	QIcon icon;

	void ClearData()
	{
        md5 = "";
        checkSum = "";
        crc32 = "";
        sha1 = "";
        bOK = false;
	}

	FileCheckSum()
	{
		md5 = "";
		checkSum = "";
		crc32 = "";
		sha1 = "";
		bOK = false;
	};
	FileCheckSum(const QFileInfo &newFileInfo)
	{
#ifdef Q_OS_WIN
        this->FileCheckSum::FileCheckSum();
        fileInfo = newFileInfo;
#else
        md5 = "";
        checkSum = "";
        crc32 = "";
        sha1 = "";
        bOK = false;
        fileInfo = newFileInfo;
#endif	
    };
	FileCheckSum(const QString &filePath)
	{
#ifdef Q_OS_WIN
        this->FileCheckSum::FileCheckSum(QFileInfo(filePath));
#else
        md5 = "";
        checkSum = "";
        crc32 = "";
        sha1 = "";
        bOK = false;
        fileInfo = QFileInfo(filePath);
#endif
		//new (this)FileCheckSum(QFileInfo(filePath));//这样写也可以
		//FileCheckSum(QFileInfo(filePath));//直接调用另一个构造函数会编译不通过
	};
	//FileCheckSum(const QString &filePath):FileCheckSum(QFileInfo(filePath))//这种方式只在VS2013下有效
	//{
	//};
};

class CalculateFileCheckThread : public QThread
{
	Q_OBJECT

public:
	CalculateFileCheckThread(QObject *parent = 0);
	~CalculateFileCheckThread();
	
	bool StartTask();
	bool StartTask(const QString &filePath);
	bool StartTask(const FileCheckSum &fileCheckSum);
	bool SetTask(const QString &filePath);
	bool SetTask(const FileCheckSum &fileCheckSum);

	//停止当前任务
	bool StopTask();

	static unsigned int CRC32[256];

signals:
	void sig_CalculateCompleted(const bool &success, const FileCheckSum &fileCheckSum); 
	void sig_Finished(const QString  &filePath); 

private:
	FileCheckSum fileCheckSum;
	bool taskSuccess;//任务执行成功
	bool taskExist;//当前有任务
	bool taskValid;//任务有效
	bool stopTask;//结束任务

	private slots:
		void slot_Finished();
		void slot_Started();

protected:
	virtual void run();
};

#endif // CALCCHECKSUMTHREAD_H
