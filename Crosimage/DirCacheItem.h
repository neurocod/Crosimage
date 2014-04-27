//DirCacheItem.h by Kostya Kozachuck as neurocod
#pragma once

class DirCacheItem: public QFileInfo {
	public:
		DirCacheItem(const QDir & dir, const QString & file);
		virtual ~DirCacheItem() {}

		QImage thumbnail;
		qint64 rating;
		QByteArray showSettings;
};