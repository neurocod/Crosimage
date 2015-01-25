//ThumbCache.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbCache.h"
#include "ThumbModel.h"
#include "ThumbWorker.h"

ThumbCache::ThumbCache() {
	connect(&ThumbWorker::instance(), SIGNAL(updated(QString, QImage)), SLOT(loadedByDb(QString, QImage)), Qt::QueuedConnection);
	//for(auto mime: QImageReader::supportedMimeTypes()) {
	//	for(auto ext: )
	//}
}
ThumbCache & ThumbCache::instance() {
	static ThumbCache p;
	return p;
}
QImage ThumbCache::get(const QFileInfo & info, const QString & path) {
	if(!_map.contains(path)) {
		auto thumb = _iconProvider.icon(info).pixmap(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH).toImage();
		_map[path] = thumb;
		ThumbWorker::instance().takeFile(path);
		_pathQueued.insert(path);
		return thumb;
	}
	maybeMakeFirst(path);
	return _map.value(path);
}
void ThumbCache::maybeMakeFirst(const QString & path) {
	if(_pathQueued.contains(path)) {//reorder queue
		ThumbWorker::instance().makeFirst(path);
	}
}
void ThumbCache::rebuild(const QString & path) {
	_map.remove(path);
	ThumbWorker::instance().takeFile(path, true);
}
void ThumbCache::loadedByDb(QString path, QImage thumb) {
	_pathQueued.remove(path);
	if(!thumb.isNull()) {
		_map[path] = thumb;
		emit loadedByCache(path, thumb);
	}
}