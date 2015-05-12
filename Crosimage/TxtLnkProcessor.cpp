//TxtLnkProcessor.cpp by Kostya Kozachuck as neurocod - 2015.01.26 20:00:49
#include "pch.h"
#include "TxtLnkProcessor.h"

const QString TxtLnkProcessor::extensionWithDot = ".txtlnk";

void TxtLnkProcessor::shellExecute(const QString & path) {
	QString path2 = pathFromFile(path);
	if(path2.isEmpty())
		return;
	QDesktopServices::openUrl(QUrl::fromLocalFile(path2));
}
QString TxtLnkProcessor::pathFromFile(const QString & path) {
	QString pathRelative;
	if(!FileReader::readUnicode(path, pathRelative))
		return path;
	QFileInfo fileInfo(path);
	QDir dir = fileInfo.dir();
	if(!pathRelative.isEmpty()) {
		QString ret = dir.absoluteFilePath(pathRelative);
		ret = QDir::cleanPath(ret);
		return ret;
	}
	return existingLinkedFileFromParentDir(path);
}
QString TxtLnkProcessor::pathFromFileOrSame(const QString & path) {
	auto ret = pathFromFile(path);
	if(ret.isEmpty())
		return path;
	if(QFileInfo(ret).exists())
		return ret;
	return path;
}
QString TxtLnkProcessor::existingLinkedFileFromParentDir(const QString & str_) {
	if(str_.isEmpty())
		return str_;
	QFileInfo info(str_);
	auto str = info.fileName();
	int index = str.lastIndexOf('.');
	if(-1==index)
		return QString();
	str = str.left(index);
	auto dir = info.dir();
	if(dir.exists(str))
		return dir.absoluteFilePath(str);
	dir.cdUp();
	if(dir.exists(str))
		return dir.absoluteFilePath(str);
	return QString();
}