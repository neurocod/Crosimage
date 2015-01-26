//MicrosoftLnkReader.cpp by Kostya Kozachuck as neurocod - 2015.01.26 16:59:22
#include "pch.h"
#include "MicrosoftLnkReader.h"
#include "Qqt/FileReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

template<class T>
QString MicrosoftLnkReader::printf(const QString & str, T t) {
	QString ret = str.arg(t);
	return ret;
}
QString MicrosoftLnkReader::targetOrSame(const QString & path) {
	MicrosoftLnkReader reader;
	if(!reader.loadAndParse(path))
		return path;
	return path;
}
StringStatus MicrosoftLnkReader::loadAndParse(const QString & path) {
	char LinkFileContent[10240];
	QByteArray fileContent;
	if(!FileReader::read(path, fileContent))
		return false;
	fileContent.append((char)0);

	int LinkFileSize = fileContent.size();
	for(int i = 0; i<fileContent.size() && i<sizeof(LinkFileContent); ++i) {
		LinkFileContent[i] = fileContent.at(i);
	}
	_signature = LinkFileContent[0]*65536*256;
	_signature += LinkFileContent[1]*65536;
	_signature += LinkFileContent[2]*256+LinkFileContent[3];
	printf("Signature: %1\n", Signature);

	for(int i = 0; i<16; i++) {
		GUID[i] = LinkFileContent[4+i];
	};

	ShortcutFlags = LinkFileContent[23]*65536*256;
	ShortcutFlags += LinkFileContent[22]*65536;
	ShortcutFlags += LinkFileContent[21]*256+LinkFileContent[20];
	printf("Shortcut Flags: %1\n", ShortcutFlags);

	TargetFileFlags = LinkFileContent[27]*65536*256;
	TargetFileFlags += LinkFileContent[26]*65536;
	TargetFileFlags += LinkFileContent[25]*256+LinkFileContent[24];
	printf("Target File Flags: %1\n", TargetFileFlags);

	for(int i = 0; i<8; i++) { CreationTime[i] = LinkFileContent[28+i]; };
	for(int i = 0; i<8; i++) { LastAccessTime[i] = LinkFileContent[36+i]; };
	for(int i = 0; i<8; i++) { LastAccessTime[i] = LinkFileContent[44+i]; };

	FileLength = LinkFileContent[55]*65536*256;
	FileLength += LinkFileContent[54]*65536;
	FileLength += LinkFileContent[53]*256+LinkFileContent[52];
	printf("Target File Size (bytes): %1\n", FileLength);

	IconNumber = LinkFileContent[59]*65536*256;
	IconNumber += LinkFileContent[58]*65536;
	IconNumber += LinkFileContent[57]*256+LinkFileContent[56];
	printf("IconNumber: %1\n", IconNumber);

	ShowWindow = LinkFileContent[63]*65536*256;
	ShowWindow += LinkFileContent[62]*65536;
	ShowWindow += LinkFileContent[61]*256+LinkFileContent[60];
	printf("ShowWindow Attributes: %1\n", IconNumber);

	HotKey = LinkFileContent[67]*65536*256;
	HotKey += LinkFileContent[66]*65536;
	HotKey += LinkFileContent[65]*256+LinkFileContent[64];
	printf("HotKey Attributes: %1\n", HotKey);

	// Now extract file name and path

	unsigned int OFFSET = 76+(unsigned char)LinkFileContent[76];
	OFFSET += (unsigned char)LinkFileContent[77]*256+(unsigned char)LinkFileContent[78]+2;

	OFFSET += LinkFileContent[OFFSET];
	printf("Share name: %1\n", &LinkFileContent[OFFSET]);

	OFFSET += strlen(&LinkFileContent[OFFSET]);
	OFFSET++;

	printf("File name: %1\n", &LinkFileContent[OFFSET]);
	return path;
}