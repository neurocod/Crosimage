//ToolBar.cpp by Kostya Kozachuck as neurocod - 08.11.2011 1:25:55
#include "pch.h"
#include "ToolBar.h"

ToolBar & ToolBar::operator<<(QWidget*w) {
	d->addWidget(w);
	return *this;
}
ToolBar & ToolBar::operator<<(QAction*a) {
	d->addAction(a);
	return *this;
}
ToolBar & ToolBar::operator<<(const QList<QAction*>& li) {
	for(auto a: li)
		*this << a;
	return *this;
}
ToolBar & ToolBar::operator<<(const QString & label) {
	return *this << new QLabel(label);
}
QToolBar* ToolBar::newRotatableToolBar() {
	return new RotatableToolBar();
}