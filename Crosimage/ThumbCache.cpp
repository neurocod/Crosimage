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
	auto it = _map.constFind(path);
	if(it!=_map.constEnd()) {
		maybeMakeFirst(path);
		return it.value();
	}
	int x = CrSettings::inst()._thumbW;
	int y = CrSettings::inst()._thumbH;
	if(info.isFile()) {
		x = qMin(64, x);
		y = qMin(64, y);
	}
	auto thumb = _iconProvider.icon(info).pixmap(x, y).toImage();
	auto sz = thumb.size();
	_map[path] = thumb;
	ThumbWorker::instance().takeFile(path);
	_pathQueued.insert(path);
	return thumb;
}
void ThumbCache::maybeMakeFirst(const QString & path) {
	if(_pathQueued.contains(path)) {//reorder queue
		ThumbWorker::instance().makeFirst(path);
	}
}
void ThumbCache::rebuild(const QFileInfo & file) {
	auto path = file.absoluteFilePath();
	_map.remove(path);
	ThumbWorker::instance().takeFile(path, true);
}
void ThumbCache::set(const QFileInfo & file, const QImage & img) {
	if(img.isNull()) {
		ASSERT(0);//warning: image will rebuild
		return;
	}
	auto path = file.absoluteFilePath();
	_map.remove(path);
	ThumbWorker::instance().setThumb(file, img);
	emit loadedByCache(path, img);
}
void ThumbCache::loadedByDb(QString path, QImage thumb) {
	_pathQueued.remove(path);
	if(!thumb.isNull()) {
		_map[path] = thumb;
		emit loadedByCache(path, thumb);
	}
}