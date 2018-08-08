#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QMainWindow>
#include <QFileInfo>

#include "ui_checksum.h"
#include "CalculateTaskManager.h"

class QDirModel;
class QModelIndex;
class QStandardItemModel;
class CalculateFileCheckThread;


class CheckSum : public QMainWindow
{
	Q_OBJECT

public:
	CheckSum(QString initFileList = QString(), QWidget *parent = 0);
	~CheckSum();

	enum TableColumn
	{
		FileName = 0,
		Checksum = 1,
		CRC32 = 2,
		MD5 = 3,
		FilePath = 4
	};

private:
	Ui::CheckSumClass ui;

	QDirModel *model;
	QStandardItemModel *tableRecord;
	QMap<QString,FileCheckSum> mapRecords;
	CalculateTaskManager calcManager;
	QMenu *tableRecordsRightMenu;//�������б��Ҽ��˵�
	QMenu *dirViewRightMenu;//�ļ��鿴���Ҽ��˵�
	QAction *actOpenFileLocation;//���ļ�λ��
	QAction *actOpenFile;//���ļ�
	QAction *actDeleteSelectedFiles;//ɾ��ѡ���ļ�
	QAction *actRefreshSelectedFiles;//ɾ��ѡ���ļ�
	QAction *actCopyFilePath;
	QAction *actAddChildFiles;//���Ŀ¼�µ��ļ�
	QAction *actAddAllChildFiles;//�ݹ��Ŀ¼�µ������ļ�
	QAction *actAddThisFile;//��Ӵ��ļ�
	QPoint relativePos; // store relative position 
	bool press;
	bool currentLower;

	int iSelectedIndex;
	bool waitExit;//�ȴ���̨������ɺ��˳�
	void SetCheckSum(const FileCheckSum &fileCheckSum,int rowIndex = -1,bool changeCurrnet = true);
	void SetCheckSumNull();

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void SetTaskPercent();
	void UpdateRefreshAllEnable();
	void UpdateBigIcon(QIcon icon = QIcon());
	void CreateMenuActions();
	void DestoryMenuActions();

	bool OpenFolderAndSelectFile(const char *filePath);
	void AddDirChildFiles(QString path,bool recursive);
    
#ifdef  Q_OS_MAC
    
    void LeawoQUrlToCFURLRef(QUrl &resUrl);
    
#endif

	//bool nativeEvent(const QByteArray & eventType, void * message, long * result);

	private slots:
		void slot_AutoWidthStateChanged(int state);
		void slot_LowerStateChanged(int state);

		void slot_RefreshExplorer(bool checked = false);
		void slot_AddToRecordList(bool checked = false);
		void slot_DeleteFromRecordList(bool checked = false);
		void slot_DeleteAllFromRecordList(bool checked = false);
		void slot_RefreshSelectedRecordList(bool checked = false);
		void slot_RefreshAllFromRecordList(bool checked = false);
		void slot_Exit(bool checked = false);
		void slot_DoubleClicked(const QModelIndex & index);
		void slot_HideShowLeft(bool checked = false);
		void slot_CurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);
		void slot_CalculateSuccess(const bool &success,const FileCheckSum &fileCheckSum);
		void slot_Completed();
		void slot_ShowToolTip(const QModelIndex &index);
		void slot_OpenFile(const QModelIndex &index);
		
		void slot_ActionAddThisFileTriggered(bool checked = false);
		void slot_ActionAddChildFilesTriggered(bool checked = false);
		void slot_ActionAddAllChildFilesTriggered(bool checked = false);

		void slot_ActionOpenFileLocationTriggered(bool checked = false);
		void slot_ActionOpenFileTriggered(bool checked = false);
		void slot_ActionDeleteSelectedFilesTriggered(bool checked = false);
		void slot_ActionRefreshSelectedFilesTriggered(bool checked = false);
		void slot_ActionCopyFilePathTriggered(bool checked = false);

	public slots:
		void receiveMessage(QString message);

protected:
	void closeEvent(QCloseEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif // CHECKSUM_H