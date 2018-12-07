//ColorEditButton.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "ColorEditButton.h"

void ColorEditButton::onClick() {
	QColor clr = QColorDialog::getColor(color._color, (QToolButton*)*this, dialogTitle, colorDialogOptions);
	if(!clr.isValid())
		return;
	color = clr;
}
QColor ColorEditButton::ColorProperty::operator=(const QColor & clr) {
	_color = clr;
	_d.setIconByColor(_d, clr);
	emit _d.colorChanged(clr);
	return clr;
}
void ColorEditButton::setIconByColor(QToolButton* btn, const QColor & clr) {
	QPixmap pix(16, 16);
	pix.fill(clr);
	btn->setIcon(pix);
}