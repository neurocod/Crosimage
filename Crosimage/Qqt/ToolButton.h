//ToolButton.h by Kostya Kozachuck as neurocod - 28.08.2011 0:27:58
#pragma once

class ToolButton: public AbstractButtonPropertyRedirects {
	public:
		template<typename ... Args>
		ToolButton(Args...args): AbstractButtonPropertyRedirects(new QToolButton()) {
			d = staticCast<QToolButton*>();
			defaultAction.init(d);
			toolButtonStyle.init(d);
			arrowType.init(d);
			autoRaise.init(d);
			popupMode.init(d);

			CtorProcessor p(*this);
			p.process_(args...);
		}
		PROPERTY_REDIRECTV(QToolButton, Qt::ToolButtonStyle, toolButtonStyle, toolButtonStyle, setToolButtonStyle);
		PROPERTY_REDIRECTV(QToolButton, QAction*, defaultAction, defaultAction, setDefaultAction);
		PROPERTY_REDIRECTV(QToolButton, Qt::ArrowType, arrowType, arrowType, setArrowType);
		PROPERTY_REDIRECTV(QToolButton, bool, autoRaise, autoRaise, setAutoRaise);
		PROPERTY_REDIRECTV(QToolButton, QToolButton::ToolButtonPopupMode, popupMode, popupMode, setPopupMode);

		EMBED_QPOINTER_AND_CAST(QToolButton)
	protected:
	struct CtorProcessor: public CtorProcessorT<ToolButton> {
		CtorProcessor(ToolButton&d): CtorProcessorT(d) {}
		using CtorProcessorT::process;
		void process(QAction*a) {
			_d->setDefaultAction(a);
		}
		void process_() { }
		template<class Arg, typename ... Args>
		void process_(Arg arg, Args...args) {
			process(arg);
			process_(args...);
		}
	};
};