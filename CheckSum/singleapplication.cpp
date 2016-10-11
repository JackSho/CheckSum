#include <QLocalSocket>
#include<qdir.h>
#include<qfile.h>
#include "singleapplication.h"

SingleApplication::SingleApplication(int &argc, char *argv[], const QString uniqueKey) : QApplication(argc, argv,true), _uniqueKey(uniqueKey)
{
	sharedMemory.setKey(_uniqueKey);
	if (sharedMemory.attach())
	{
		sharedMemory.detach();
		if(sharedMemory.create(1))
		{
			_isRunning = false;
			localServer = new QLocalServer(this);
			connect(localServer, SIGNAL(newConnection()), this, SLOT(receiveMessage()));
#ifdef Q_OS_MAC
			QString serverFile=QDir::cleanPath(QDir::tempPath())+"/"+_uniqueKey;
			QFile::remove(serverFile);
			qDebug()<<serverFile;
#endif
			bool ret2=localServer->listen(_uniqueKey);
			if(ret2)
			{
				qDebug("local server listen2....");
			}
			else 
			{
				qDebug()<<"local server listen failed:"<<localServer->errorString();
				localServer->close();
			}
			return;
		}
		//sharedMemory.attach();
		_isRunning = true;
	}
	else
	{
		_isRunning = false;
		// create shared memory.
		if (!sharedMemory.create(1))
		{
			qDebug("Unable to create single instance.");
			return;
		}
		// create local server and listen to incomming messages from other instances.

		localServer = new QLocalServer(this);
		connect(localServer, SIGNAL(newConnection()), this, SLOT(receiveMessage()));
#ifdef Q_OS_MAC
		QString serverFile=QDir::cleanPath(QDir::tempPath())+"/"+_uniqueKey;
		QFile::remove(serverFile);
#endif
		bool ret=localServer->listen(_uniqueKey);
		if(ret)
		{
			qDebug("local server listen....");
		}
		else
		{
			qDebug()<<"local server listen failed:"<<localServer->errorString();
			localServer->close();
		} 
	}
}


// public slots.

void SingleApplication::receiveMessage()
{
	QLocalSocket *localSocket = localServer->nextPendingConnection();
	if (!localSocket->waitForReadyRead(timeout))
	{
		qDebug(localSocket->errorString().toLatin1());
		return;
	}
	QByteArray byteArray = localSocket->readAll();
	QString message = QString::fromUtf8(byteArray.constData());
	emit messageAvailable(message);
	localSocket->disconnectFromServer();
}

// public functions.

bool SingleApplication::isRunning()
{
	return _isRunning;
}

bool SingleApplication::sendMessage(const QString &message)
{
	if (!_isRunning)
		return false;
	QLocalSocket localSocket(this);
	localSocket.connectToServer(_uniqueKey, QIODevice::WriteOnly);
	if (!localSocket.waitForConnected(timeout))
	{
		qDebug(localSocket.errorString().toLatin1());
		return false;
	}
	localSocket.write(message.toUtf8());
	if (!localSocket.waitForBytesWritten(timeout))
	{
		qDebug(localSocket.errorString().toLatin1());
		return false;
	}
	localSocket.disconnectFromServer();
	return true;
}
