//ThumbCache.cpp by Kostya Kozachuck as neurocod - 2013.10.19 03:08:17
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
	if(!m_map.contains(path)) {
		auto thumb = m_iconProvider.icon(QFileInfo(path)).pixmap(ThumbModel::s_nThumbW, ThumbModel::s_nThumbH).toImage();
		m_map[path] = thumb;
		ThumbWorker::instance().takeFile(path);
		m_pathQueued.insert(path);
		return thumb;
	}
	if(m_pathQueued.contains(path)) {//reorder queue
		ThumbWorker::instance().makeFirst(path);
	}
	return m_map.value(path);
}
void ThumbCache::update(QString path, QImage thumb) {
	m_pathQueued.remove(path);
	if(!thumb.isNull()) {
		m_map[path] = thumb;
		emit updated(path, thumb);
	}
}