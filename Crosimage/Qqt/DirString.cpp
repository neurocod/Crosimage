//DirString.cpp by Kostya Kozachuck as neurocod - 12.08.2011 18:28:31
#include "pch.h"
#include "DirString.h"

DirString::DirString() {
}
DirString::DirString(const QString & path): _str(path) {
	correctSlash();
}
void DirString::correctSlash() {
	if(!_str.isEmpty() && !_str.endsWith('/'))
		_str += '/';
}
//static
int DirString::length(const QStringList & li) {
	int len = 0;
	for(auto str: li) {
		len += str.length();
		if(!str.endsWith('/'))
			len++;
	}
	return len;
}
bool DirString::isEmpty()const {
	return _str.isEmpty();
}
DirString& DirString::operator+=(const QString & str) {
	if(!isEmpty() &&
		(str.startsWith('/') || str.startsWith('\\'))
	) {
		_str.remove(0, 1);
	}
	_str += str;
	correctSlash();
	return *this;
}
QString DirString::visualStudioSolutionDir() {//static
	QDir dir = QCoreApplication::applicationDirPath();//Debug/Release
	dir.cdUp();//x64/Win32
	dir.cdUp();//solution
	return dir.absolutePath();
}