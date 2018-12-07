//FormLayout.cpp by Kostya Kozachuck as neurocod - 28.06.2012 21:59:11
#include "pch.h"
#include "FormLayout.h"

FormLayout& FormLayout::operator << (QWidget*w) {
	d->addRow(w);
	return *this;
}
FormLayout& FormLayout::operator << (const QString & str) {
	d->addRow(new QLabel(str));
	return *this;
}
QLabel* FormLayout::addHtmlRow(const QString & strHtml, QWidget *field) {
	QLabel * label = new QLabel;
	label->setTextFormat(Qt::RichText);
	label->setText(strHtml);
	if(field)
		d->addRow(label, field);
	else
		d->addRow(label);
	return label;
}