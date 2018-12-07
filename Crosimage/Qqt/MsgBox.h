//MsgBox.h by Kostya Kozachuck as neurocod - 23.12.2011 6:34:32
#pragma once
#include "Widget.h"

class MsgBox: public QMessageBox {
	public:
		using CtorProcessor = CtorProcessorT<MsgBox>;
		template<typename... Args>
		MsgBox(Args...args)//: QMessageBox(Widget_widgetFromArgs(args...)) {
		//in windows this is ok ^ but in gcc - not
		{
			detailedText.init(this);
			icon.init(this);
			iconPixmap.init(this);
			informativeText.init(this);
			standardButtons.init(this);
			text.init(this);
			textFormat.init(this);

			icon = MsgBox::Information;
			standardButtons = QMessageBox::Ok;
			setCustomStyle();

			CtorProcessor p(*this);
			p.process_(args...);
		}

		typedef QMessageBox::StandardButton StandardButton;
		typedef QMessageBox::StandardButton Btn;
		typedef QMessageBox::StandardButtons StandardButtons;

		PROPERTY_REDIRECT (QMessageBox, QString, detailedText, detailedText, setDetailedText);
		PROPERTY_REDIRECTV(QMessageBox, Icon, icon, icon, setIcon);
		PROPERTY_REDIRECT (QMessageBox, QPixmap, iconPixmap, iconPixmap, setIconPixmap);
		PROPERTY_REDIRECT (QMessageBox, QString, informativeText, informativeText, setInformativeText);
		PROPERTY_REDIRECTV(QMessageBox, StandardButtons, standardButtons, standardButtons, setStandardButtons);
		PROPERTY_REDIRECT (QMessageBox, QString, text, text, setText);
		PROPERTY_REDIRECTV(QMessageBox, Qt::TextFormat, textFormat, textFormat, setTextFormat);

		//StandardButtons buttons;
		int _nStringParam = 0;

		void set(const QString & str) {
			if(_nStringParam==0) {
				text = str;
			} else {
				setWindowTitle(text);
				text = str;
			}
			_nStringParam++;
			if(_nStringParam>2) {
				ASSERT(0);
				return;
			}
		}
		void set(QMessageBox::StandardButtons _buttons) {
			standardButtons = _buttons;
		}
		void set(QMessageBox::Icon ico) {
			icon = ico;
		}
		void set(QMessageBox::StandardButton _defaultButton) {
			setDefaultButton(_defaultButton);
		}
		Btn execReturnBtn() {
			if(exec()==-1)
				return Cancel;
			return standardButton(clickedButton());
		}
		QMessageBox* operator->() { return this; }
		void setCustomStyle();
		static int display(QtMsgType type, const QMessageLogContext & context, const QString &msg);
};
//________________________________________________________________
MsgBox::Btn msgBox(const QString & t1);

//S means toString() first param
template<class T1>
MsgBox::Btn msgBoxT(T1 t1) {
	return msgBox(toString(t1));
}

template<class T1, class T2>
MsgBox::Btn msgBox(T1 t1, T2 t2) {
	MsgBox box;
	box.icon = MsgBox::Information;
	box.set(t1);
	box.set(t2);
	return box.execReturnBtn();
}
template<class T1, class T2, class T3>
MsgBox::Btn msgBox(T1 t1, T2 t2, T3 t3) {
	MsgBox box;
	box.icon = MsgBox::Information;
	box.set(t1);
	box.set(t2);
	box.set(t3);
	return box.execReturnBtn();
}
