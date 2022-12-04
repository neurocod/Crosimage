//FileSystemThread.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "FileSystemThread.h"
#include "DirDb.h"

FileSystemThread& FileSystemThread::instance() {
	static FileSystemThread p;
	return p;
}
FileSystemThread::FileSystemThread() {
	moveToThread(this);
	start();
	while(!isRunning())
		msleep(1);
}
FileSystemThread::~FileSystemThread() {
	quit();
	while(!isFinished())
		msleep(1);
}
bool compareByNameAndNumber(const QString & s1, const QString & s2);
void FileSystemThread::readDir(QDir dir) {
	QElapsedTimer t;
	t.start();
	QString dirPath = dir.path();//for debug
	ASSERT(QThread::currentThread()!=qApp->thread());//don't call directly from gui thread cause gui thread will slow down
	QStringList entryList = dir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot
		|QDir::Hidden|QDir::System//without this it hides broken shortcuts
		);
	//qDebug() << t.elapsed();
	entryList.removeOne(DirDb::dbFileName);
	entryList.removeOne(DirDb::dbFileName2);
	entryList.removeOne("Thumbs.db");
	if((dir.sorting() & QDir::SortByMask)==QDir::Name) {
		ASSERT(dir.sorting() & QDir::DirsFirst);
		QStringList dirs;
		QStringList other;
		bool goDirs = true;//optimization to not to check isDir()
		for(auto s: entryList) {
			if(goDirs) {
				if(QFileInfo(dir.absoluteFilePath(s)).isDir())
					dirs << s;
				else
					goDirs = false;
			}
			if(!goDirs)
				other << s;
		}
		std::sort(other.begin(), other.end(), compareByNameAndNumber);
		entryList = dirs + other;
	}
	emit items(dir, entryList);
}