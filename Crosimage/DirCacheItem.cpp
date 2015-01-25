//DirCacheItem.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "DirCacheItem.h"
#include "ThumbCache.h"

DirCacheItem::DirCacheItem(const QDir & dir, const QString & file): _fileInfo(dir, file) {
	_absoluteFilePath = _fileInfo.absoluteFilePath();
	_fileName = _fileInfo.fileName();
	_thumbnail = ThumbCache::instance().get(_fileInfo, _absoluteFilePath);
}
QString DirCacheItem::absoluteFilePath()const {
	return _absoluteFilePath;
}
QString DirCacheItem::fileName()const {
	return _fileName;
}
const QFileInfo & DirCacheItem::fileInfo()const {
	return _fileInfo;
}
const QImage & DirCacheItem::thumbnail()const {
	return _thumbnail;
}
void DirCacheItem::setThumbnail(const QImage &i) {
	_thumbnail = i;
}