//DirCache.h by Kostya Kozachuck as neurocod
#pragma once
#include "DirCacheItem.h"

class DirCache: public QObject {
	public:
		DirCache();
		virtual ~DirCache() {}

	protected:
		QString _dir;
		QVector<DirCacheItem> _items;
		void clear();
};