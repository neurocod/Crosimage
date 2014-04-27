//ThumbModel.h by Kostya Kozachuck as neurocod - 29.03.2013 7:44:38
#pragma once
#include "FileSystemThread.h"
#include "ImgDbWorker.h"
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

		//void setColumnCountByView(int count);
		typedef DirCacheItem Item;
		QModelIndex indexByIntIndex(int i)const;
		Item* itemAt(const QModelIndex & index)const;
		Item* itemAt(int index)const;
		const QDir & dir()const { return m_dir; }
		const QList<Item*> & items()const { return m_items; }
		QStringList files()const;
		void setSortFlags(QDir::SortFlags f);
		void setDir(const QDir & dir);
	public slots:
		void setDir(const QString & str, bool check=true);
		void refresh();
		void updateThumb(QString, QImage);
		void directoryLoaded(QDir path, QStringList items);
	protected:
		int m_nColCount;

		QDir m_dir;
		FileSystemThread m_dirThread;
		QList<Item*> m_items;
		mutable bool m_invalidateFiles;
		mutable QStringList m_files;
		QFileIconProvider m_iconProvider;
		void clear();
};