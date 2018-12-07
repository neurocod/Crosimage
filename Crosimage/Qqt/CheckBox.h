//CheckBox.h by Kostya Kozachuck as neurocod - 16.09.2011 1:08:43
#pragma once

class CheckBox: public AbstractButtonPropertyRedirects {
	public:
		void operator=(CheckBox&) = delete;
		template<typename ... Args>
		CheckBox(Args... args): AbstractButtonPropertyRedirects(new QCheckBox) {
			d = staticCast<QCheckBox*>();
			checkState.init(d);

			CtorProcessorT<CheckBox> p(*this);
			p.process_(args...);
		}
		PROPERTY_REDIRECTV(QCheckBox, bool, tristate, isTristate, setTristate);
		PROPERTY_REDIRECTV(QCheckBox, Qt::CheckState, checkState, checkState, setCheckState);

		EMBED_QPOINTER_AND_CAST(QCheckBox)
};