//Menu.cpp by Kostya Kozachuck as neurocod - 16.09.2011 2:01:02
#include "pch.h"
#include "Menu.h"

void Menu::showUnder(QWidget*w) {
	QPoint pt = w->mapToGlobal(w->rect().bottomLeft());
	icon.destination()->popup(pt);
}
Menu& Menu::operator<<(QAction*act) {
	d->addAction(act);
	return *this;
}