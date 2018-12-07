//Console.cpp by Kostya Kozachuck as neurocod - 2018.05.15 14:35:58
#include "pch.h"
#include "Console.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

Console::Console(): QTextStream(stdout) {
}
Console& Console::setTextColor(QRgb color, bool intense) {
	flush();
#ifdef Q_OS_WIN
	int flags = 0;
	if(qRed(color)>127)
		flags |= FOREGROUND_RED;
	if(qGreen(color)>127)
		flags |= FOREGROUND_GREEN;
	if(qBlue(color)>127)
		flags |= FOREGROUND_BLUE;
	if(intense)
		flags |= FOREGROUND_INTENSITY;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, flags);
#else
#pragma error "not reaized"
#endif
	return *this;
}
Console& Console::setTextColor() {
	return setTextColor(0xffffff, false);
}
Console& Console::writeDateTime() {
	*this << QDateTime::currentDateTime().toLocalTime().toString("yyyy.MM.dd hh:mm:ss ");
	return *this;
}
void Console::setWindowTitle(const QString & s) {
	SetConsoleTitleA(s.toLatin1().data());
}
Console& Console::write(const QString & text, QRgb color) {
	setTextColor(color);
	*this << text;
	setTextColor();
	return *this;
}