//AltTabLabel.h by Kostya Kozachuck as neurocod
#pragma once
class AltTabView;

class AltTabLabel: public QLabel {
	public:
		AltTabLabel(AltTabView*parent, QWidget*wToActivate);
		virtual ~AltTabLabel();
		QPointer<QWidget> _widToActivate;
	protected:
		virtual void keyPressEvent(QKeyEvent * event)override;
		virtual void mousePressEvent(QMouseEvent * event)override;
		virtual void mouseMoveEvent(QMouseEvent * event)override;
		virtual void paintEvent(QPaintEvent * e)override;
		AltTabView* _parent;
};