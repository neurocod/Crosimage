//FileSystemThread.h by Kostya Kozachuck as neurocod - 2013.10.29 10:51:48
#pragma once

class FileSystemThread: public MyThread {
	Q_OBJECT
	public:
		FileSystemThread();
		virtual ~FileSystemThread();
	public slots:
		void readDir(QDir dir);
	signals:
		void items(QDir dir, QStringList items);
	protected:
		
};