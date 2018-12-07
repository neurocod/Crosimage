//Layout.h by Kostya Kozachuck as neurocod - 11.03.2012 21:04:56
#pragma once

//abstract - properties redirect
class Layout: public ObjectPropertyRedirects {
	MAYBE_SUPER(ObjectPropertyRedirects)
	public:
		Layout(QLayout*_d): ObjectPropertyRedirects(_d) {
			d = _d;
			sizeConstraint.init(d);
			spacing.init(d);
		}

		PROPERTY_REDIRECTV(QLayout, QLayout::SizeConstraint, sizeConstraint, sizeConstraint, setSizeConstraint);
		PROPERTY_REDIRECTV(QLayout, int, spacing, spacing, setSpacing);
		EMBED_QPOINTER_AND_CAST(QLayout)
};