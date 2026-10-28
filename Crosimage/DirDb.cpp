//DirDb.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "DirDb.h"

const QString DirDb::dbFileName = "thumbnails.sqlite";
const QString DirDb::dbFileName2 = dbFileName + "-journal";

namespace {
const QString settingsTableSql = "CREATE TABLE IF NOT EXISTS settings ("
	"k TEXT UNIQUE NOT NULL PRIMARY KEY, v BLOB NULL)";

// Short-lived connections belong to the filesystem thread; thumbnail connections
// and their caches remain exclusively owned by ThumbWorker.
class DirectorySettingsDb: public SqliteDb {
public:
	explicit DirectorySettingsDb(const QDir & dir): _path(dir.absoluteFilePath(DirDb::dbFileName)) {}
protected:
	ReadStatus connectToDbOnce()override {
		if(_db.isOpen())
			return true;
		_db = QSqlDatabase::addDatabase("QSQLITE", "settings:" + _path);
		_db.setDatabaseName(_path);
		if(!_db.open())
			return _db.lastError().text();
		PreparedSqlQuery q(*this, settingsTableSql);
		return q.execOrTrace();
	}
private:
	QString _path;
};
}

ReadStatus DirDb::loadSortFlags(const QDir & dir, QDir::SortFlags & flags) {
	if(!QFileInfo::exists(dir.absoluteFilePath(dbFileName)))
		return false;
	DirectorySettingsDb db(dir);
	auto status = db.initSqlOnce();
	if(!status.ok())
		return status;
	PreparedSqlQuery q(db, "SELECT v FROM settings WHERE k='sortFlags'");
	status = q.execOrTrace();
	if(!status.ok())
		return status;
	if(!q.next())
		return false;
	bool ok = false;
	const int value = q.value(0).toInt(&ok);
	const int sortBy = value & (QDir::SortByMask | QDir::Type);
	if(!ok || (value & ~(QDir::SortByMask | QDir::Type | QDir::Reversed | QDir::DirsFirst))
		|| (sortBy!=QDir::Name && sortBy!=QDir::Time && sortBy!=QDir::Size && sortBy!=QDir::Type))
		return false;
	flags = QDir::SortFlags(value) | QDir::DirsFirst;
	return true;
}

WriteStatus DirDb::saveSortFlags(const QDir & dir) {
	DirectorySettingsDb db(dir);
	auto status = db.initSqlOnce();
	if(!status.ok())
		return status;
	PreparedSqlQuery q(db, "INSERT OR REPLACE INTO settings (k, v) VALUES ('sortFlags', :value)");
	if(!q.prepareOnceOrTrace().ok())
		return false;
	q.bindValue(":value", int(dir.sorting()));
	return q.execOrTrace();
}

DirDb& DirDb::instance(const QDir & dir) {
	QString str = dir.absolutePath();
#ifdef _DEBUG //всегда вызывается из одного и того же потока, не нужна синхронизация и защита
	static QThread* thread = QThread::currentThread();
	ASSERT(thread==QThread::currentThread());
#endif
	static QMap<QString, DirDb*> map;
	auto ret = map.value(str);
	if(!ret) {
		ret = new DirDb(dir);
		map.insert(str, ret);
	}
	return *ret;
}
DirDb& DirDb::instance(const QFileInfo & file) {
	return instance(file.dir());
}
DirDb::DirDb(const QDir & dir):
	_qThumbGet(*this, "SELECT thumb, modified FROM files WHERE name=:name;"),
	_qThumbGetAll(*this, "SELECT name, thumb, modified, rating, show_settings FROM files;"),
	_qThumbSet(*this, "INSERT OR REPLACE INTO files (name, thumb, modified) VALUES (:name, :thumb, :modified);"),
	_qThumbSetRating(*this, "UPDATE files SET rating=:rating WHERE name=:name;")
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
ReadStatus DirDb::readAllToCache() {
	if(!initSqlOnce().ok())
		return false;
	auto & q = _qThumbGetAll;
	auto status = q.execOrTrace();
	if(!status.ok())
		return status;
	while(q.next()) {
		int col = 0;
		QString name = q.value(col++).value<QString>();
		auto item = getOrCreate(name);
		item->_thumb = q.value(col++).value<QByteArray>();
		item->_modified = dateTimeFromVariant(q.value(col++));
		item->_rating = q.value(col++).value<int>();
		item->_showSettings = q.value(col++).value<QByteArray>();
	}
	std::stable_sort(_items.begin(), _items.end(), &DirDb::compareByRating);
	return true;
}
bool DirDb::compareByRating(const Item * i1, const Item * i2) {
	return i1->_rating > i1->_rating;
}
void DirDb::freeCacheMemory() {
	qDeleteAll(_items);
	_items.clear();
	_byName.clear();
}
ReadStatus DirDb::thumbnail(const QFileInfo & file, OUT QImage & img) {
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
WriteStatus DirDb::setThumbnailS(const QFileInfo & file, const QImage & image) {//static
	return instance(file).setThumbnail(file, image);
}
WriteStatus DirDb::setThumbnail(const QFileInfo & file, const QImage & img) {
	auto ret = initSqlOnce();
	if(!ret.ok())
		return ret;
	QString name = file.fileName();
	auto item = getOrCreate(name);
	item->_modified = file.lastModified();
	item->_iThumb = img;
	{
		QBuffer buff(&item->_thumb);
		if(img.isNull()) {//otherwise - warning in QImageWriter
			item->_thumb.clear();
		} else {
			QImageWriter writer(&buff, "jpg");
			auto sz = img.size();
			writer.write(img);
		}
	}
	_qThumbSet.bindValue(QStringLiteral(":name"), name);
	_qThumbSet.bindValue(QStringLiteral(":thumb"), item->_thumb);
	_qThumbSet.bindValue(QStringLiteral(":modified"), toVariantByteArray(item->_modified));
	return _qThumbSet.execOrTrace();
}
ReadStatus DirDb::connectToDbOnce() {
	if(_db.isOpen())
		return true;
	_db = QSqlDatabase::addDatabase("QSQLITE", _dbPath);
	_db.setDatabaseName(_dbPath);
	if(!_db.open())
		return _db.lastError().text();
	return maybeInstallDb();
}
StringStatus DirDb::maybeInstallDb() {
	QStringList names = _db.tables();
	
	if(!names.contains("settings")) {
		PreparedSqlQuery q(*this, settingsTableSql);
		auto s = q.execOrTrace();
		if(!s.ok())
			return s;
	}
	if(!names.contains("files")) {
		PreparedSqlQuery q(*this, "CREATE TABLE [files] ( "
			"name TEXT UNIQUE NOT NULL PRIMARY KEY, "
			"thumb BLOB NULL, "//null -> can't generate cause unknown format etc
			"modified BLOB NULL, "
			"rating INTEGER NULL, "//INTEGER DEFAULT '0' NOT NULL
			"show_settings BLOB NULL )");
		auto s = q.execOrTrace();
		if(!s.ok())
			return s;
	}
	return true;
}
WriteStatus DirDb::setRating(const QFileInfo & file, int n) {
	auto ret = initSqlOnce();
	if(!ret.ok())
		return ret;
	auto & q = _qThumbSetRating;
	q.bindValue(QStringLiteral(":name"), file.fileName());
	q.bindValue(QStringLiteral(":rating"), n);
	getOrCreate(file.fileName())->_rating = n;
	return q.execOrTrace();
}
