//MicrosoftLnkReader.cpp by Kostya Kozachuck as neurocod - 2015.01.26 16:59:22
#include "pch.h"
#include "MicrosoftLnkReader.h"
#include "Qqt/FileReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

quint8 MicrosoftLnkReader::CorrectGuid[16] = {0x01, 0x14, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46};
QString MicrosoftLnkReader::targetOrSame(const QString & path) {
	MicrosoftLnkReader reader;
	if(!reader.loadAndParse(path).ok())
		return path;
	return path;
}
StringStatus MicrosoftLnkReader::loadAndParse(const QString & path) {
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
		return ReadStatus(false, file.errorString());

	QDataStream stream(&file);
	stream >> _signature;
	if(_signature!=CorrectSignature)
		return ReadStatus(false, QString("Unknown link signature: %1, correct is %2").arg(_signature).arg(CorrectSignature));

	for(int i = 0; i<16; i++) {
		quint8 ch;
		stream >> ch;
		_guid[i] = ch;
		if(_guid[i]!=CorrectGuid[i])
			return ReadStatus(false, "Incorrect guid");
	};

	stream >> _shortcutFlags;
	stream >> _targetFileFlags;
	stream >> _creationTime;
	stream >> _lastAccessTime;
	stream >> _lastAccessTime;

	stream >> _fileLength;
	stream >> _iconNumber;
	stream >> _showWindow;
	stream >> _hotKey;

	QString sharedName= read0Terminated(stream, true);
	_fileName = read0Terminated(stream, true);
	return true;
}
QString MicrosoftLnkReader::read0Terminated(QDataStream & stream, bool unicode) {
	QString ret;
	if(unicode) {
		while(1) {
			quint16 ch;
			stream >> ch;
			if(!ch || stream.atEnd())
				break;
			ret += ch;
		}
		return ret;
	}
	while(1) {
		quint8 ch;
		stream >> ch;
		if(!ch || stream.atEnd())
			break;
		ret += ch;
	}
	return ret;
}