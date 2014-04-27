//FileSystemThread.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "FileSystemThread.h"
#include "ImgDbWorker.h"

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
	ASSERT(QThread::currentThread()!=qApp->thread());
	QStringList entryList = dir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden);
	//qDebug() << t.elapsed();
	entryList.removeOne(ImgDbWorker::dbFileName);
	entryList.removeOne("Thumbs.db");
	if((dir.sorting() & QDir::SortByMask)==QDir::Name) {
		ASSERT(dir.sorting() & QDir::DirsFirst);
		QStringList dirs;
		QStringList other;
		bool goDirs = true;//optimization to no to check isDir()
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
		qSort(other.begin(), other.end(), compareByNameAndNumber);
		entryList = dirs + other;
	}
	emit items(dir, entryList);
}