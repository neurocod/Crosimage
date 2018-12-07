//ToolBar.h by Kostya Kozachuck as neurocod - 08.11.2011 1:25:55
#pragma once

class ToolBar: public WidgetPropertyRedirects {
	public:
		void operator=(QToolBar*b);
		template<typename ...Args>
		ToolBar(Args...args) : WidgetPropertyRedirects(newRotatableToolBar()) {
			d = staticCast<QToolBar*>();
			allowedAreas.init(d);
			floatable.init(d);
			//floating.init(d);
			iconSize.init(d);
			movable.init(d);
			orientation.init(d);
			toolButtonStyle.init(d);

			d->setAutoFillBackground(true);//for color management using palettes
			CtorProcessorT<ToolBar> p(*this);
			p.process_(args...);
		}

		ToolBar & operator<<(QWidget*w);
		ToolBar & operator<<(QAction*a);
		ToolBar & operator<<(const QString & label);
		ToolBar & operator<<(const QList<QAction*>& li);
		PROPERTY_REDIRECTV(QToolBar, Qt::ToolBarAreas, allowedAreas, allowedAreas, setAllowedAreas);
		PROPERTY_REDIRECTV(QToolBar, bool, floatable, isFloatable, setFloatable);
		//PROPERTY_REDIRECTV(QToolBar, const bool floating, floating, setFloating);
		PROPERTY_REDIRECT (QToolBar, QSize, iconSize, iconSize, setIconSize);
		PROPERTY_REDIRECTV(QToolBar, bool, movable, isMovable, setMovable);
		PROPERTY_REDIRECTV(QToolBar, Qt::Orientation, orientation, orientation, setOrientation);
		PROPERTY_REDIRECTV(QToolBar, Qt::ToolButtonStyle, toolButtonStyle, toolButtonStyle, setToolButtonStyle);

		EMBED_QPOINTER_AND_CAST(QToolBar)
	protected:
		static QToolBar* newRotatableToolBar();
};
#include "RotatableToolBar.h"