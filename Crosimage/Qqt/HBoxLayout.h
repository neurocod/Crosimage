//HBoxLayout.h by Kostya Kozachuck as neurocod - 19.07.2011 9:46:49
#pragma once
#include "BoxLayout.h"

class HBoxLayout: public BoxLayout {
	MAYBE_SUPER(BoxLayout)
	public:
		HBoxLayout() : BoxLayout(new QHBoxLayout()) {
			d = staticCast<QHBoxLayout*>();
		}
		HBoxLayout(QWidget*parent) : HBoxLayout() {
			parent->setLayout(d);
		}
		HBoxLayout(QWidget&parent): HBoxLayout() {
			parent.setLayout(d);
		}
		HBoxLayout(QBoxLayout*parent) : HBoxLayout() {
			parent->addLayout(d);
		}

		EMBED_QPOINTER_AND_CAST(QHBoxLayout)
};