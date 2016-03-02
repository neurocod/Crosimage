//ThumbCache.h by Kostya Kozachuck as neurocod
#pragma once

//lives in main thread
class ThumbCache: public QObject {
	Q_OBJECT
	public:
		virtual ~ThumbCache() {}
		static ThumbCache& instance();
		QImage get(const QFileInfo & info, const QString & absoluteFilePath);
		void set(const QFileInfo & path, const QImage & img);
		void maybeMakeFirst(const QString & path);
		void rebuild(const QFileInfo & path);
	public slots:
		void loadedByDb(QString path, QImage thumb);
	signals:
		void loadedByCache(const QString & path, const QImage & thumb);
	protected:
		ThumbCache();
		QMap<QString, QImage> _map;
		QFileIconProvider _iconProvider;
		QSet<QString> _pathQueued;
		QSet<QString> _supportedExtensions;
};