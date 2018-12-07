//Dialog.h by Konstantine Kozachuck as neurocod - 2018.09.10
#pragma once
#include "DialogButtonBox.h"

//exclude help title button
class Dialog: public QDialog {
	public:
		Dialog(QWidget* parent = 0);
};