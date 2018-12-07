//VBoxLayout.h by Kostya Kozachuck as neurocod - 19.07.2011 9:47:06
#pragma once
#include "BoxLayout.h"

class VBoxLayout: public BoxLayout {
	MAYBE_SUPER(BoxLayout)
	public:
		VBoxLayout() : BoxLayout(new QVBoxLayout()) {
			d = staticCast<QVBoxLayout*>();
		}
		VBoxLayout(QWidget*parent) : VBoxLayout() {
			parent->setLayout(d);
		}
		VBoxLayout(QWidget & parent): VBoxLayout() {
			parent.setLayout(d);
		}
		VBoxLayout(QBoxLayout*parent) : VBoxLayout() {
			parent->addLayout(d);
		}

		EMBED_QPOINTER_AND_CAST(QVBoxLayout)
	protected:
		void init();
};
#define VBOXLAYOUT QBoxLayout* _layOld = lay; VBoxLayout lay(_layOld);
#define VBOXLAYOUT00 QBoxLayout* _layOld = lay; VBoxLayout00 lay(_layOld);
#define HBOXLAYOUT QBoxLayout* _layOld = lay; HBoxLayout lay(_layOld);
#define HBOXLAYOUT00 QBoxLayout* _layOld = lay; HBoxLayout00 lay(_layOld);