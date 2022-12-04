//FileWriter.cpp by Kostya Kozachuck as neurocod - 22.10.2011 18:07:19
#include "pch.h"
#include "FileWriter.h"

WriteStatus FileWriter::write(const QString & fileName, const QByteArray& arr) {
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))
		return formatError(fileName, file.errorString());
	file.write(arr);
	file.close();
	return true;
}
WriteStatus FileWriter::write8bit(const QString & fileName, const QString & strFileData) {
	return write(fileName, strFileData.toLocal8Bit());
}
WriteStatus FileWriter::writeUnicode(const QString & fileName, const QString & contents, const QStringConverter::Encoding encoding) {
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))
		return formatError(fileName, file.errorString());
	QTextStream stream(&file);
	stream.setEncoding(encoding);
	stream.setGenerateByteOrderMark(true);
	stream << contents;
	file.close();
	return true;
}
WriteStatus FileWriter::removeFile(const QString & path) {
	QFileInfo info(path);
	if(info.exists() && info.isFile()) {
		return info.dir().remove(info.fileName());
	}
	return WriteStatus(QObject::tr("Can't remove file %1").arg(path));
}
WriteStatus FileWriter::formatError(const QString & file, const QString & error) {
	return WriteStatus(QObject::tr("Error writing file '%1': %2").arg(file).arg(error));
}
WriteStatus FileWriter::formatError(const QFile & file) {
	if(file.error()==QFile::NoError)
		return true;
	return formatError(file.fileName(), file.errorString());
}