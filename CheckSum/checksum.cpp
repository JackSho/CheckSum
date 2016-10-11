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

#ifdef Q_OS_WIN

#include <QtWin>
#include <qt_windows.h>
#include <ShlObj.h>
#include <Windowsx.h >

#endif

#include "checksum.h"
#include "CalculateFileCheckThread.h"




//#include "QtWindowsAero.h"

CheckSum::CheckSum(QString initFileList, QWidget *parent)
	: QMainWindow(parent),waitExit(false),iSelectedIndex(-1),press(false),rightMenu(NULL),actOpenFileLocation(NULL),actOpenFile(NULL)
{
    ui.setupUi(this);

#ifdef Q_OS_WIN

	if (QtWin::isCompositionEnabled())                           //����DWM���״̬
	{
		QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);    //����Ч��
		QtWin::enableBlurBehindWindow(this);
		setAttribute(Qt::WA_TranslucentBackground, true);         //��͸������
		setAttribute(Qt::WA_NoSystemBackground, false);           //�����ޱ���
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

	//����setModel()����������ģ����QTableView��
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
		mapRecords[filePath].bOK = false;
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
			mapRecords[filePath].bOK = false;
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
	QFileIconProvider fileIP;
	ui.lineEdit_fileName->setText(fileCheckSum.fileInfo.fileName());
	ui.lineEdit_MD5->setText("0x" + fileCheckSum.md5);
	ui.lineEdit_checkSum->setText("0x" + fileCheckSum.checkSum);
	ui.lineEdit_CRC32->setText("0x" + fileCheckSum.crc32);
	ui.lineEdit_SHA1->setText("0x" + fileCheckSum.sha1);
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
#ifdef Q_DEBUG
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
#ifdef Q_DEBUG
    qDebug() << "CheckSum::dropEvent enter."
#endif

	QList<QUrl> urls = event->mimeData()->urls();
	int exist = 0;
	for(int i=0;i<urls.count();i++)
	{
		QUrl url = urls.at(i);
        QString path =url.toLocalFile();
        
#ifdef Q_DEBUG
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
#ifdef Q_DEBUG
    
    qDebug() << "CheckSum::mousePressEvent enter."
    
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
		calcManager.DeleteCalculateTask();//�û����ڹرճ���ɾ������δ�������
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
	QString toopTip = "File: " + filePath + 
		"\nMd5: 0x" + fileCheckSum.md5 + 
		"\nCheckSum: 0x" + fileCheckSum.checkSum + 
		"\nSize: " + QString("%L1").arg(fileCheckSum.fileInfo.size()) + " Bytes";

	QToolTip::showText(QCursor::pos(), toopTip); 
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
	QPoint pt = ui.tableView_records->mapFromGlobal(QCursor::pos());
	pt -= QPoint(ui.tableView_records->verticalHeader()->width(),ui.tableView_records->horizontalHeader()->height());
	int rowIndex = ui.tableView_records->indexAt(pt).row();
	//qDebug()<<QString("x is %1, y is %2, row is %3.").arg(pt.x()).arg(pt.y()).arg(ind.row());
	if(rowIndex>-1)
	{
		rightMenu->exec(QCursor::pos());
		event->accept();
		//event->ignore();
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
	rightMenu = new QMenu(this);
    
#ifdef Q_OS_WIN
    
    actOpenFileLocation = new QAction("Open file location",this);
    connect(actOpenFileLocation, SIGNAL(triggered(bool)), this, SLOT(slot_ActionOpenFileLocationTriggered(bool)));
    rightMenu->addAction(actOpenFileLocation);
    
#endif
    
	actOpenFile = new QAction("Open file",this);

	connect(actOpenFile, SIGNAL(triggered(bool)), this, SLOT(slot_ActionOpenFileTriggered(bool)));
	rightMenu->addAction(actOpenFile);
}

void CheckSum::DestoryMenuActions()
{
	if(actOpenFileLocation)
	{
		delete (actOpenFileLocation);
	}
	if(actOpenFile)
	{
		delete (actOpenFile);
	}
	if(rightMenu)
	{
		delete (rightMenu);
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
		//ShellExecuteA(0,"open","explorer.exe",codec->fromUnicode(param).data(),NULL,SW_SHOWNORMAL);//���ܲ������ƣ�ÿ�ζ��򿪴��ڣ�
	}
}

void CheckSum::slot_ActionOpenFileTriggered(bool checked)
{
	QModelIndexList rowModelIndexList = ui.tableView_records->selectionModel()->selectedRows();
	while(rowModelIndexList.count())
	{
		int rowIndex = rowModelIndexList.takeLast().row();
		QString filePath = tableRecord->item(rowIndex,CheckSum::FilePath)->text();
		QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
	}
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

void LeawoQUrlToCFURLRef(QUrl &resUrl)
{
    QByteArray filePath = resUrl.toEncoded();
    if ( filePath.startsWith("file:///.file/id=") )
    {
 /*       CFStringRef pathStr =CFStringCreateWithCString(kCFAllocatorDefault
                                                       , filePath.data()
                                                       ,kCFStringEncodingUTF8);
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
 */   }
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
        // IShellFolder::ParseDisplayNameҪ������ֽ�  
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
  
        // SHOpenFolderAndSelectItems�Ƿǹ�����API��������Ҫ��shell32.dll��ȡ  
        // �ú���ֻ��XP�����ϵ�ϵͳ��֧�֣�Win2000��98�ǲ�֧�ֵģ����ǵ�Win2000  
        // ��98�Ѿ����������ˣ����ԾͲ������ˣ��������Ҫ֧�������ϵ�ϵͳ����Ҫ  
                // ���Ӷ���Ĵ�������  
        HMODULE hShell32DLL = ::LoadLibraryA("shell32.dll");  
        //ASSERT( hShell32DLL != NULL );  
        if( hShell32DLL != NULL )  
        {  
            typedef HRESULT (WINAPI *pSelFun)( LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST  *apidl, DWORD dwFlags );  
            pSelFun pFun = (pSelFun)::GetProcAddress( hShell32DLL, "SHOpenFolderAndSelectItems" );  
            //ASSERT( pFun != NULL );     
            if( pFun != NULL )  
            {  
                hr = pFun( cpidl, 0, NULL, 0 ); // �ڶ�������cidl��Ϊ0����ʾ��ѡ���ļ�  
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
          
        // �ͷ�pDesktopFolder  
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
//		if(res == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) < 0)// ������������£�GetAsyncKeyState�����ķ���ֵС��0  
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