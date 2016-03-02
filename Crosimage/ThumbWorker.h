//ThumbWorker.h by Kostya Kozachuck as neurocod
#pragma once

class ThumbWorker: public QThread {
	Q_OBJECT
	public:
		static ThumbWorker & instance();
		virtual ~ThumbWorker() {}
		void exit();
		void takeFile(const QString & path, bool updateAnyway = false);
		void setThumb(const QFileInfo & file, const QImage & img);
		void makeFirst(const QString & path);
		void generateRecursive(const QDir & dir);
	signals:
		void updated(QString path, QImage thumb);
	protected:
		ThumbWorker();
		virtual void run()override;
		volatile bool _bNeedExit;
		volatile bool _bStarted;
		struct Job {
			bool _delete = true;
			virtual void run(ThumbWorker*w) = 0;
			virtual ~Job() { }
			enum Type {
				TypeGenerateThumb,
				TypeGenerateDirRecursive,
				TypeSetThumb
			};
			virtual Type type()const = 0;
			virtual bool equal(const Job & other)const { return type()==other.type(); }
		};
		struct GenerateThumb: public Job {
			QString _path;
			bool _updateAnyway = true;

			virtual ~GenerateThumb() {}
			virtual Type type()const override { return TypeGenerateThumb; }
			GenerateThumb(const QString & path = QString(), bool updateAnyway = false): _path(path), _updateAnyway(updateAnyway) { }
			virtual void run(ThumbWorker*w)override;
			virtual bool equal(const Job & other)const override {
				if(other.type()!=type())
					return false;
				auto & o = static_cast<const GenerateThumb &>(other);
				return _path==o._path && _updateAnyway==o._updateAnyway;
			}
		};
		struct SetThumb: public Job {
			QFileInfo _file;
			QImage _img;

			SetThumb(const QFileInfo & file, const QImage & img): _file(file), _img(img) {}
			virtual ~SetThumb() { }
			virtual void run(ThumbWorker*w)override;
			virtual Type type()const override { return TypeSetThumb; }
		};
		struct GenerateDirRecursive: public Job {
			GenerateDirRecursive(const QDir & dir, QDirIterator::IteratorFlags flags): _dirIterator(dir, flags) { }
			virtual ~GenerateDirRecursive() { }
			QDirIterator _dirIterator;
			virtual void run(ThumbWorker*w)override;
			virtual Type type()const override { return TypeGenerateDirRecursive; }
		};
		struct NewRating: public Job {
			virtual ~NewRating() { }
			QFileInfo _info;
			int _rating;
			virtual void run(ThumbWorker*w)override;
		};
		QQueue<Job*> _queue;//FIFO
		QMutex _lock;
		int _nThumbnailsCreated;
		int _nFilesRead;

		void pushBack(Job*j);
		friend class ThumbVideoWorker;
		QImage thumb(const QString & path);
		QImage thumbFromVideo(const QString & path);
		static bool isVideoFile(const QString & path);
		QImage generateThumb(const QString & path, bool updateAnyway, bool innerCall=false);
		void maybeUpdate(bool innerCall, const QString & path, const QImage & image);
		void maybeUpdate(bool innerCall, const QString & path, const QList<QImage> & images);
};