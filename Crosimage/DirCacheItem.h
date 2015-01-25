//DirCacheItem.h by Kostya Kozachuck as neurocod
#pragma once

class DirCacheItem {
	public:
		DirCacheItem(const QDir & dir, const QString & file);
		virtual ~DirCacheItem() {}
		QString absoluteFilePath()const;
		QString fileName()const;
		const QFileInfo & fileInfo()const;
		const QImage & thumbnail()const;
		void setThumbnail(const QImage &i);

		qint64 _rating;
		QByteArray _showSettings;
	protected:
		QImage _thumbnail;
		QFileInfo _fileInfo;
		QString _absoluteFilePath;
		QString _fileName;
};