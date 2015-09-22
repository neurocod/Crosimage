//ThumbModel.h by Kostya Kozachuck as neurocod
#pragma once
#include "FileSystemThread.h"
#include "DirDb.h"
#include "DirCache.h"
#include "DirCacheItem.h"

class ThumbModel: public QAbstractTableModel {
	Q_OBJECT
	public:
		static const int s_nThumbW = 325;
		static const int s_nThumbH = 325;
		ThumbModel(QObject*parent=0);
		virtual ~ThumbModel();

		virtual int rowCount(const QModelIndex & parent=QModelIndex())const override;
		virtual int columnCount(const QModelIndex & parent=QModelIndex())const override;
		virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole)const override;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const override;
		void setColumnCount(int cols);
		int rowHeight(int row)const;

		//void setColumnCountByView(int count);
		typedef DirCacheItem Item;
		QModelIndex indexByIntIndex(int i)const;
		Item* itemBy(const QModelIndex & index)const;
		Item* itemBy(int index)const;
		const QDir & dir()const { return _dir; }
		const QList<Item*> & items()const { return _items; }
		QStringList files()const;
		void setSortFlags(QDir::SortFlags f);
		void setDir(const QDir & dir);
		virtual Qt::ItemFlags flags(const QModelIndex & index)const override;
	public slots:
		void setDir(const QString & str, bool check=true);
		void refresh();
		void updateThumb(const QString & path, const QImage & thumb);
		void directoryLoaded(QDir path, QStringList items);
	protected:
		int _nColCount;

		QDir _dir;
		FileSystemThread & _dirThread;
		QList<Item*> _items;
		mutable bool _invalidateFiles;
		mutable QStringList _files;
		QFileIconProvider _iconProvider;
		void clear();
};