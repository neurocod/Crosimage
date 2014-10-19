//ThumbCache.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbCache.h"
#include "ThumbModel.h"
#include "ThumbWorker.h"

ThumbCache::ThumbCache() {
	connect(&ThumbWorker::instance(), SIGNAL(update(QString, QImage)), SLOT(update(QString, QImage)), Qt::QueuedConnection);
	//for(auto mime: QImageReader::supportedMimeTypes()) {
	//	for(auto ext: )
	//}
}
ThumbCache & ThumbCache::instance() {
	static ThumbCache p;
	return p;
}
QImage ThumbCache::get(const QFileInfo & info) {
	return get(info.absoluteFilePath());
}
QImage ThumbCache::get(const QString & path) {
	if(!_map.contains(path)) {
		auto thumb = _iconProvider.icon(QFileInfo(path)).pixmap(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH).toImage();
		_map[path] = thumb;
		ThumbWorker::instance().takeFile(path);
		_pathQueued.insert(path);
		return thumb;
	}
	if(_pathQueued.contains(path)) {//reorder queue
		ThumbWorker::instance().makeFirst(path);
	}
	return _map.value(path);
}
void ThumbCache::update(QString path, QImage thumb) {
	_pathQueued.remove(path);
	if(!thumb.isNull()) {
		_map[path] = thumb;
		emit updated(path, thumb);
	}
}