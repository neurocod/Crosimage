//DirCache.h by Kostya Kozachuck as neurocod
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