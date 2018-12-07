//ProgressBar.h by Kostya Kozachuck as neurocod - 08.05.2012 14:21:12
#pragma once

class ProgressBar: public WidgetPropertyRedirects {
	MAYBE_SUPER(WidgetPropertyRedirects)
	public:
		template<typename ... Args>
		ProgressBar(Args...args): WidgetPropertyRedirects(new QProgressBar){
			d = staticCast<QProgressBar*>();
			alignment.init(d);
			format.init(d);
			invertedAppearance.init(d);
			maximum.init(d);
			minimum.init(d);
			orientation.init(d);
			//text.init(d);
			textDirection.init(d);
			textVisible.init(d);
			value.init(d);

			CtorProcessorT<ProgressBar> p(*this);
			p.process_(args...);
		}

		PROPERTY_REDIRECTV(QProgressBar, Qt::Alignment, alignment, alignment, setAlignment);
		PROPERTY_REDIRECT (QProgressBar, QString, format, format, setFormat);
		PROPERTY_REDIRECTV(QProgressBar, bool, invertedAppearance, invertedAppearance, setInvertedAppearance);
		PROPERTY_REDIRECTV(QProgressBar, int, maximum, maximum, setMaximum);
		PROPERTY_REDIRECTV(QProgressBar, int, minimum, minimum, setMinimum);
		PROPERTY_REDIRECTV(QProgressBar, Qt::Orientation, orientation, orientation, setOrientation);
		//PROPERTY_REDIRECTV(QProgressBar, const QString, text);
		PROPERTY_REDIRECTV(QProgressBar, QProgressBar::Direction, textDirection, textDirection, setTextDirection);
		PROPERTY_REDIRECTV(QProgressBar, bool, textVisible, isTextVisible, setTextVisible);
		PROPERTY_REDIRECTV(QProgressBar, int, value, value, setValue);

		EMBED_QPOINTER_AND_CAST(QProgressBar)
};