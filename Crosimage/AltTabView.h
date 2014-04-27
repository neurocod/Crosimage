//AltTabView.h by Kostya Kozachuck as neurocod - 2014.01.09 12:54:48
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
		QMap<QWidget*, AltTabLabel*> m_mains;
		QPointer<AltTabLabel> m_selected;
		QGridLayout* m_lay;
		int m_colCount;
		int m_nNextRow;
		int m_nNextCol;
		static QWidgetList topLevelWidgets();
		QPixmap grab(QWidget*w);
};