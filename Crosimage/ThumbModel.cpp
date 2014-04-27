//ThumbModel.cpp by Kostya Kozachuck as neurocod - 29.03.2013 7:44:43
#include "pch.h"
#include "ThumbModel.h"
#include "ThumbCache.h"

ThumbModel::ThumbModel(QObject*parent):
	QAbstractTableModel(parent)
{
	m_nColCount = 1;
	m_invalidateFiles = false;
	connect(&ThumbCache::instance(), SIGNAL(updated(QString, QImage)), SLOT(updateThumb(QString, QImage)) );
	connect(&m_dirThread, SIGNAL(items(QDir, QStringList)), SLOT(directoryLoaded(QDir, QStringList)) );
}
ThumbModel::~ThumbModel() {
	clear();
}
int ThumbModel::rowCount(const QModelIndex & parent)const {
	int rows = m_items.count()/m_nColCount;
	if(rows*m_nColCount<m_items.size())
		rows++;
	return rows;
}
int ThumbModel::columnCount(const QModelIndex & parent)const {
	return m_nColCount;
}
QVariant ThumbModel::data(const QModelIndex & index, int role)const {
	auto item = itemAt(index);
	if(!item)
		return QVariant();
	if(Qt::DisplayRole==role) {
		return item->absoluteFilePath();
	}
	if(Qt::ToolTipRole==role) {
		return item->fileName();
	}
	if(Qt::DecorationRole==role) {
		QIcon icon = m_iconProvider.icon(*item);
		auto sizes = icon.availableSizes();
		if(!sizes.isEmpty()) {
			return icon.pixmap(sizes[0]);
		}
	}
	return QVariant();
}
QVariant ThumbModel::headerData(int section, Qt::Orientation orientation, int role)const {
	if(Qt::DisplayRole==role && orientation==Qt::Vertical) {
		QString str = toString(section*m_nColCount);
		return str;
	}
	return __super::headerData(section, orientation, role);
}
ThumbModel::Item* ThumbModel::itemAt(int index)const {
	if(index<0 || index>=m_items.count()) {
		return 0;
	}
	return m_items[index];
}
ThumbModel::Item* ThumbModel::itemAt(const QModelIndex & index)const {
	if(!index.isValid())
		return 0;
	int n = index.row()*m_nColCount + index.column();
	return itemAt(n);
}
void ThumbModel::setDir(const QString & str, bool check) {
	QDir dir(str);
	if(check && dir.absolutePath()==m_dir.absolutePath())
		return;
	dir.setSorting(QDir::Time);
	setDir(dir);
}
void ThumbModel::setDir(const QDir & dir) {
	beginResetModel();
	clear();
	m_dir = dir;
	m_dir.setSorting(m_dir.sorting()|QDir::DirsFirst);
	endResetModel();
	QMetaObject::invokeMethod(&m_dirThread, "readDir", Qt::QueuedConnection, Q_ARG(QDir, m_dir));
}
void ThumbModel::clear() {
	qDeleteAll(m_items);
	m_items.clear();
	m_invalidateFiles = true;
}
void ThumbModel::directoryLoaded(QDir dir, QStringList items) {
	if(m_dir!=dir)
		return;
	beginResetModel();
	clear();
	for(auto item: items)
		m_items << new Item(m_dir, item);
	endResetModel();
	for (int i = m_items.count() - 1; i >= 0 ; i--) {
		ThumbCache::instance().get(*m_items[i]);
	}
}
void ThumbModel::refresh() {
	setDir(m_dir.absolutePath(), false);
}
void ThumbModel::updateThumb(QString path, QImage image) {
	if(m_items.isEmpty())
		return;
	//auto i0 = index(0,0);
	//auto i1 = index(rowCount()-1, columnCount()-1);
	//emit dataChanged(i0, i0);
	for(int i=0; i<m_items.count(); ++i) {
		auto item = m_items[i];
		if(item->absoluteFilePath()==path) {
			auto in = index(i/m_nColCount, i % m_nColCount);
			emit dataChanged(in, in);
			break;
		}
	}
}
QStringList ThumbModel::files()const {
	if(m_invalidateFiles) {
		m_invalidateFiles = false;
		m_files.clear();
		for(auto item: m_items) {
			if(item->isFile())
				m_files << item->absoluteFilePath();
		}
	}
	return m_files;	
}
QModelIndex ThumbModel::indexByIntIndex(int i)const {
	int row	= i/columnCount();
	int col = i%columnCount();
	return createIndex(row, col);
}
void ThumbModel::setSortFlags(QDir::SortFlags f) {
	QDir dir = m_dir;
	dir.setSorting(f|QDir::DirsFirst);
	setDir(dir);
}
void ThumbModel::setColumnCount(int cols) {
	if(cols<1 || cols==m_nColCount)
		return;
	beginResetModel();
	m_nColCount = cols;
	endResetModel();
}