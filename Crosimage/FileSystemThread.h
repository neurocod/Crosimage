//FileSystemThread.h by Kostya Kozachuck as neurocod
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