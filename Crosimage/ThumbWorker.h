//ThumbWorker.h by Kostya Kozachuck as neurocod
#pragma once

class ThumbWorker: public QThread {
	Q_OBJECT
	public:
		static ThumbWorker & instance();
		virtual ~ThumbWorker() {}
		void exit();
		void takeFile(const QString & path, bool updateAnyway = false);
		void makeFirst(const QString & path);
	signals:
		void updated(QString path, QImage thumb);
	protected:
		ThumbWorker();
		virtual void run()override;
		volatile bool _bNeedExit;
		volatile bool _bStarted;
		struct Job {
			QString _path;
			bool _updateAnyway = true;
			Job(const QString & path = QString(), bool updateAnyway = false): _path(path), _updateAnyway(updateAnyway) {}
			bool operator==(const Job & other)const {
				return other._path == _path && other._updateAnyway == _updateAnyway;
			}
		};
		QQueue<Job> _queue;//FIFO
		QMutex _lock;
		int _nThumbnailsCreated;
		int _nFilesRead;

		friend class ThumbVideoWorker;
		QImage thumb(const QString & path);
		QImage thumbFromVideo(const QString & path);
		static bool isVideoFile(const QString & path);
		QImage processNextFile(const QString & path, bool updateAnyway, bool innerCall=false);
		void maybeUpdate(bool innerCall, const QString & path, const QImage & image);
		void maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images);
		void writeToDb(bool innerCall, const QFileInfo & info, const QImage & image);
};