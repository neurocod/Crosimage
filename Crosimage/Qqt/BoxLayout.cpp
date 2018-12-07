//BoxLayout.cpp by Kostya Kozachuck as neurocod - 19.07.2011 9:47:17
#include "pch.h"
#include "BoxLayout.h"

BoxLayout& BoxLayout::operator<<(QWidget*w) {
	d->addWidget(w);
	return *this;
}
BoxLayout& BoxLayout::operator<<(QWidget&w) {
	d->addWidget(&w);
	return *this;
}
BoxLayout& BoxLayout::operator<<(QLayout*l) {
	d->addLayout(l);
	return *this;
}
BoxLayout& BoxLayout::operator<<(QLayoutItem * item) {
	d->addItem(item);
	return *this;
}
QLabel* BoxLayout::addLabel(const QString & str) {
	auto ret = new QLabel(str);
	*this << ret;
	return ret;
}
BoxLayout& BoxLayout::operator<<(const QString & str) {
	addLabel(str);
	return *this;
}
void BoxLayout::setCompact() {
	d->setMargin(0);
	d->setSpacing(0);
}
void BoxLayout::insertStretch(int index, int stretch) {
	d->insertStretch(index, stretch);
}
void BoxLayout::addStretch(int stretch) {
	d->addStretch(stretch);
}