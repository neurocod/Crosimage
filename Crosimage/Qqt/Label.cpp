//Label.cpp by Kostya Kozachuck as neurocod - 29.02.2012 1:30:10
#include "pch.h"
#include "Label.h"

void Label::set(const QImage & img) {
	set(QPixmap::fromImage(img));
}
void Label::set(const QPixmap& img) {
	d->setPixmap(img);
}
void Label::set(const QString& str) {
	d->setText(str);
}
void Label::operator=(const QImage & img) {
	set(img);
}
void Label::operator=(const QPixmap& img) {
	set(img);
}
void Label::operator=(const QString& str) {
	set(str);
}