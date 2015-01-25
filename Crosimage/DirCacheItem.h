//DirCacheItem.h by Kostya Kozachuck as neurocod
#pragma once

class DirCacheItem {
	public:
		DirCacheItem(const QDir & dir, const QString & file);
		virtual ~DirCacheItem() {}
		QString absoluteFilePath()const;
		QString fileName()const;
		const QFileInfo & fileInfo()const;

		QImage _thumbnail;
		qint64 _rating;
		QByteArray _showSettings;
	protected:
		QFileInfo _fileInfo;
		QString _absoluteFilePath;
		QString _fileName;
};