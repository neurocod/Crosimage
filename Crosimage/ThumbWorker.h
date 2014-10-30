//ThumbWorker.h by Kostya Kozachuck as neurocod
#pragma once

class ThumbWorker: public QThread {
	Q_OBJECT
	public:
		static ThumbWorker & instance();
		virtual ~ThumbWorker() {}
		void exit();
		void takeFile(const QString & path);
		void makeFirst(const QString & path);
	signals:
		void update(QString path, QImage thumb);
	protected:
		ThumbWorker();
		virtual void run()override;
		volatile bool _bNeedExit;
		volatile bool _bStarted;
		struct Job {
			QString _path;
			bool _checkModificationTime = true;
			Job(const QString & path = QString(), bool checkModificationTime = true): _path(path), _checkModificationTime(checkModificationTime) {}
			bool operator==(const Job & other)const {
				return other._path == _path && other._checkModificationTime == _checkModificationTime;
			}
		};
		QQueue<Job> _queue;//FIFO
		QMutex _lock;
		int _nThumbnailsCreated;
		int _nFilesRead;

		QImage thumb(const QString & path);
		QImage processNextFile(const QString & path, bool innerCall=false);
		void maybeUpdate(bool innerCall, const QString & path, const QImage & image);
		void maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images);
		void writeToDb(bool innerCall, const QFileInfo & info, const QImage & image);
};