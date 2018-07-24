#include <QDirModel>
#include <QFile>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>
#include <QToolTip>
#include <QMenu>
#include <QDesktopServices>
#include <QTextCodec>
#include <QDir>
#include <QProcess>

#ifdef Q_OS_WIN

#include <QtWin>
#include <qt_windows.h>
#include <ShlObj.h>
#include <Windowsx.h >

#endif

#ifdef Q_OS_MAC

#include <AppKit/NSWorkspace.h>
#include <CoreServices/CoreServices.h>

#endif

#include "checksum.h"
#include "CalculateFileCheckThread.h"


CheckSum::CheckSum(QString initFileList, QWidget *parent)
	: QMainWindow(parent),
	waitExit(false),
	iSelectedIndex(-1),
	press(false),
	currentLower(false),
	dirViewRightMenu(NULL),
	actAddThisFile(NULL),
	actAddChildFiles(NULL),
	actAddAllChildFiles(NULL),
	tableRecordsRightMenu(NULL),
	actOpenFileLocation(NULL),
	actOpenFile(NULL),
	actDeleteSelectedFiles(NULL),
	actRefreshSelectedFiles(NULL)
{
    ui.setupUi(this);

#ifdef Q_OS_WIN

	if (QtWin::isCompositionEnabled())                           //返回DWM组合状态
	{
		QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);    //玻璃效果
		QtWin::enableBlurBehindWindow(this);
		setAttribute(Qt::WA_TranslucentBackground, true);         //半透明背景
		setAttribute(Qt::WA_NoSystemBackground, false);           //禁用无背景
		setStyleSheet("CheckSum { background: transparent; }");
	}
	else
	{
		QtWin::resetExtendedFrame(this);
		setAttribute(Qt::WA_TranslucentBackground, false);
		setStyleSheet(QString("CheckSum { background: %1; }").arg(QtWin::realColorizationColor().name()));
	}
    
#endif
    
	model = new QDirModel(this);

	setAcceptDrops(true);

	ui.treeView_explorer->setSortingEnabled(true);
	ui.treeView_explorer->setModel(model);  
    ui.treeView_explorer->setRootIndex(model->index(""));
	
	ui.widget_Left->layout()->setMargin(0);
	ui.widget_icon->layout()->setMargin(0);

	ui.checkBox_autoWidth->setCheckState(Qt::Checked);
	
	connect(ui.checkBox_autoWidth,SIGNAL(stateChanged(int)),this,SLOT(slot_AutoWidthStateChanged(int)));
	connect(ui.checkBox_lower,SIGNAL(stateChanged(int)),this,SLOT(slot_LowerStateChanged(int)));

	connect(ui.treeView_explorer,SIGNAL(doubleClicked(const QModelIndex &)),SLOT(slot_DoubleClicked(const QModelIndex &)),Qt::DirectConnection);
	connect(ui.pushButton_refresh,SIGNAL(clicked(bool)),SLOT(slot_RefreshExplorer(bool)),Qt::DirectConnection);
	connect(ui.pushButton_add,SIGNAL(clicked(bool)),SLOT(slot_AddToRecordList(bool)),Qt::DirectConnection);
	connect(ui.pushButton_delete,SIGNAL(clicked(bool)),SLOT(slot_DeleteFromRecordList(bool)),Qt::DirectConnection);
	connect(ui.pushButton_deleteAll,SIGNAL(clicked(bool)),SLOT(slot_DeleteAllFromRecordList(bool)),Qt::DirectConnection);
	connect(ui.pushButton_refreshSelected,SIGNAL(clicked(bool)),SLOT(slot_RefreshSelectedRecordList(bool)),Qt::DirectConnection);
	connect(ui.pushButton_refreshAll,SIGNAL(clicked(bool)),SLOT(slot_RefreshAllFromRecordList(bool)),Qt::DirectConnection);
	connect(ui.pushButton_exit,SIGNAL(clicked(bool)),SLOT(slot_Exit(bool)),Qt::DirectConnection);
	connect(ui.pushButton_hideShowLeft,SIGNAL(clicked(bool)),SLOT(slot_HideShowLeft(bool)),Qt::DirectConnection);

	tableRecord = new QStandardItemModel();
	tableRecord->setHorizontalHeaderItem(CheckSum::FileName, new QStandardItem("File Name"));
	tableRecord->setHorizontalHeaderItem(CheckSum::Checksum, new QStandardItem("CheckSum"));
	tableRecord->setHorizontalHeaderItem(CheckSum::CRC32, new QStandardItem("CRC32"));
	tableRecord->setHorizontalHeaderItem(CheckSum::MD5, new QStandardItem("MD5"));
	tableRecord->setHorizontalHeaderItem(CheckSum::FilePath, new QStandardItem("File Path"));

	//利用setModel()方法将数据模型与QTableView绑定
	ui.tableView_records->setModel(tableRecord);
	ui.tableView_records->setIconSize(QSize(20,20));
	ui.tableView_records->setMouseTracking(true);
	ui.tableView_records->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_records->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.tableView_records->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableView_records->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.tableView_records->horizontalHeader()->setSectionResizeMode(ui.checkBox_autoWidth->checkState() == Qt::Checked ? QHeaderView::ResizeToContents : QHeaderView::Interactive);
	ui.tableView_records->setSortingEnabled(true);
	ui.tableView_records->setAlternatingRowColors(true);

	connect(ui.tableView_records,SIGNAL(entered(const QModelIndex &)),this,SLOT(slot_ShowToolTip(const QModelIndex &)),Qt::DirectConnection);
	connect(ui.tableView_records, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slot_OpenFile(const QModelIndex &)));
	connect(ui.tableView_records->selectionModel(),SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),SLOT(slot_CurrentRowChanged(const QModelIndex &, const QModelIndex &)),Qt::DirectConnection);

	connect(&calcManager,SIGNAL(sig_CalculateSuccess(const bool &,const FileCheckSum &)),SLOT(slot_CalculateSuccess(const bool &,const FileCheckSum &)),Qt::DirectConnection);
	connect(&calcManager,SIGNAL(sig_Completed()),SLOT(slot_Completed()),Qt::DirectConnection);
	
	ui.label_Percent->setText(QString("<font size=\"5\">Completed: %1/%2</font>").arg(0).arg(0));

	int width = ui.widget_icon->width();
	width = 128;
	ui.widget_icon->setFixedSize(width,width);
	ui.widget_icon->setAutoFillBackground(true);
	ui.widget_icon->setPalette(QPalette(Qt::transparent));
	ui.widget_BigIcon->setAutoFillBackground(true);
			
	ui.pushButton_refreshAll->setEnabled(false);
	ui.pushButton_refreshSelected->setEnabled(false);

	CreateMenuActions();

	slot_HideShowLeft();

	UpdateBigIcon();

	if(initFileList.length() > 0)
	{
		QStringList fileList = initFileList.split("\n\n");
		int exist = 0;
		while(fileList.count())
		{
			QString path = fileList.takeFirst();
			QFileInfo fileInfo(path);
			if(fileInfo.isFile())
			{
				if(!mapRecords.contains(fileInfo.filePath()))
				{
					calcManager.AddCalculateTask(fileInfo.filePath());
				}
				else
				{
					exist ++;
				}
			}
		}
		if(exist)
		{
			QMessageBox::warning(this,this->windowTitle(),"The file is already in the list!");
		}
	}
}

CheckSum::~CheckSum()
{
	delete (model);
	delete (tableRecord);
	DestoryMenuActions();
}

void CheckSum::slot_AutoWidthStateChanged(int state)
{
	ui.tableView_records->horizontalHeader()->setSectionResizeMode(state == Qt::Checked ? QHeaderView::ResizeToContents : QHeaderView::Interactive);
}

void CheckSum::slot_LowerStateChanged(int state)
{
	currentLower = (state == Qt::Checked);

	ui.lineEdit_MD5->setText("0x" + (currentLower ? ui.lineEdit_MD5->text().mid(2).toLower() : ui.lineEdit_MD5->text().mid(2).toUpper()));
	ui.lineEdit_checkSum->setText("0x" + (currentLower ? ui.lineEdit_checkSum->text().mid(2).toLower() : ui.lineEdit_checkSum->text().mid(2).toUpper()));
	ui.lineEdit_CRC32->setText("0x" + (currentLower ? ui.lineEdit_CRC32->text().mid(2).toLower() : ui.lineEdit_CRC32->text().mid(2).toUpper()));
	ui.lineEdit_SHA1->setText("0x" + (currentLower ? ui.lineEdit_SHA1->text().mid(2).toLower() : ui.lineEdit_SHA1->text().mid(2).toUpper()));
}

void CheckSum::slot_RefreshExplorer(bool checked)
{
	model->refresh();
}

void CheckSum::slot_AddToRecordList(bool checked)
{
	QModelIndex index = ui.treeView_explorer->currentIndex();
	if(index.isValid())
	{
		slot_DoubleClicked(index);
	}
}

void CheckSum::slot_DeleteFromRecordList(bool checked)
{
	QModelIndexList rowModelIndexList = ui.tableView_records->selectionModel()->selectedRows();
	while(rowModelIndexList.count())
	{
		int rowIndex = rowModelIndexList.takeLast().row();
		QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
		calcManager.DeleteCalculateTask(filePath);
		mapRecords.remove(filePath);
		tableRecord->removeRow(rowIndex);
	}
	ui.tableView_records->clearSelection();
	if(tableRecord->rowCount() == 0)
	{
		SetCheckSumNull();
		ui.pushButton_refreshSelected->setEnabled(false);
	}
	SetTaskPercent();
	UpdateRefreshAllEnable();
}

void CheckSum::slot_DeleteAllFromRecordList(bool checked)
{
	tableRecord->removeRows(0,tableRecord->rowCount());
	calcManager.DeleteCalculateTask();
	mapRecords.clear();
	SetCheckSumNull();
	SetTaskPercent();
	ui.pushButton_refreshAll->setEnabled(false);
	ui.pushButton_refreshSelected->setEnabled(false);
}

void CheckSum::slot_RefreshSelectedRecordList(bool checked)
{
	QModelIndexList rowModelIndexList = ui.tableView_records->selectionModel()->selectedRows();
	while(rowModelIndexList.count())
	{
		int rowIndex = rowModelIndexList.takeLast().row();
		QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
		QFileInfo fileInfo(filePath);
		tableRecord->setItem(rowIndex,CheckSum::FileName,new QStandardItem(fileInfo.fileName()));
		mapRecords[filePath].ClearData();
		SetCheckSum(mapRecords[filePath],rowIndex,false);
		calcManager.AddCalculateTask(filePath);
		ui.pushButton_refreshSelected->setEnabled(false);
	}
	ui.tableView_records->clearSelection();
	UpdateRefreshAllEnable();
	SetTaskPercent();
	UpdateBigIcon();
}

void CheckSum::slot_RefreshAllFromRecordList(bool checked)
{
	int rowIndex = 0;
	if(tableRecord->rowCount())
	{
		while(rowIndex < tableRecord->rowCount())
		{
			QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
			QFileInfo fileInfo(filePath);
			tableRecord->setItem(rowIndex,CheckSum::FileName,new QStandardItem(fileInfo.fileName()));
			mapRecords[filePath].ClearData();
			SetCheckSum(mapRecords[filePath],rowIndex,false);
			calcManager.AddCalculateTask(filePath);
			rowIndex ++;
		}
		ui.tableView_records->clearSelection();
		ui.pushButton_refreshAll->setEnabled(false);
		ui.pushButton_refreshSelected->setEnabled(false);
	}
	SetTaskPercent();
	UpdateBigIcon();
}

void CheckSum::slot_HideShowLeft(bool checked)
{
	static bool showLeft = true;
	static QRect rect(0,0,0,0);
	if(rect.left() == 0 && rect.right() == 0 && rect.width() == 0 && rect.height() == 0)
	{
		rect = ui.widget_Left->geometry();
	}
	if(showLeft)
	{
		ui.widget_Left->hide();
		ui.pushButton_add->setEnabled(false);
		ui.pushButton_hideShowLeft->setText("Show Left");
	}
	else
	{
		ui.widget_Left->show();
		ui.pushButton_add->setEnabled(true);
		ui.pushButton_hideShowLeft->setText("Hide Left");
	}
	showLeft = !showLeft;
}

void CheckSum::slot_Exit(bool checked)
{
	close();
}

void CheckSum::slot_DoubleClicked(const QModelIndex & index)
{
	QFileInfo fileInfo = model->fileInfo(index);
	if(fileInfo.isFile())
	{
		if(!mapRecords.contains(fileInfo.filePath()))
		{
			FileCheckSum fileCheckSum(fileInfo);
			SetCheckSum(fileCheckSum,-1,false);
			calcManager.AddCalculateTask(fileCheckSum);
		}
	}
}

void CheckSum::SetCheckSum(const FileCheckSum &fileCheckSum,int rowIndex,bool changeCurrent)
{
	ui.lineEdit_fileName->setText(fileCheckSum.fileInfo.fileName());
	ui.lineEdit_MD5->setText("0x" + (currentLower ? fileCheckSum.md5.toLower() : fileCheckSum.md5.toUpper()));
	ui.lineEdit_checkSum->setText("0x" + (currentLower ? fileCheckSum.checkSum.toLower() : fileCheckSum.checkSum));
	ui.lineEdit_CRC32->setText("0x" + (currentLower ? fileCheckSum.crc32.toLower() : fileCheckSum.crc32));
	ui.lineEdit_SHA1->setText("0x" + (currentLower ? fileCheckSum.sha1.toLower() : fileCheckSum.sha1));
	ui.lineEdit_Size->setText(QString("%L1").arg(fileCheckSum.fileInfo.size()) + " Bytes");
	
	int rowCount = tableRecord->rowCount();
	rowCount = rowIndex == -1 ? rowCount : ((rowIndex >= 0 && rowIndex < rowCount) ? rowIndex : rowCount);
	if(fileCheckSum.bOK)
	{
		tableRecord->setItem(rowCount,CheckSum::FileName,new QStandardItem(fileCheckSum.icon,fileCheckSum.fileInfo.fileName()));
		UpdateBigIcon(fileCheckSum.icon);
	}
	else
	{
		tableRecord->setItem(rowCount,CheckSum::FileName,new QStandardItem(fileCheckSum.fileInfo.fileName()));
		UpdateBigIcon();
	}
	tableRecord->setItem(rowCount,CheckSum::Checksum,new QStandardItem("0x"+fileCheckSum.checkSum));
	tableRecord->setItem(rowCount,CheckSum::CRC32,new QStandardItem("0x"+fileCheckSum.crc32));
	tableRecord->setItem(rowCount,CheckSum::MD5,new QStandardItem("0x"+fileCheckSum.md5));
	tableRecord->setItem(rowCount,CheckSum::FilePath,new QStandardItem(fileCheckSum.fileInfo.filePath()));
	mapRecords[fileCheckSum.fileInfo.filePath()] = fileCheckSum;
	ui.tableView_records->setFocus(Qt::MouseFocusReason);
	ui.tableView_records->activateWindow();
	if(changeCurrent == true)
	{
		iSelectedIndex = rowCount;
		ui.tableView_records->selectRow(rowCount);
	}
	SetTaskPercent();
}

void CheckSum::SetCheckSumNull()
{
	ui.lineEdit_fileName->clear();
	ui.lineEdit_MD5->clear();
	ui.lineEdit_checkSum->clear();
	ui.lineEdit_CRC32->clear();
	ui.lineEdit_SHA1->clear();
	ui.lineEdit_Size->clear();
	UpdateBigIcon();
}

void CheckSum::dragEnterEvent(QDragEnterEvent *event)
{
#ifdef QT_DEBUG
    QStringList formatList = event->mimeData()->formats();
    while(formatList.count())
    {
        qDebug() << QString("QStringList formatList has: %1").arg(formatList.takeFirst());
    }
#endif

    if(event->mimeData()->hasUrls())
	//if (event->mimeData()->hasFormat("text/uri-list"))
		event->acceptProposedAction();
}

void CheckSum::dropEvent(QDropEvent *event)
{
#ifdef QT_DEBUG
    qDebug() << "CheckSum::dropEvent enter.";
#endif

	bool needConvertFileUrl = false;
	
#ifdef  Q_OS_MAC
	SInt32 macVersionMajor;
	SInt32 macVersionMinor;
	if ((Gestalt(gestaltSystemVersionMajor, &macVersionMajor) == noErr) && (Gestalt(gestaltSystemVersionMinor, &macVersionMinor) == noErr))
	{
		
#ifdef QT_DEBUG
		qDebug() << QString("CheckSum::dropEvent enter. macVersionMajor = %1, macVersionMinor = %2").arg(macVersionMajor).arg(macVersionMinor);
#endif
		///System/Library/CoreServices/SystemVersion.plist  Could look here, but this is easier.
		if (macVersionMajor >= 10 && macVersionMinor >= 10) // need at least v10.10.0 of Mac OS X
		{
			needConvertFileUrl = true;
		}
	}
#endif
	
	QList<QUrl> urls = event->mimeData()->urls();
	int exist = 0;
	for(int i=0;i<urls.count();i++)
	{
		QUrl url = urls.at(i);
#ifdef Q_OS_MAC
        if(needConvertFileUrl)
        {
            LeawoQUrlToCFURLRef(url);
        }
#endif
        QString path =url.toLocalFile();
        
#ifdef QT_DEBUG
        qDebug() <<QString("toLocalFile is %1").arg(url.toLocalFile());
        qDebug() <<QString("toDisplayString is %1").arg(url.toDisplayString());
        qDebug() <<QString("topLevelDomain is %1").arg(url.topLevelDomain());
        qDebug() <<QString("toString is %1").arg(url.toString());
#endif
        
		QFileInfo fileInfo(path);
		if(fileInfo.isFile())
		{
			if(!mapRecords.contains(fileInfo.filePath()))
			{
				FileCheckSum fileCheckSum(fileInfo);
				SetCheckSum(fileCheckSum,-1,false);
				calcManager.AddCalculateTask(fileCheckSum);
			}
			else
			{
				exist ++;
			}
		}
	}
	if(exist)
	{
		QMessageBox::warning(this,this->windowTitle(),"The file is already in the list!");
	}
	return ;
} 

void CheckSum::mousePressEvent(QMouseEvent *event)
{
#ifdef QT_DEBUG
    
    qDebug() << "CheckSum::mousePressEvent enter.";
    
#endif
    
	if(event->button() == Qt::LeftButton)
	{
		press = true;
		relativePos=this->pos()-event->globalPos();
	}
}

void CheckSum::mouseMoveEvent(QMouseEvent *event)
{
	if(press)
		this->move(event->globalPos()+ relativePos);
}

void CheckSum::mouseReleaseEvent(QMouseEvent *event)
{
	press = false;
}

void CheckSum::slot_CurrentRowChanged(const QModelIndex & current, const QModelIndex & previous)
{
	if(iSelectedIndex >= 0)
	{
		iSelectedIndex = -1;
	}
	else
	{
		if(current.isValid())
		{
			int rowIndex = current.row();
			QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
			FileCheckSum fileCheckSum = mapRecords[filePath];
			SetCheckSum(fileCheckSum,rowIndex);
			ui.pushButton_refreshSelected->setEnabled(fileCheckSum.bOK);
		}
	}
}

void CheckSum::slot_CalculateSuccess(const bool &success,const FileCheckSum &fileCheckSum)
{
	QString filePath = fileCheckSum.fileInfo.filePath();
	QList<QStandardItem *> findList = tableRecord->findItems(filePath,Qt::MatchExactly,CheckSum::FilePath);
	if(success)
	{
		if(findList.isEmpty())
		{
			SetCheckSum(fileCheckSum);
		}
		else if(findList.count() == 1)
		{
			int rowIndex = findList.first()->row();
			SetCheckSum(fileCheckSum,rowIndex);
		}
		ui.pushButton_refreshSelected->setEnabled(fileCheckSum.bOK);
	}
	else
	{
		if(findList.count() == 1)
		{
			int rowIndex = findList.first()->row();
			tableRecord->removeRow(rowIndex);
			SetCheckSumNull();
		}
		mapRecords.remove(filePath);
		SetTaskPercent();
	}
	UpdateRefreshAllEnable();
}

void CheckSum::slot_Completed()
{
	if(waitExit)
	{
		close();
		return ;
	}
	UpdateRefreshAllEnable();
}

void CheckSum::receiveMessage(QString message)
{
#ifdef Q_OS_WIN
    
	HWND id = (HWND)QWidget::winId();
	AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), NULL), GetCurrentThreadId(), true) ;
	SetForegroundWindow(id) ;
	SetFocus(id) ;
	AttachThreadInput(GetWindowThreadProcessId(GetForegroundWindow(), NULL), GetCurrentThreadId(), false) ;
    
#endif
    
	ui.centralWidget->activateWindow();
	setWindowState(Qt::WindowActive); 

	if(message.length() > 0 && !waitExit)
	{
		QStringList fileList = message.split("\n\n");
		int exist = 0;
		while(fileList.count())
		{
			QString path = fileList.takeFirst();
			QFileInfo fileInfo(path);
			if(fileInfo.isFile())
			{
				if(!mapRecords.contains(fileInfo.filePath()))
				{
					qDebug()<<QString("Receive a new task : %1").arg(fileInfo.filePath());
					FileCheckSum fileCheckSum(fileInfo);
					SetCheckSum(fileCheckSum,-1,false);
					calcManager.AddCalculateTask(fileCheckSum);
				}
				else
				{
					exist ++;
				}
			}
		}
		if(exist)
		{
			//QMessageBox::warning(this,this->windowTitle(),"The file is already in the list!");
		}
	}
}

void CheckSum::closeEvent(QCloseEvent *event)
{
	QList<QString> uncompleteTaksk = calcManager.UnCompletedTask();
	if(!uncompleteTaksk.isEmpty())
	{
		waitExit = true;
		calcManager.DeleteCalculateTask();//用户正在关闭程序，删除所有未完成任务
		event->ignore();
		return ;
	}
	event->accept();
}

void CheckSum::slot_ShowToolTip(const QModelIndex &index)
{
	if(!index.isValid())
		return;
	int rowIndex = index.row();
	QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();

	if(filePath.isEmpty())
		return;
	FileCheckSum fileCheckSum = mapRecords[filePath];
	double size = fileCheckSum.fileInfo.size();
	QString sizeUnit = "B";
	double fileSize = fileCheckSum.fileInfo.size() / 1024.0;
	if(fileSize >= 1.0)
	{
		sizeUnit = "KB";
		size = fileSize;
		fileSize = fileSize / 1024.0;
		if(fileSize >= 1.0)
		{
			sizeUnit = "MB";
			size = fileSize;
			fileSize = fileSize / 1024.0;
			if(fileSize >= 1.0)
			{
				sizeUnit = "GB";
				size = fileSize;
				fileSize = fileSize / 1024.0;
				if(fileSize >= 1.0)
				{
					sizeUnit = "TB";
					size = fileSize;
					fileSize = fileSize / 1024.0;
					if(fileSize >= 1.0)
					{
						sizeUnit = "PB";
						size = fileSize;
						fileSize = fileSize / 1024.0;
						if(fileSize >= 1.0)
						{
							sizeUnit = "EB";
							size = fileSize;
							fileSize = fileSize / 1024.0;
							if(fileSize >= 1.0)
							{
								sizeUnit = "ZB";
								size = fileSize;
								fileSize = fileSize / 1024.0;
							}
						}
					}
				}
			}
		}
	}
	QString toopTip = "File: " + filePath + 
		"\nCheckSum: 0x" + fileCheckSum.checkSum + 
		"\nMd5: 0x" + fileCheckSum.md5 + 
		"\nSHA1: 0x" + fileCheckSum.sha1 +
		"\nSize: " + QString("%1 %2").arg(QString::number(size,'f',2)).arg(sizeUnit);
		//"\nSize: " + QString("%L1").arg(fileCheckSum.fileInfo.size()) + " Bytes";//以字节为单位，千分位表示文件大小(Size: 10 234 Bytes)

	QToolTip::showText(QCursor::pos(), toopTip); 
}

void CheckSum::slot_OpenFile(const QModelIndex &index)
{
	if(!index.isValid())
		return;
	int rowIndex = index.row();
	QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
	
	if(filePath.isEmpty())
		return;
	
	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void CheckSum::SetTaskPercent()
{	
	const static QString completedText("<font size=\"5\">Completed: %1/%2</font>");
	const static QString uncompletedText("<font size=\"5\" color=\"red\">Completed: %1/%2</font>");
	int taskNum = mapRecords.count();
	int completedNum = 0;
	QMap<QString,FileCheckSum>::const_iterator it = mapRecords.begin();
	while(it != mapRecords.end())
	{
		if(it->bOK)
		{
			completedNum++;
		}
		it ++;
	}
	QString percentText;
	if(completedNum < taskNum)
	{
		percentText = uncompletedText.arg(completedNum).arg(taskNum);
	}
	else
	{
		percentText = completedText.arg(completedNum).arg(taskNum);
	}
	ui.label_Percent->setText(percentText);
}

void CheckSum::contextMenuEvent(QContextMenuEvent *event)
{
	if(ui.treeView_explorer->underMouse())
	{
		QModelIndex index = ui.treeView_explorer->currentIndex();
		QFileInfo fileInfo = model->fileInfo(index);
		if(fileInfo.isFile())
		{	
#ifdef QT_DEBUG
			qDebug() << QString("%1 is File.").arg(fileInfo.filePath());
#endif // QT_DEBUG

			dirViewRightMenu->addAction(actAddThisFile);
			dirViewRightMenu->exec(QCursor::pos());
			event->accept();
		}
		else if(fileInfo.isDir())
		{
#ifdef QT_DEBUG
			qDebug() << QString("%1 is Directory.").arg(fileInfo.filePath());
#endif // QT_DEBUG
			dirViewRightMenu->addAction(actAddChildFiles);
			dirViewRightMenu->addAction(actAddAllChildFiles);
			dirViewRightMenu->exec(QCursor::pos());
			event->accept();
		}
		dirViewRightMenu->clear();
	}
	else if(ui.tableView_records->underMouse())
	{
		//QPoint pt = ui.tableView_records->mapFromGlobal(QCursor::pos());
		//pt -= QPoint(ui.tableView_records->verticalHeader()->width(),ui.tableView_records->horizontalHeader()->height());
		//int rowIndex = ui.tableView_records->indexAt(pt).row();
		int rowIndex = ui.tableView_records->currentIndex().row();
		if(rowIndex>-1)
		{
			tableRecordsRightMenu->exec(QCursor::pos());
			event->accept();
			//event->ignore();
		}
	}
}

void CheckSum::UpdateRefreshAllEnable()
{
	int completedNum = 0;
	QMap<QString,FileCheckSum>::const_iterator it = mapRecords.begin();
	while(it != mapRecords.end())
	{
		if(it->bOK)
		{
			completedNum++;
			break;
		}
		it++;
	}
	if(completedNum > 0)
	{
		ui.pushButton_refreshAll->setEnabled(true);
	}
	else
	{
		ui.pushButton_refreshAll->setEnabled(false);
	}
}

void CheckSum::CreateMenuActions()
{
	actAddThisFile = new QAction("Add This File",this);
	actAddChildFiles = new QAction("Add Child Files",this);	
	actAddAllChildFiles = new QAction("Add All Child Files",this);
    connect(actAddThisFile, SIGNAL(triggered(bool)), this, SLOT(slot_ActionAddThisFileTriggered(bool)));
    connect(actAddChildFiles, SIGNAL(triggered(bool)), this, SLOT(slot_ActionAddChildFilesTriggered(bool)));
    connect(actAddAllChildFiles, SIGNAL(triggered(bool)), this, SLOT(slot_ActionAddAllChildFilesTriggered(bool)));
	dirViewRightMenu = new QMenu(this);
	
	tableRecordsRightMenu = new QMenu(this);

    actOpenFileLocation = new QAction("Open file location",this);
    connect(actOpenFileLocation, SIGNAL(triggered(bool)), this, SLOT(slot_ActionOpenFileLocationTriggered(bool)));
    tableRecordsRightMenu->addAction(actOpenFileLocation);
    
	actOpenFile = new QAction("Open",this);
	connect(actOpenFile, SIGNAL(triggered(bool)), this, SLOT(slot_ActionOpenFileTriggered(bool)));
	tableRecordsRightMenu->addAction(actOpenFile);

	actDeleteSelectedFiles = new QAction("Remove",this);
	connect(actDeleteSelectedFiles, SIGNAL(triggered(bool)), this, SLOT(slot_ActionDeleteSelectedFilesTriggered(bool)));
	tableRecordsRightMenu->addAction(actDeleteSelectedFiles);

	actRefreshSelectedFiles = new QAction("Refresh",this);
	connect(actRefreshSelectedFiles, SIGNAL(triggered(bool)), this, SLOT(slot_ActionRefreshSelectedFilesTriggered(bool)));
	tableRecordsRightMenu->addAction(actRefreshSelectedFiles);
}

void CheckSum::DestoryMenuActions()
{
	if(actAddThisFile)
	{
		delete (actAddThisFile);
	}
	if(actAddChildFiles)
	{
		delete (actAddChildFiles);
	}
	if(actAddAllChildFiles)
	{
		delete (actAddAllChildFiles);
	}
	if(dirViewRightMenu)
	{
		delete (dirViewRightMenu);
	}
	if(actOpenFileLocation)
	{
		delete (actOpenFileLocation);
	}
	if(actOpenFile)
	{
		delete (actOpenFile);
	}
	if(tableRecordsRightMenu)
	{
		delete (tableRecordsRightMenu);
	}
	if(actDeleteSelectedFiles)
	{
		delete (actDeleteSelectedFiles);
	}
}

void CheckSum::slot_ActionAddThisFileTriggered(bool checked)
{
	slot_AddToRecordList(checked);
}

void CheckSum::AddDirChildFiles(QString path,bool recursive)
{
	QDir dir(path);
	if(dir.exists())
	{
		dir.setFilter(QDir::Dirs|QDir::Files);
		dir.setSorting(QDir::DirsFirst);
		QFileInfoList list = dir.entryInfoList();
		int i=0;
		do{
			QFileInfo fileInfo = list.at(i);
			if(fileInfo.fileName()=="." || fileInfo.fileName()=="..")
			{
				i++;
				continue;
			}
			if(fileInfo.isDir())
			{
				if(recursive)
				{
					AddDirChildFiles(fileInfo.filePath(),recursive);
				}
			}
			else if(fileInfo.isFile())
			{
				if(!mapRecords.contains(fileInfo.filePath()))
				{
					FileCheckSum fileCheckSum(fileInfo);
					SetCheckSum(fileCheckSum,-1,false);
					calcManager.AddCalculateTask(fileCheckSum);
				}
			}
			i++;
		}
		while(i<list.size());
	}
}

void CheckSum::slot_ActionAddChildFilesTriggered(bool checked)
{
	QModelIndex index = ui.treeView_explorer->currentIndex();
	QFileInfo fileInfo = model->fileInfo(index);
	if(fileInfo.isDir())
	{
		AddDirChildFiles(fileInfo.filePath(),false);
	}
}

void CheckSum::slot_ActionAddAllChildFilesTriggered(bool checked)
{
	QModelIndex index = ui.treeView_explorer->currentIndex();
	QFileInfo fileInfo = model->fileInfo(index);
	if(fileInfo.isDir())
	{
		AddDirChildFiles(fileInfo.filePath(),true);
	}
}

void CheckSum::slot_ActionOpenFileLocationTriggered(bool checked)
{
	QModelIndexList rowModelIndexList = ui.tableView_records->selectionModel()->selectedRows();
	while(rowModelIndexList.count())
	{
		int rowIndex = rowModelIndexList.takeLast().row();
		QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
		OpenFolderAndSelectFile(QTextCodec::codecForLocale()->fromUnicode(filePath.replace('/','\\')).data());

		//QString param(QString("/select, ") + fileInfo.absoluteFilePath().replace('/','\\'));
		//ShellExecuteA(0,"open","explorer.exe",codec->fromUnicode(param).data(),NULL,SW_SHOWNORMAL);//功能不够完善（每次都打开窗口）
	}
}

void CheckSum::slot_ActionOpenFileTriggered(bool checked)
{
	QModelIndexList rowModelIndexList = ui.tableView_records->selectionModel()->selectedRows();
	while(rowModelIndexList.count())
	{
		slot_OpenFile(rowModelIndexList.takeLast());
	}
}

void CheckSum::slot_ActionDeleteSelectedFilesTriggered(bool checked)
{
	slot_DeleteFromRecordList(checked);
}

void CheckSum::slot_ActionRefreshSelectedFilesTriggered(bool checked)
{
	slot_RefreshSelectedRecordList(checked);
}

void CheckSum::UpdateBigIcon(QIcon icon)
{
	if(icon.isNull())
	{
		ui.widget_BigIcon->setPalette(QPalette(Qt::transparent));
		ui.label_NoIcon->setFixedSize(100,100);
		ui.label_NoIcon->setVisible(true);
	}
	else
	{
		int width = ui.widget_icon->width();//Defautl size is (128,128)
		QPixmap pixmap = icon.pixmap(width,width);	
		width=pixmap.width();
		int height = pixmap.height();
		width = width < height ? height : width;
		ui.widget_BigIcon->setFixedSize(width,width);
		QPalette palette;
		palette.setBrush(QPalette::Window,QBrush(pixmap));
		ui.widget_BigIcon->setPalette(palette);
		ui.label_NoIcon->setFixedSize(100,100);
		ui.label_NoIcon->setVisible(false);
	}
}

#ifdef  Q_OS_MAC

void CheckSum::LeawoQUrlToCFURLRef(QUrl &resUrl)
{
    QByteArray filePath = resUrl.toEncoded();
    if ( filePath.startsWith("file:///.file/id=") )
    {
        CFStringRef pathStr =CFStringCreateWithCString(kCFAllocatorDefault, filePath.data() ,kCFStringEncodingUTF8);
        //for CFURLCreateFilePathURL create temp URL, tempUrl = "file:///.file/id="
        CFURLRef tempUrl =CFURLCreateWithString(kCFAllocatorDefault, pathStr,NULL);
        //convert tempUrl to system file path
        CFErrorRef error =0;
        CFURLRef fileUrl =CFURLCreateFilePathURL(kCFAllocatorDefault, tempUrl, &error);
        //fileUrl = "eg:file:///Volumes/DISK2_10_9_5/Users/"
        //get char * for Qt from CFURLRef
        CFStringRef tempStr =CFURLGetString(fileUrl);
        CFIndex length =CFStringGetLength(tempStr);
        CFIndex maxSize =CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
        char *str = (char *)malloc(maxSize);
        //        memset(str, 0, sizeof(char)*(length + 1));
        CFStringGetCString(tempStr, str, maxSize,kCFStringEncodingUTF8);
        resUrl = QUrl(str);
    }
}

#endif

bool CheckSum::OpenFolderAndSelectFile(const char *filePath)
{
#ifdef Q_OS_WIN
    
	LPITEMIDLIST pidl;
    LPCITEMIDLIST cpidl;  
    LPSHELLFOLDER pDesktopFolder;  
    ULONG chEaten;  
    HRESULT hr;  
    WCHAR wfilePath[MAX_PATH+1] = { 0 };  
  
    ::CoInitialize( NULL );  
  
    if ( SUCCEEDED( SHGetDesktopFolder( &pDesktopFolder ) ) )  
    {  
        // IShellFolder::ParseDisplayName要传入宽字节  
        LPWSTR lpWStr = NULL;  
//#ifdef _UNICODE  
//        _tcscpy( wfilePath, strFilePath );  
//        lpWStr = wfilePath;  
//#else  
        MultiByteToWideChar( CP_ACP, 0, (LPCSTR)filePath, -1, wfilePath, MAX_PATH );   
        lpWStr = wfilePath;  
//#endif  
  
        hr = pDesktopFolder->ParseDisplayName( NULL, 0, lpWStr, &chEaten, &pidl, NULL );  
        if ( FAILED( hr ) )  
        {  
            pDesktopFolder->Release();  
            ::CoUninitialize();  
            return FALSE;  
        }  
  
        cpidl = pidl;  
  
        // SHOpenFolderAndSelectItems是非公开的API函数，需要从shell32.dll获取  
        // 该函数只有XP及以上的系统才支持，Win2000和98是不支持的，考虑到Win2000  
        // 和98已经基本不用了，所以就不考虑了，如果后面要支持上述老的系统，则要  
                // 添加额外的处理代码  
        HMODULE hShell32DLL = ::LoadLibraryA("shell32.dll");  
        //ASSERT( hShell32DLL != NULL );  
        if( hShell32DLL != NULL )  
        {  
            typedef HRESULT (WINAPI *pSelFun)( LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST  *apidl, DWORD dwFlags );  
            pSelFun pFun = (pSelFun)::GetProcAddress( hShell32DLL, "SHOpenFolderAndSelectItems" );  
            //ASSERT( pFun != NULL );     
            if( pFun != NULL )  
            {  
                hr = pFun( cpidl, 0, NULL, 0 ); // 第二个参数cidl置为0，表示是选中文件  
                if ( FAILED( hr ) )  
                {  
                    ::FreeLibrary( hShell32DLL );  
                    pDesktopFolder->Release();  
                    ::CoUninitialize();  
                    return FALSE;  
                }  
            }  
  
            ::FreeLibrary( hShell32DLL );  
        }  
        else  
        {  
            pDesktopFolder->Release();  
            ::CoUninitialize();  
            return FALSE;  
        }  
          
        // 释放pDesktopFolder  
        pDesktopFolder->Release();  
    }  
    else  
    {  
        ::CoUninitialize();  
        return FALSE;  
    }  
  
    ::CoUninitialize();  
    return TRUE;

#else
    QString pathIn(filePath);
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn.replace('\\', '/'));
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
    return true;
#endif
}

//bool CheckSum::nativeEvent(const QByteArray & eventType, void * message, long * result)
//{
//	MSG *msg = (MSG *)message;
//	if(msg->message == WM_NCHITTEST)
//	{
//		int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
//        int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
//		QPoint global(GET_X_LPARAM(msg->lParam),GET_Y_LPARAM(msg->lParam));
//		//this->mapFromGlobal(global);
//		//QWidget * pp = this->childAt(xPos,yPos);
//		QWidget * pp = this->childAt(this->mapFromGlobal(global));
//  //      if(this->childAt(xPos,yPos) == 0)
//  //      {
//			qDebug()<<QString("xPos is %1,yPos is %2,pp is %3").arg(xPos).arg(yPos).arg((int)pp);
//  //          *result = HTCAPTION;
//		//	return true;
//  //      }else{
//  //          return false;
//  //      }
//
//				
//		long res = ::DefWindowProc(msg->hwnd,msg->message,msg->wParam,msg->lParam);
//		if(res == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) < 0)// 如果鼠标左键按下，GetAsyncKeyState函数的返回值小于0  
//		{
//			*result = HTCAPTION;
//			return true;
//		}
//	}
//	else
//	{
//		return QMainWindow::nativeEvent(eventType,message,result);
//	}
//}
