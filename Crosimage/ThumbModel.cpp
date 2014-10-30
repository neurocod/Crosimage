//ThumbModel.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbModel.h"
#include "ThumbCache.h"

ThumbModel::ThumbModel(QObject*parent):
	QAbstractTableModel(parent)
{
	_nColCount = 1;
	_invalidateFiles = false;
	connect(&ThumbCache::instance(), SIGNAL(loadedByCache(QString, QImage)), SLOT(updateThumb(QString, QImage)) );
	connect(&_dirThread, SIGNAL(items(QDir, QStringList)), SLOT(directoryLoaded(QDir, QStringList)) );
}
ThumbModel::~ThumbModel() {
	clear();
}
int ThumbModel::rowCount(const QModelIndex & parent)const {
	int rows = _items.count()/_nColCount;
	if(rows*_nColCount<_items.size())
		rows++;
	return rows;
}
int ThumbModel::columnCount(const QModelIndex & parent)const {
	return _nColCount;
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
		QIcon icon = _iconProvider.icon(*item);
		auto sizes = icon.availableSizes();
		if(!sizes.isEmpty()) {
			return icon.pixmap(sizes[0]);
		}
	}
	return QVariant();
}
QVariant ThumbModel::headerData(int section, Qt::Orientation orientation, int role)const {
	if(Qt::DisplayRole==role && orientation==Qt::Vertical) {
		QString str = toString(section*_nColCount);
		return str;
	}
	return __super::headerData(section, orientation, role);
}
ThumbModel::Item* ThumbModel::itemAt(int index)const {
	if(index<0 || index>=_items.count()) {
		return 0;
	}
	return _items[index];
}
ThumbModel::Item* ThumbModel::itemAt(const QModelIndex & index)const {
	if(!index.isValid())
		return 0;
	int n = index.row()*_nColCount + index.column();
	return itemAt(n);
}
void ThumbModel::setDir(const QString & str, bool check) {
	QDir dir(str);
	if(check && dir.absolutePath()==_dir.absolutePath())
		return;
	dir.setSorting(QDir::Time);
	setDir(dir);
}
void ThumbModel::setDir(const QDir & dir) {
	beginResetModel();
	clear();
	_dir = dir;
	_dir.setSorting(_dir.sorting()|QDir::DirsFirst);
	endResetModel();
	QMetaObject::invokeMethod(&_dirThread, "readDir", Qt::QueuedConnection, Q_ARG(QDir, _dir));
}
void ThumbModel::clear() {
	qDeleteAll(_items);
	_items.clear();
	_invalidateFiles = true;
}
void ThumbModel::directoryLoaded(QDir dir, QStringList items) {
	if(_dir!=dir)
		return;
	beginResetModel();
	clear();
	for(auto item: items)
		_items << new Item(_dir, item);
	endResetModel();
	for (int i = _items.count() - 1; i >= 0 ; i--) {
		ThumbCache::instance().get(*_items[i]);
	}
}
void ThumbModel::refresh() {
	setDir(_dir.absolutePath(), false);
}
void ThumbModel::updateThumb(QString path, QImage image) {
	if(_items.isEmpty())
		return;
	//auto i0 = index(0,0);
	//auto i1 = index(rowCount()-1, columnCount()-1);
	//emit dataChanged(i0, i0);
	for(int i=0; i<_items.count(); ++i) {
		auto item = _items[i];
		if(item->absoluteFilePath()==path) {
			auto in = index(i/_nColCount, i % _nColCount);
			emit dataChanged(in, in);
			break;
		}
	}
}
QStringList ThumbModel::files()const {
	if(_invalidateFiles) {
		_invalidateFiles = false;
		_files.clear();
		for(auto item: _items) {
			if(item->isFile())
				_files << item->absoluteFilePath();
		}
	}
	return _files;	
}
QModelIndex ThumbModel::indexByIntIndex(int i)const {
	int row	= i/columnCount();
	int col = i%columnCount();
	return createIndex(row, col);
}
void ThumbModel::setSortFlags(QDir::SortFlags f) {
	QDir dir = _dir;
	dir.setSorting(f|QDir::DirsFirst);
	setDir(dir);
}
void ThumbModel::setColumnCount(int cols) {
	if(cols<1 || cols==_nColCount)
		return;
	beginResetModel();
	_nColCount = cols;
	endResetModel();
}
Qt::ItemFlags ThumbModel::flags(const QModelIndex & index)const {
	auto item = itemAt(index);
	if(!item)
		return 0;
	return __super::flags(index);
}