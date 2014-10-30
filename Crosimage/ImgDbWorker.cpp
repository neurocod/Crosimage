//ImgDbWorker.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ImgDbWorker.h"

const QString ImgDbWorker::dbFileName = "thumbnails.sqlite";

ImgDbWorker* ImgDbWorker::instance(const QDir & dir) {
	QString str = dir.absolutePath();
#ifdef _DEBUG //всегда вызывается из одного и того же потока, не нужна синхронизация и защита
	static QThread* thread = 0;
	if(thread) {
		ASSERT(thread==QThread::currentThread());
	} else {
		thread = QThread::currentThread();
	}
#endif
	static QMap<QString, ImgDbWorker*> map;
	auto ret = map.value(str);
	if(!ret) {
		ret = new ImgDbWorker(dir);
		map.insert(str, ret);
	}
	return ret;
}
ImgDbWorker* ImgDbWorker::instance(const QFileInfo & file) {
	return instance(file.dir());
}
ImgDbWorker::ImgDbWorker(const QDir & dir):
	_qThumbGet(this, "SELECT thumb, modified FROM files WHERE name=:name;"),
	_qThumbGetAll(this, "SELECT name, thumb, modified, rating, show_settings FROM files;"),
	_qThumbSet(this, "INSERT OR REPLACE INTO files (name, thumb, modified) VALUES (:name, :thumb, :modified);")
{
	_dbPath = dir.absoluteFilePath(dbFileName);
	QElapsedTimer timer;
    timer.start();
	auto ret = readAll();
	//qDebug() << "ImgDbWorker readAll took" << timer.elapsed() << "milliseconds";
}
ReadStatus ImgDbWorker::thumbnail(const QFileInfo & file, OUT QImage & img) {
	return instance(file)->thumbnail_(file, img);
}
ReadStatus ImgDbWorker::readAll() {
	if(!initSqlOnce().ok())
		return false;
	//auto & q = _qThumbGetAll;
	//if(!execOrTrace(q))
	//	return false;
	//while(q.next()) {
	//	Item item;
	//	int col = 0;
	//	item.name = q.value(col++).value<QString>();
	//	{
	//		auto arrImg = q.value(col++).value<QByteArray>();
	//		if(!arrImg.isEmpty()) {
	//			QBuffer stream(&arrImg);
	//			QImageReader reader(&stream);
	//			if(!reader.read(&item.thumbnail)) {
	//				QString str = reader.errorString();
	//			}
	//		}
	//	}
	//	item.lastModified = dateTimeFromVariant(q.value(col++));
	//	item.rating = q.value(col++).toLongLong();
	//	item.showSettings = q.value(col++).toByteArray();
	//	_items << item;
	//}
	return true;
}
ReadStatus ImgDbWorker::thumbnail_(const QFileInfo & file, OUT QImage & img) {
	auto ret = initSqlOnce();
	if(!ret.ok())
		return ret;
	auto & q = _qThumbGet;
	q.bindValue(":name", file.fileName());
	if(!execOrTrace(q))
		return false;
	if(!q.next())
		return false;
	int col = 0;
	{
		auto arrImg = q.value(col++).value<QByteArray>();
		if(arrImg.isEmpty())
			return true;
		QBuffer stream(&arrImg);
		QImageReader reader(&stream);
		if(!reader.read(&img))
			return ReadStatus(false, reader.errorString());
	}
	QDateTime modified = dateTimeFromVariant(q.value(col++));
	if(modified.isNull())
		return false;
	if(file.isDir())
		return true;//dirs change too often, no check
	bool ok = modified==file.lastModified();
	return ok;
}
ReadStatus ImgDbWorker::setThumbnail(const QFileInfo & file, const QImage & img) {
	return instance(file)->setThumbnail_(file, img);
}
WriteStatus ImgDbWorker::setThumbnail_(const QFileInfo & file, const QImage & img) {
	auto ret = initSqlOnce();
	if(!ret.ok())
		return ret;
	_qThumbSet.bindValue(":name", file.fileName());
	{
		QBuffer buff;
		if(!img.isNull()) { //otherwise - warning
			QImageWriter writer(&buff, "png");
			writer.write(img);
		}
		_qThumbSet.bindValue(":thumb", buff.buffer());
	}
	auto modified = file.lastModified();
	_qThumbSet.bindValue(":modified", toVariantByteArray(modified));
	return execOrTrace(_qThumbSet);
}
ReadStatus ImgDbWorker::connectToDbOnce() {
	if(_db.isOpen())
		return true;
	_db = QSqlDatabase::addDatabase("QSQLITE", _dbPath);
	_db.setDatabaseName(_dbPath);
	if(!_db.open())
		return ReadStatus(false, _db.lastError().text());
	return maybeInstallDb();
}
bool ImgDbWorker::maybeInstallDb() {
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