//ThumbWorker.h by Kostya Kozachuck as neurocod - 2013.10.19 20:55:38
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
		volatile bool m_bNeedExit;
		volatile bool m_bStarted;
		QQueue<QString> m_queue;//FIFO
		QMutex m_lock;

		QImage thumb(const QString & path);
		QImage processNextFile(const QString & path, bool innerCall=false);
		int m_nThumbnailsCreated;
		int m_nFilesRead;
		void maybeUpdate(bool innerCall, const QString & path, const QImage & image);
		void maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images);
		void writeToDb(bool innerCall, const QFileInfo & info, const QImage & image);
};