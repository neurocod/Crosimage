//ImgDbWorker.cpp by Kostya Kozachuck as neurocod - 2013.10.29 16:07:06
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
	m_qThumbGet(this, "SELECT thumb, modified FROM files WHERE name=:name;"),
	m_qThumbGetAll(this, "SELECT name, thumb, modified, rating, show_settings FROM files;"),
	m_qThumbSet(this, "INSERT OR REPLACE INTO files (name, thumb, modified) VALUES (:name, :thumb, :modified);")
{
	m_dbPath = dir.absoluteFilePath(dbFileName);
	QElapsedTimer timer;
    timer.start();
	bool ret = readAll();
	//qDebug() << "ImgDbWorker readAll took" << timer.elapsed() << "milliseconds";
}
bool ImgDbWorker::thumbnail(const QFileInfo & file, OUT QImage & img) {
	return instance(file)->_thumbnail(file, img);
}
bool ImgDbWorker::readAll() {
	if(!initSqlOnce())
		return false;
	//auto & q = m_qThumbGetAll;
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
	//	m_items << item;
	//}
	return true;
}
bool ImgDbWorker::_thumbnail(const QFileInfo & file, OUT QImage & img) {
	if(!initSqlOnce())
		return false;
	auto & q = m_qThumbGet;
	q.bindValue(":name", file.fileName());
	if(!execOrTrace(q))
		return false;
	if(!q.next()) {
		return false;
	}
	int col = 0;
	{
		auto arrImg = q.value(col++).value<QByteArray>();
		if(arrImg.isEmpty())
			return true;
		QBuffer stream(&arrImg);
		QImageReader reader(&stream);
		if(!reader.read(&img)) {
			QString str = reader.errorString();
			return false;
		}
	}
	QDateTime modified = dateTimeFromVariant(q.value(col++));
	if(file.isDir()) {
		return true;//dirs change too often, no check
	}
	bool ret = modified==file.lastModified();
	return ret;
}
bool ImgDbWorker::setThumbnail(const QFileInfo & file, const QImage & img) {
	return instance(file)->_setThumbnail(file, img);
}
bool ImgDbWorker::_setThumbnail(const QFileInfo & file, const QImage & img) {
	if(!initSqlOnce())
		return false;
	m_qThumbSet.bindValue(":name", file.fileName());
	{
		QBuffer buff;
		if(!img.isNull()) { //otherwise - warning
			QImageWriter writer(&buff, "png");
			writer.write(img);
		}
		m_qThumbSet.bindValue(":thumb", buff.buffer());
	}
	auto modified = file.lastModified();
	m_qThumbSet.bindValue(":modified", toVariantByteArray(modified));
	return execOrTrace(m_qThumbSet);
}
bool ImgDbWorker::connectToDbOnce() {
	if(m_db.isOpen())
		return true;
	m_db = QSqlDatabase::addDatabase("QSQLITE", m_dbPath);
	m_db.setDatabaseName(m_dbPath);
	if(!m_db.open())
		return false;
	if(!maybeInstallDb())
		return false;
	return true;
}
bool ImgDbWorker::maybeInstallDb() {
	QStringList names = m_db.tables();
	QSqlQuery q(m_db);
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