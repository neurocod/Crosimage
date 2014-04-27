//AltTabLabel.h by Kostya Kozachuck as neurocod - 2014.01.09 14:05:36
#pragma once
class AltTabView;

class AltTabLabel: public QLabel {
	public:
		AltTabLabel(AltTabView*parent, QWidget*wToActivate);
		virtual ~AltTabLabel();
		QPointer<QWidget> m_widToActivate;
	protected:
		virtual void keyPressEvent(QKeyEvent * event)override;
		virtual void mousePressEvent(QMouseEvent * event)override;
		virtual void mouseMoveEvent(QMouseEvent * event)override;
		virtual void paintEvent(QPaintEvent * e)override;
		AltTabView*m_parent;
};