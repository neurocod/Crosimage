//FileFacility.cpp by Kostya Kozachuck as neurocod - 03.07.2010 13:34:47
#include "pch.h"
#include "FileFacility.h"
#include "DirString.h"

bool FileFacility::createDirForFile(const QString & strFile) {
	int pos = strFile.lastIndexOf('/');
	if(-1==pos) {
		//current directory
		return true;
	}
	QString strPath = strFile.left(pos);
	return QDir().mkpath(strPath);
}
bool FileFacility::serializationCheck(bool bSave, QDataStream & stream, QString strCheck) {
	if(stream.status()!=QDataStream::Ok)
		return false;
	const QChar nMagicCheck = 0xf00d;//food
	if(bSave) {
		stream << nMagicCheck;
		stream << strCheck;
		return true;
	} else {
		bool ret = false;
		QChar nCheck;
		stream >> nCheck;
		if(stream.status()!=QDataStream::Ok)
			return false;
		if(nMagicCheck==nCheck) {
			QString strTest;
			stream >> strTest;
			if(stream.status()!=QDataStream::Ok)
				return false;
			ret = strCheck==strTest;
			if(!ret) {
				QString strError = QObject::tr("Loading check failed: not found string '%1'\n")
					.arg(strCheck);
				qCritical() << strError;
			}
		} else {
			QString strError = QObject::tr("Loading check failed: not found number before string '%1'\n").arg(strCheck);
			qCritical() << strError;
		}

		return ret;
	}
}
void FileFacility::showDirWithFile(const QString & dir, const QString & file) {
	#ifdef Q_OS_WIN32
		QString path = DirString(dir);
		path += file;
		//problems when path contains ','
		QProcess::startDetached("explorer.exe", QStringList() << "/select," << QDir::toNativeSeparators(path));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
	#endif
}
void FileFacility::showDirWithFile(const QString & _path) {
	QString path = _path;
	path.replace('\\', '/');
	QString dir, file;
	int index = path.lastIndexOf('/');
	if(-1==index) {
		dir = path;
	} else {
		index++;
		dir = path.left(index);
		file = path.mid(index);
	}
	showDirWithFile(dir, file);
}
void FileFacility::openInTextEditor(const QString & strPath, bool warnIfNotExists) {
	if(warnIfNotExists) {
		if(!QFile::exists(strPath)) {
			qCritical() << tr("Warning: file %1 does not exist").arg(strPath);
			return;
		}
	}
	QStringList args;
	args << strPath;
#ifdef Q_OS_WIN
	//TODO: choose Program files, if no Notepad++ - use notepad
	QString editorPathName = "C:/Program Files (x86)/Notepad++/notepad++.exe";
	/*if (lineNumber != -1) {
		args << "-n" + toString(lineNumber);
	}*/
	QProcess::startDetached("\"" + editorPathName + "\"", args);
#else
#ifdef QT_WIDGETS_LIB
	QDesktopServices::openUrl(QUrl::fromLocalFile(strPath));
#else
	QProcess::startDetached("nano", args);
#endif
#endif
}
QString FileFacility::imageExtensionsForFileDialog(const QList<QByteArray> & li) {
	struct Names: public QHash<QString, QString> {
		Names() {
			insert(QStringLiteral("bmp"), "Windows Bitmap");
			insert(QStringLiteral("gif"), "Graphic Interchange Format (optional)");
			insert(QStringLiteral("jpg"), "Joint Photographic Experts Group");
			insert(QStringLiteral("jpeg"), "Joint Photographic Experts Group");
			insert(QStringLiteral("png"), "Portable Network Graphics");
			insert(QStringLiteral("pbm"), "Portable Bitmap");
			insert(QStringLiteral("pgm"), "Portable Graymap");
			insert(QStringLiteral("ppm"), "Portable Pixmap");
			insert(QStringLiteral("tiff"), "Tagged Image File Format");
			insert(QStringLiteral("xbm"), "X11 Bitmap");
			insert(QStringLiteral("xpm"), "X11 Pixmap");
			insert(QStringLiteral("ico"), "Windows icon");
			insert(QStringLiteral("pdf"), "Adobe pdf");
		}
	};
	static const Names names;
	QString ret;

	//Images (*.png *.xpm *.jpg);;
	ret += tr("Images (");
	for(QByteArray arr: li) {
		ret += "*.";
		ret += arr;
		ret += " ";
	}
	ret.remove(ret.length()-1, 1);
	ret += ");;";
	for(QByteArray arr: li) {
		QString ext = arr;
		QString desc = names[ext];
		if(desc.isEmpty())
			desc = tr("%1 files").arg(ext.toUpper());
		ret += desc + QString(" (*.%1);;").arg(ext);
	}
	ret += tr("All files (*);;");
	return ret;
}
QString FileFacility::formatDiskBytes(const quint64 bytes) {
	static const QStringList names = {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};
	const quint64 divider = 1024;
	QString ret;
	quint64 rest = bytes;
	for(int nPart = 0; nPart<names.size() && rest>0; nPart++) {
		quint64 part = rest % divider;
		rest /= divider;
		ret.prepend(QString(" %1 %2").arg(part).arg(names[nPart]));
	}
	ret = ret.trimmed();
	if(bytes>divider) {
		ret.prepend(" (");
		ret += ")";
	}
	ret.prepend(QString("%1 bytes").arg(bytes));
	return ret;
}