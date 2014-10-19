//AltTabView.h by Kostya Kozachuck as neurocod
#pragma once
class AltTabLabel;

class AltTabView: public QWidget {
	Q_OBJECT
	public:
		AltTabView();
		virtual ~AltTabView();
		static void showInstance();
		void clicked(QWidget*w);
		void select(AltTabLabel*p);
		void deleted(AltTabLabel*p);
	public slots:
		void updateUI();
	protected:
		QMap<QWidget*, AltTabLabel*> _mains;
		QPointer<AltTabLabel> _selected;
		QGridLayout* _lay;
		int _colCount;
		int _nNextRow;
		int _nNextCol;
		static QWidgetList topLevelWidgets();
		QPixmap grab(QWidget*w);
};