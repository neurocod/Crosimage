//BoxLayout.h by Kostya Kozachuck as neurocod - 19.07.2011 9:47:16
#pragma once
#include "Layout.h"

class BoxLayout: public Layout {
	MAYBE_SUPER(Layout)
	public:
		BoxLayout(QBoxLayout*_d): Layout(_d) {
			d = _d;
		}
		BoxLayout& operator<<(QWidget&w);
		BoxLayout& operator<<(QWidget*w);
		BoxLayout& operator<<(QLayout*l);
		BoxLayout& operator<<(QLayoutItem * item);
		BoxLayout& operator<<(const QString & str);//adds QLabel
		QLabel* addLabel(const QString & str);
		void setCompact();
		void insertStretch(int index=-1, int stretch = 0);
		void addStretch(int stretch = 0);

		//int count
		EMBED_QPOINTER_AND_CAST(QBoxLayout)
};