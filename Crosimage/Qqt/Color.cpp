//Color.cpp by Konstantine Kozachuck as neurocod
#include "pch.h"
#include "Color.h"
#ifndef QT_GUI_LIB

QColor::QColor(const QString & str) {
	QString s = str.trimmed();
	if(s.startsWith('#'))
		s.remove(0, 1);
	if(s.length() == 6) {
		WIP;
	} else {
		WIP;
	}
}
QColor::QColor(int rgba) {
	r = rgba & 0xff0000;
	g = rgba & 0x00ff00;
	b = rgba & 0x0000ff;
}
bool QColor::isValid()const {
	return r>=0 && g>=0 && b>=0;
}
QString QColor::toString()const {
	return Qqt::toString(*this);
}
bool QColor::operator==(const QColor &c)const {
	return c.r == r
		&& c.g == g
		&& c.b == b;
}
#endif