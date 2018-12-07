//ToString.h by Kostya Kozachuck as neurocod - 03.12.2012 21:22:06
#pragma once
#include <QTextStream>

namespace Qqt {
	QString toString(double n);
	QString toString(decltype(endl));
	QString toString(bool n);
	QString toString(char n);
	QString toString(qint32 n);
	QString toString(qint64 n);
	QString toString(quint32 n);
	QString toString(quint64 n);
	QString toString(QEvent::Type t);
	QString toString(const QColor & c);
	QString toString(const QSize & sz);
	QString toString(const QSizeF & sz);
	QString toString(const QRect & r);
	QString toString(const QRectF & r);
	QString toString(const QPoint & p);
	QString toString(const QPointF & p);
	QString toString(const QString & str);
	QString toString(const QDateTime & d);
	QString toString(const QJsonValue & d);
	QString reversed(const QString & str);
	template<class T>
	QList<T> reversed(const QList<T> & li) {
		QList<T> ret;
		for(auto t: li)
			ret.prepend(t);
		return ret;
	}
	QByteArray toByteArray(const QString & str);
	bool isValid(const QColor & clr);//see https://bugreports.qt.io/browse/QTBUG-41572
}
//rtr = russian translate - old function for compatibility. Need be replaced by QObject::tr().
QString rtr(const wchar_t* rus, const char* eng);