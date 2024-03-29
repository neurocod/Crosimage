﻿//Widget.cpp by Kostya Kozachuck as neurocod - 16.09.2011 18:53:45
#include "pch.h"
#include "Widget.h"

void setWindowTitleDebug(QWidget*w, const QString & str) {
#ifdef _DEBUG
	QString t = str;
	t += " - class " + QString(w->metaObject()->className());
	w->setWindowTitle(t);
#else
	w->setWindowTitle(str);
#endif
}
void setWindowTitleDebug(QWidget*w, const QString & title, const char*function) {
#ifdef _DEBUG
	QString str = title;
	QString name = function;
	int index = name.indexOf("::");
	if(-1==index)
		name = w->metaObject()->className();
	else
		name.truncate(index);
	str += " - class " + name;
	w->setWindowTitle(str);
#else
	w->setWindowTitle(title);
#endif
}
QRect Widget::mapToGlobal(QWidget*w, const QRect & r) {
	auto p1 = r.topLeft();
	auto p2 = r.bottomRight();
	p1 = w->mapToGlobal(p1);
	p2 = w->mapToGlobal(p2);
	return QRect(p1, p2);
}
qreal Widget::dpiScale() {
	return qApp->devicePixelRatio();
}
int Widget::mapDpi(int pixelsLowDpi) {
	return pixelsLowDpi * dpiScale();
}
qreal Widget::mapDpi(qreal pixelsLowDpi) {
	return pixelsLowDpi* dpiScale();
}
//can't make it work in header files in gcc because pch not working etc
template<class Arg>
QWidget* Widget_widgetFromArg(Arg arg) {
	return 0;
}
template<>
QWidget* Widget_widgetFromArg(QWidget*w) {
	return w;
}
QWidget* Widget_widgetFromArgs() {
	return 0;
}
template<class Arg, typename ... Args>
QWidget* Widget_widgetFromArgs(Arg arg, Args...args) {
	if(auto w = Widget_widgetFromArg(arg))
		return w;
	return Widget_widgetFromArgs(args...);
}
