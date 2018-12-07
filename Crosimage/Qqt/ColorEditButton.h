//ColorEditButton.h by Kostya Kozachuck as neurocod
#pragma once

class ColorEditButton: public QObject, public ToolButton {
	Q_OBJECT
	public:
		template<typename ... Args>
		ColorEditButton(Args...args): ToolButton(args...), color(this) {
			//if(!toolButtonStyle set)
				toolButtonStyle = Qt::ToolButtonTextBesideIcon;
			connectClicks(this, &ColorEditButton::onClick);
		}

		static void setIconByColor(QToolButton* btn, const QColor & clr);
		QString dialogTitle;
		class ColorProperty {
			public:
				operator QColor()const { return _color; }
				QColor operator=(const QColor & clr);
			protected:
				QColor _color;
				ColorEditButton & _d;
				friend class ColorEditButton;
				ColorProperty(ColorEditButton*d): _d(*d) {}
		} color;
		QColorDialog::ColorDialogOptions colorDialogOptions = QColorDialog::ShowAlphaChannel;
	signals:
		void colorChanged(const QColor & clr);
	protected:
		void onClick();
};