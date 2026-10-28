//FileSystemThread.h by Kostya Kozachuck as neurocod
#pragma once

class FileSystemThread: public QThread {
	Q_OBJECT
	public:
		static FileSystemThread& instance();
		virtual ~FileSystemThread();
	public slots:
		void readDir(QDir dir, bool restoreSorting, quint64 requestId);
	signals:
		void items(QDir dir, QStringList items, quint64 requestId);
	protected:
		FileSystemThread();
};
