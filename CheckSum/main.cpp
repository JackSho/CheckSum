
#include <QtWidgets/QApplication>


#include "checksum.h"
#include "singleapplication.h"

#ifndef QT_DLL
#include <QtPlugin>
#endif


#if defined Q_OS_WIN32   //for win  
#include <windows.h>  
bool checkOnly()  
{  
	//  创建互斥量  
	HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"MyCheckSumTool" );  
	//  检查错误代码  
	if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  
	{  
		//  如果已有互斥量存在则释放句柄并复位互斥量  
		CloseHandle(m_hMutex);  
		m_hMutex  =  NULL;  
		//  程序退出  
		return  false;  
	}  
	else  
		return true;  
}  
#endif
#if defined  Q_OS_LINUX   //for linux  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
bool checkOnly()  
{  
    const char filename[]  = "/tmp/lockfile";  
    int fd = open (filename, O_WRONLY | O_CREAT , 0644);  
    int flock = lockf(fd, F_TLOCK, 0 );  
    if (fd == -1) {  
            perror("open lockfile/n");  
            return false;  
    }  
    //给文件加锁  
    if (flock == -1) {  
            perror("lock file error/n");  
            return false;  
    }  
    //程序退出后，文件自动解锁  
    return true;  
}  
#endif  




#define	ONLYTOOLMUTEXNAME		L"MyCheckSumTool"
#define SINGLEAPPUNIQUEKEY		"CheckSum Tool Single Application"
int main(int argc, char *argv[])
{
#ifndef QT_DLL
	Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
#endif
	SingleApplication *sA = NULL;
    
#ifdef Q_OS_WIN
    
	HANDLE hMutex = ::CreateMutex(NULL,TRUE,ONLYTOOLMUTEXNAME);
	DWORD dwErr = ::GetLastError();
	if(dwErr == ERROR_ALREADY_EXISTS)
	{
		::WaitForSingleObject(hMutex,INFINITE);
		sA = new SingleApplication(argc, argv,SINGLEAPPUNIQUEKEY);
		::ReleaseMutex(hMutex);
		::CloseHandle(hMutex);
		hMutex = NULL;
	}
	else if(hMutex && dwErr == ERROR_SUCCESS)
	{
		sA = new SingleApplication(argc, argv,SINGLEAPPUNIQUEKEY);
		::ReleaseMutex(hMutex);
	}
    
#else
    
    sA = new SingleApplication(argc, argv,SINGLEAPPUNIQUEKEY);
    
#endif
    
	int i=0;
	QStringList argumentList;
	QString argument("Start again");
	if(argc>1)
	{
		argument.clear();
	};
	while(++i <argc)
	{
		argumentList.append(QString::fromLocal8Bit(argv[i]));
	}
	i = 0;
	bool serverLive = true;
	//serverLive = !checkOnly();
	while(i < argumentList.count() && serverLive)
	{
		argument += argumentList.at(i) + "\n\n";
		if(++i % 10 == 0 )
		{
			if(sA->isRunning())
			{
				sA->sendMessage(argument.trimmed());
			}
			else
			{
				serverLive = false;
			}
			argument.clear();
		}
	}
	if(serverLive)
	{
		if(sA->isRunning())
		{
			sA->sendMessage(argument.trimmed());
			return 0;
		}
		else
		{
			serverLive = false;
		}
	}

	argument.clear();
	while(argumentList.count())
	{
		argument += argumentList.takeFirst() + "\n\n";
	}
	CheckSum w(argument.trimmed());
	QObject::connect(sA,SIGNAL(messageAvailable(QString)),&w,SLOT(receiveMessage(QString)));
	w.show();
	int code = sA->exec();
	delete (sA);
    
#ifdef Q_OS_WIN
    
	if(hMutex == NULL)
	{
		::CloseHandle(hMutex);
		hMutex = NULL;
	}
    
#else
    
#endif
    
	return code;
}