//ComboBox.cpp by Kostya Kozachuck as neurocod - 11.03.2012 7:19:02
#include "pch.h"
#include "ComboBox.h"

ComboBox& ComboBox::operator<<(const QStringList & texts) {
	d->addItems(texts);
	return *this;
}
ComboBox& ComboBox::operator<<(const QString & text) {
	d->addItem(text);
	return *this;
}
void ComboBox::add(const QString & text, const QVariant & userData) {
	d->addItem(text, userData);
}
void ComboBox::add(const QIcon & icon, const QString & text, const QVariant & userData) {
	d->addItem(icon, text, userData);
}