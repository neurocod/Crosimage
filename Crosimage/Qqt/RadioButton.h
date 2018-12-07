//RadioButton.h by Kostya Kozachuck as neurocod - 11.03.2012 6:50:31
#pragma once

class RadioButton: public AbstractButtonPropertyRedirects {
	MAYBE_SUPER(AbstractButtonPropertyRedirects)
	public:
		template<typename... Args>
		RadioButton(Args...args) : AbstractButtonPropertyRedirects(new QRadioButton()) {
			d = staticCast<QRadioButton*>();

			CtorProcessorT<RadioButton> p(*this);
			p.process_(args...);
		}

		EMBED_QPOINTER_AND_CAST(QRadioButton)
};