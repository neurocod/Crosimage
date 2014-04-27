//DirCache.h by Kostya Kozachuck as neurocod - 2013.11.04 17:36:06
#pragma once
#include "DirCacheItem.h"

class DirCache: public QObject {
	public:
		DirCache();
		virtual ~DirCache() {}

	protected:
		QString m_dir;
		QVector<DirCacheItem> m_items;
		void clear();
};