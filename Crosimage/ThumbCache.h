//ThumbCache.h by Kostya Kozachuck as neurocod - 2013.10.19 03:08:17
#pragma once

//lives in main thread
class ThumbCache: public QObject {
	Q_OBJECT
	public:
		virtual ~ThumbCache() {}
		static ThumbCache& instance();
		QImage get(const QFileInfo & info);
	public slots:
		void update(QString path, QImage thumb);
	signals:
		void updated(QString path, QImage thumb);
	protected:
		QImage get(const QString & path);
		ThumbCache();
		QMap<QString, QImage> m_map;
		QFileIconProvider m_iconProvider;
		QSet<QString> m_pathQueued;
		QSet<QString> m_supportedExtensions;
};