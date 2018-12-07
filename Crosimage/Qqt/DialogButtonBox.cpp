//DialogButtonBox.cpp by Kostya Kozachuck as neurocod - 13.12.2012 9:09:01
#include "pch.h"
//#include "DialogButtonBox.h"

DialogButtonBox::DialogButtonBox(QDialog*parent, bool addOkCancelAcceptReject): QDialogButtonBox(parent) {
	if(addOkCancelAcceptReject) {
		addOk();
		addCancel();
		acceptRejectTo(parent);
	}
}
QPushButton* DialogButtonBox::addOk() {
	auto ret = addButton(QDialogButtonBox::Ok);
	ret->setIcon(okIcon());
	return ret;
}
QPushButton* DialogButtonBox::addCancel() {
	auto ret = addButton(QDialogButtonBox::Cancel);
	ret->setIcon(cancelIcon());
	return ret;
}
void DialogButtonBox::acceptRejectTo(QDialog*dlg) {
	connect(this, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
	connect(this, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
}
const QPixmap& DialogButtonBox::cancelIcon() {
	static const QPixmap ret(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-32.png");
	return ret;
}
const QPixmap& DialogButtonBox::okIcon() {
	static const QPixmap ret(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-32.png");
	return ret;
}
