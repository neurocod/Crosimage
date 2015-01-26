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
	QFileInfo file(path);
	QString ret = file.dir().absoluteFilePath(pathRelative);
	ret = QDir::cleanPath(ret);
	return ret;
}
QString TxtLnkProcessor::pathFromFileOrSame(const QString & path) {
	auto ret = pathFromFile(path);
	if(ret.isEmpty())
		return path;
	if(QFileInfo(ret).exists())
		return ret;
	return path;
}