﻿//CompactHBoxLayout.h by Kostya Kozachuck as neurocod - 19.07.2011 9:47:09
#pragma once
#include "HBoxLayout.h"

QHBoxLayout* compactHBoxLayout(QWidget*parent);

class CompactHBoxLayout: public HBoxLayout {
	public:
		CompactHBoxLayout();
		CompactHBoxLayout(QWidget*parent);
		CompactHBoxLayout(QWidget&parent);
		CompactHBoxLayout(QBoxLayout*parent);
};
typedef CompactHBoxLayout HBoxLayout00;