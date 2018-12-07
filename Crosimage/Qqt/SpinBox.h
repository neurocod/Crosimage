//SpinBox.h by Kostya Kozachuck as neurocod - 11.03.2012 20:16:58
#pragma once
#include "AbstractSpinBoxPropertyRedirects.h"

class SpinBox: public AbstractSpinBoxPropertyRedirects {
	MAYBE_SUPER(AbstractSpinBoxPropertyRedirects)
	public:
		template<typename ... Args>
		SpinBox(Args...args): AbstractSpinBoxPropertyRedirects(new QSpinBox) {
			d = staticCast<QSpinBox*>();
			maximum.init(d);
			minimum.init(d);
			prefix.init(d);
			singleStep.init(d);
			suffix.init(d);
			value.init(d);

			CtorProcessorT<SpinBox> p(*this);
			p.process_(args...);
		}

		//PROPERTY_REDIRECTV(QSpinBox, const QString, cleanText
		PROPERTY_REDIRECTV(QSpinBox, int, maximum, maximum, setMaximum);
		PROPERTY_REDIRECTV(QSpinBox, int, minimum, minimum, setMinimum);
		PROPERTY_REDIRECT (QSpinBox, QString, prefix, prefix, setPrefix);
		PROPERTY_REDIRECTV(QSpinBox, int, singleStep, singleStep, setSingleStep);
		PROPERTY_REDIRECT (QSpinBox, QString, suffix, suffix, setSuffix);
		PROPERTY_REDIRECTV(QSpinBox, int, value, value, setValue);

		template<class T1, class T2>
		void connectValueChangedInt(T1*receiver, const T2 slot, Qt::ConnectionType type = Qt::AutoConnection) {
			QSpinBox* btn = maximum.destination();
			QObject::connect(btn, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), receiver, slot, type);
		}
		template<class T1, class T2>
		void connectValueChangedStr(T1*receiver, const T2 slot, Qt::ConnectionType type = Qt::AutoConnection) {
			QSpinBox* btn = maximum.destination();
			QObject::connect(btn, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), receiver, slot, type);
		}

		EMBED_QPOINTER_AND_CAST(QSpinBox)
};