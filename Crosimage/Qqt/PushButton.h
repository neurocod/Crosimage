//PushButton.h by Kostya Kozachuck as neurocod - 27.08.2011 21:20:17
#pragma once

class PushButton: public QObject, public AbstractButtonPropertyRedirects {
	public:
		template<typename...Args>
		PushButton(Args...args) : AbstractButtonPropertyRedirects(new QPushButton) {
			d = staticCast<QPushButton*>();
			defaultAction.init(this);

			CtorProcessorT<PushButton> p(*this);
			p.process_(args...);
		}
		QAction* defaultAction_()const { return _defaultAction; }
		void setDefaultAction(QAction *action);

		PROPERTY_REDIRECTV(PushButton, QAction*, defaultAction, defaultAction_, setDefaultAction);

		EMBED_QPOINTER_AND_CAST(QPushButton)
	protected:
		QPointer<QAction> _defaultAction;
};
