//FileFacility.h by Kostya Kozachuck as neurocod - 03.07.2010 13:34:46
#pragma once
#include "DirString.h"
#include "FileReader.h"
#include "FileWriter.h"

//QObject for tr
class FileFacility: public QObject {
	public:
		static bool createDirForFile(const QString & strFile);
		static void showDirWithFile(const QString & dir, const QString & file);
		static void showDirWithFile(const QString & path);
		static QString formatDiskBytes(const quint64 bytes);
		static void openInTextEditor(const QString & strPath, bool warnIfNotExists=false);
		static bool serializationCheck(bool bSave, QDataStream & stream, QString strCheck);

		static QString imageExtensionsForFileDialog(const QList<QByteArray> & li);
		template<class T>//T can be QImageReader/QImageWriter
		static QString imageExtensionsForFileDialog() {
			auto li = T::supportedImageFormats();
			return imageExtensionsForFileDialog(li);
		}
	protected:
		FileFacility() {}
};