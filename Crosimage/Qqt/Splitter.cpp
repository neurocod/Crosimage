//Splitter.cpp by Kostya Kozachuck as neurocod - 16.09.2011 18:59:11
#include "pch.h"
#include "Splitter.h"

Splitter & Splitter::operator<<(QWidget*w) {
	d->addWidget(w);
	return *this;
}
Splitter & Splitter::operator<<(QLayout*lay) {
	Widget w;
	w->setLayout(lay);
	d->addWidget(w);
	return *this;
}