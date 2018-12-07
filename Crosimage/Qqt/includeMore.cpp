//includeMore.cpp by Kostya Kozachuck as neurocod
//BSD license https://github.com/neurocod/Qqt
#include "pch.h"
#include "includeMore.h"

LPCSTR NLStyle::s_strDateTimeFormatFull = "yyyy.MM.dd hh:mm:ss.zzz";

CompactTextEdit::CompactTextEdit(QWidget*parent):
		QTextEdit(parent) {
}
//virtual
QSize CompactTextEdit::sizeHint()const {
	QSize sz = __super::sizeHint();
	sz.setHeight(30);
	return sz;
}
//virtual
QSize CompactTextEdit::minimumSizeHint()const {
	QSize sz(30,30);
	return sz;
}
//________________________________________________________________
CompactLineEdit::CompactLineEdit(QWidget*parent):
		QLineEdit(parent) {
}
//virtual
QSize CompactLineEdit::sizeHint()const {
	QSize sz = __super::sizeHint();
	sz.setWidth(128);
	return sz;
}
//virtual
QSize CompactLineEdit::minimumSizeHint()const {
	QSize sz = __super::minimumSizeHint();
	return sz;
}
