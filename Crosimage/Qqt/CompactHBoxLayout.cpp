﻿//CompactHBoxLayout.cpp by Kostya Kozachuck as neurocod - 19.07.2011 9:47:10
#include "pch.h"
#include "CompactHBoxLayout.h"

QHBoxLayout* compactHBoxLayout(QWidget*parent) {
	QHBoxLayout* layout = new QHBoxLayout(parent);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	return layout;
}
//________________________________________________________________
CompactHBoxLayout::CompactHBoxLayout() {
	setCompact();
}
CompactHBoxLayout::CompactHBoxLayout(QWidget*parent): HBoxLayout(parent) {
	setCompact();
}
CompactHBoxLayout::CompactHBoxLayout(QWidget&parent) : HBoxLayout(parent) {
	setCompact();
}
CompactHBoxLayout::CompactHBoxLayout(QBoxLayout*parent): HBoxLayout(parent) {
	setCompact();
}