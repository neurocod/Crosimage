//DirCacheItem.cpp by Kostya Kozachuck as neurocod - 2013.11.09 15:57:36
#include "pch.h"
#include "DirCacheItem.h"

DirCacheItem::DirCacheItem(const QDir & dir, const QString & file): QFileInfo(dir, file) {
}
