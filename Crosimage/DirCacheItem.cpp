//DirCacheItem.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "DirCacheItem.h"
#include "ThumbCache.h"

DirCacheItem::DirCacheItem(const QDir & dir, const QString & file): QFileInfo(dir, file) {
	thumbnail = ThumbCache::instance().get(*this);
}
