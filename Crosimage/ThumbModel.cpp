//ThumbModel.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ThumbModel.h"
#include "ThumbCache.h"

ThumbModel::ThumbModel(QObject*parent):
	QAbstractTableModel(parent),
	_dirThread(FileSystemThread::instance())
{
	_nColCount = 1;
	_invalidateFiles = false;
	connect(&ThumbCache::instance(), SIGNAL(loadedByCache(const QString &, const QImage&)),
		SLOT(updateThumb(const QString&, const QImage&)) );
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
	auto item = itemBy(index);
	if(!item)
		return QVariant();
	if(Qt::DisplayRole==role) {
		return item->absoluteFilePath();
	}
	if(Qt::ToolTipRole==role) {
		return item->fileName();
	}
	if(Qt::DecorationRole==role) {
		QIcon icon = _iconProvider.icon(item->fileInfo());
		auto sizes = icon.availableSizes();
		if(!sizes.isEmpty()) {
			return icon.pixmap(sizes[0]);
		}
	}
	if(Qt::SizeHintRole==role) {
		QSize sz(s_nThumbW, rowHeight(index.row()));
		return sz;
	}
	return QVariant();
}
int ThumbModel::rowHeight(int row)const {
	TopResult<int, int> ret;
	for(int col = 0; col<_nColCount; ++col) {
		if(auto item = itemBy(index(row, col))) {
			if(!item->thumbnail().isNull())
				ret.addMaxKey(item->thumbnail().height(), 0);
		}
	}
	if(ret.isSet())
		return ret.key();
	return s_nThumbH;
}
QVariant ThumbModel::headerData(int section, Qt::Orientation orientation, int role)const {
	if(Qt::Vertical==orientation) {
		if(Qt::DisplayRole==role) {
			QString str = toString(section*_nColCount);
			return str;
		}
		if(Qt::DisplayRole==role) {
			return section;
		}
		if(Qt::SizeHintRole==role) {
			static int w = 40;
			QSize sz(w, rowHeight(section));
			return sz;
		}
	}
	return __super::headerData(section, orientation, role);
}
ThumbModel::Item* ThumbModel::itemBy(int index)const {
	if(index<0 || index>=_items.count()) {
		return 0;
	}
	return _items[index];
}
ThumbModel::Item* ThumbModel::itemBy(const QModelIndex & index)const {
	if(!index.isValid())
		return 0;
	int n = index.row()*_nColCount + index.column();
	return itemBy(n);
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
}
void ThumbModel::refresh() {
	setDir(_dir.absolutePath(), false);
}
void ThumbModel::updateThumb(const QString & path, const QImage & image) {
	if(_items.isEmpty())
		return;
	for(int i=0; i<_items.count(); ++i) {
		auto item = _items[i];
		if(item->absoluteFilePath()==path) {
			item->setThumbnail(image);
			auto in = indexByIntIndex(i);
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
			if(item->fileInfo().isFile())
				_files << item->absoluteFilePath();
		}
	}
	return _files;
}
QModelIndex ThumbModel::indexByIntIndex(int i)const {
	int row	= i / _nColCount;
	int col = i % _nColCount;
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
	auto item = itemBy(index);
	if(!item)
		return 0;
	return __super::flags(index);
}