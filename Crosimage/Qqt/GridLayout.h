//GridLayout.h by Kostya Kozachuck as neurocod - 11.03.2012 21:00:45
#pragma once

class GridLayout: public Layout {
	MAYBE_SUPER(Layout)
	public:
		GridLayout(): Layout(new QGridLayout) {
			d = staticCast<QGridLayout*>();
		}
		EMBED_QPOINTER_AND_CAST(QGridLayout)
};