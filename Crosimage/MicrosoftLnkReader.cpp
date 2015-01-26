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
struct TLinkHeaderInfo {
	static const int Signature = 0x4C000000;
	quint32 _signature;
	unsigned char GUID[16];//The current GUID for shortcuts is 01 14 02 00 00 00 00 00 C0 00 00 00 00 00 46, It may change in the future.
	quint32 ShortcutFlags;//Shortcut flags are explained below
	quint32 TargetFileFlags;//Target file flags - Flags are explained below
	unsigned char CreationTime[8];//Windows FILETIME structure
	unsigned char LastAccessTime[8];//FILETIME
	unsigned char ModificationTime[8];//FILETIME
	quint32 FileLength;//The length of the target file. 0 if the target is not a file.
		//This value is used to find the target when the link is broken.
	quint32 IconNumber;//If the file has a custom icon (set by the flags bit 6),
		//then this long integer indicates the index of the icon to use. Otherwise it is zero.
	quint32 ShowWindow;//SW_SHOWNORMAL=1, SW_SHOWMAXIMIZED=3, SW_SHOWMINNOACTIVE=7, all other values MUST be treated as SW_SHOWNORMAL
	quint32 HotKey;//The hot key assigned for this shortcut
	quint32 Reserved1;//Reserved - Always 0
	quint32 Reserved2;//Reserved - Always 0
};
QString MicrosoftLnkReader::targetOrSame(const QString & path) {
	TLinkHeaderInfo HeaderStructure;

	char LinkFileContent[10240];
	QByteArray fileContent;
	if(!FileReader::read(path, fileContent))
		return path;
	fileContent.append((char)0);

	int LinkFileSize = fileContent.size();
	for(int i = 0; i<fileContent.size() && i<sizeof(LinkFileContent); ++i) {
		LinkFileContent[i] = fileContent.at(i);
	}
	HeaderStructure._signature = LinkFileContent[0]*65536*256;
	HeaderStructure._signature += LinkFileContent[1]*65536;
	HeaderStructure._signature += LinkFileContent[2]*256+LinkFileContent[3];
	printf("Signature: %1\n", HeaderStructure.Signature);

	for(int i = 0; i<16; i++) {
		HeaderStructure.GUID[i] = LinkFileContent[4+i];
	};

	HeaderStructure.ShortcutFlags = LinkFileContent[23]*65536*256;
	HeaderStructure.ShortcutFlags += LinkFileContent[22]*65536;
	HeaderStructure.ShortcutFlags += LinkFileContent[21]*256+LinkFileContent[20];
	printf("Shortcut Flags: %1\n", HeaderStructure.ShortcutFlags);

	HeaderStructure.TargetFileFlags = LinkFileContent[27]*65536*256;
	HeaderStructure.TargetFileFlags += LinkFileContent[26]*65536;
	HeaderStructure.TargetFileFlags += LinkFileContent[25]*256+LinkFileContent[24];
	printf("Target File Flags: %1\n", HeaderStructure.TargetFileFlags);

	for(int i = 0; i<8; i++) { HeaderStructure.CreationTime[i] = LinkFileContent[28+i]; };
	for(int i = 0; i<8; i++) { HeaderStructure.LastAccessTime[i] = LinkFileContent[36+i]; };
	for(int i = 0; i<8; i++) { HeaderStructure.LastAccessTime[i] = LinkFileContent[44+i]; };

	HeaderStructure.FileLength = LinkFileContent[55]*65536*256;
	HeaderStructure.FileLength += LinkFileContent[54]*65536;
	HeaderStructure.FileLength += LinkFileContent[53]*256+LinkFileContent[52];
	printf("Target File Size (bytes): %1\n", HeaderStructure.FileLength);

	HeaderStructure.IconNumber = LinkFileContent[59]*65536*256;
	HeaderStructure.IconNumber += LinkFileContent[58]*65536;
	HeaderStructure.IconNumber += LinkFileContent[57]*256+LinkFileContent[56];
	printf("IconNumber: %1\n", HeaderStructure.IconNumber);

	HeaderStructure.ShowWindow = LinkFileContent[63]*65536*256;
	HeaderStructure.ShowWindow += LinkFileContent[62]*65536;
	HeaderStructure.ShowWindow += LinkFileContent[61]*256+LinkFileContent[60];
	printf("ShowWindow Attributes: %1\n", HeaderStructure.IconNumber);

	HeaderStructure.HotKey = LinkFileContent[67]*65536*256;
	HeaderStructure.HotKey += LinkFileContent[66]*65536;
	HeaderStructure.HotKey += LinkFileContent[65]*256+LinkFileContent[64];
	printf("HotKey Attributes: %1\n", HeaderStructure.HotKey);

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