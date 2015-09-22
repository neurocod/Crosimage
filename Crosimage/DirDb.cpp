//DirDb.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "DirDb.h"

const QString DirDb::dbFileName = "thumbnails.sqlite";
const QString DirDb::dbFileName2 = dbFileName + "-journal";

DirDb* DirDb::instance(const QDir & dir) {
	QString str = dir.absolutePath();
#ifdef _DEBUG //всегда вызывается из одного и того же потока, не нужна синхронизация и защита
	static QThread* thread = 0;
	if(thread) {
		ASSERT(thread==QThread::currentThread());
	} else {
		thread = QThread::currentThread();
	}
#endif
	static QMap<QString, DirDb*> map;
	auto ret = map.value(str);
	if(!ret) {
		ret = new DirDb(dir);
		map.insert(str, ret);
	}
	return ret;
}
DirDb* DirDb::instance(const QFileInfo & file) {
	return instance(file.dir());
}
DirDb::DirDb(const QDir & dir):
	_qThumbGet(this, "SELECT thumb, modified FROM files WHERE name=:name;"),
	_qThumbGetAll(this, "SELECT name, thumb, modified, rating, show_settings FROM files;"),
	_qThumbSet(this, "INSERT OR REPLACE INTO files (name, thumb, modified) VALUES (:name, :thumb, :modified);")
{
	_dbPath = dir.absoluteFilePath(dbFileName);
	QElapsedTimer timer;
    timer.start();
	auto ret = readAllToCache();
	//qDebug() << "DirDb readAll took" << timer.elapsed() << "milliseconds";
}
DirDb::~DirDb() {
	qDeleteAll(_items);
}
ReadStatus DirDb::thumbnail(const QFileInfo & file, OUT QImage & img) {
	return instance(file)->thumbnail_(file, img);
}
ReadStatus DirDb::readAllToCache() {
	if(!initSqlOnce().ok())
		return false;
	auto & q = _qThumbGetAll;
	if(!execOrTrace(q))
		return false;
	while(q.next()) {
		int col = 0;
		QString name = q.value(col++).value<QString>();
		auto item = getOrCreate(name);
		item->_thumb = q.value(col++).value<QByteArray>();
		item->_modified = dateTimeFromVariant(q.value(col++));
		item->_rating = q.value(col++).value<int>();
		item->_showSettings = q.value(col++).value<QByteArray>();
	}
	qStableSort(_items.begin(), _items.end(), &DirDb::compareByRating);
	return true;
}
bool DirDb::compareByRating(const Item * i1, const Item * i2) {
	return i1->_rating > i1->_rating;
}
ReadStatus DirDb::thumbnail_(const QFileInfo & file, OUT QImage & img) {
	QString name = file.fileName();
	auto it = _byName.find(name);
	if(it==_byName.end())
		return false;
	Item* item = *it;
	if(item->_iThumb.isNull()) {
		if(item->_thumb.isNull())
			return false;
		QBuffer stream(&item->_thumb);
		QImageReader reader(&stream);
		if(!reader.read(&item->_iThumb)) {
			QString str = reader.errorString();
			return false;
		}
	}
	img = item->_iThumb;
	return true;
	//auto ret = initSqlOnce();
	//if(!ret.ok())
	//	return ret;
	//auto & q = _qThumbGet;
	//q.bindValue(":name", name);
	//if(!execOrTrace(q))
	//	return false;
	//if(!q.next())
	//	return false;
	//int col = 0;
	//{
	//	auto arrImg = q.value(col++).value<QByteArray>();
	//	if(arrImg.isEmpty())
	//		return true;
	//	QBuffer stream(&arrImg);
	//	QImageReader reader(&stream);
	//	if(!reader.read(&img))
	//		return ReadStatus(false, reader.errorString());
	//}
	//QDateTime modified = dateTimeFromVariant(q.value(col++));
	//if(modified.isNull())
	//	return false;
	//if(file.isDir())
	//	return true;//dirs change too often, no check
	//bool ok = modified==file.lastModified();
	//return ok;
}
DirDb::Item* DirDb::getOrCreate(const QString & name) {
	auto it = _byName.find(name);
	if(it!=_byName.end())
		return *it;
	auto item = new Item(name);
	_byName.insert(name, item);
	_items << item;
	return item;
}
ReadStatus DirDb::setThumbnail(const QFileInfo & file, const QImage & img) {
	return instance(file)->setThumbnail_(file, img);
}
WriteStatus DirDb::setThumbnail_(const QFileInfo & file, const QImage & img) {
	QString name = file.fileName();
	auto item = getOrCreate(name);
	item->_modified = file.lastModified();
	item->_iThumb = img;
	{
		QBuffer buff(&item->_thumb);
		if(img.isNull()) {//otherwise - warning in QImageWriter
			item->_thumb.clear();
		} else {
			QImageWriter writer(&buff, "png");
			writer.write(img);
		}
	}
	auto ret = initSqlOnce();
	if(!ret.ok())
		return ret;
	_qThumbSet.bindValue(":name", name);
	_qThumbSet.bindValue(":thumb", item->_thumb);
	_qThumbSet.bindValue(":modified", toVariantByteArray(item->_modified));
	return execOrTrace(_qThumbSet);
}
ReadStatus DirDb::connectToDbOnce() {
	if(_db.isOpen())
		return true;
	_db = QSqlDatabase::addDatabase("QSQLITE", _dbPath);
	_db.setDatabaseName(_dbPath);
	if(!_db.open())
		return ReadStatus(false, _db.lastError().text());
	return maybeInstallDb();
}
bool DirDb::maybeInstallDb() {
	QStringList names = _db.tables();
	QSqlQuery q(_db);
	if(!names.contains("settings")) {
		if(!execOrTrace(q, "CREATE TABLE [settings] ( "
			"k TEXT UNIQUE NOT NULL PRIMARY KEY, "
			"v BLOB NULL )"))
			return false;
	}
	if(!names.contains("files")) {
		if(!execOrTrace(q, "CREATE TABLE [files] ( "
			"name TEXT UNIQUE NOT NULL PRIMARY KEY, "
			"thumb BLOB NULL, "//null -> can't generate cause unknown format etc
			"modified BLOB NULL, "
			"rating INTEGER NULL, "//INTEGER DEFAULT '0' NOT NULL
			"show_settings BLOB NULL )"))
			return false;
	}
	return true;
}