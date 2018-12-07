//Color.h by Konstantine Kozachuck as neurocod
#pragma once
#ifndef QT_GUI_LIB

struct QColor {
	qint16 r = 0;
	qint16 g = 0;
	qint16 b = 0;
	int red()const { return r; }
	int green()const { return g; }
	int blue()const { return b; }
	bool operator==(const QColor &c)const;
	
	QColor() {}
	QColor(int rgba);
	QColor(const QString & str);
	bool isValid()const;
	operator QVariant()const { return toString(); }
	QString toString()const;
};
#endif