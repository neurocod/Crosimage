﻿//RotatableToolBar.h by Kostya Kozachuck as neurocod - 21.11.2011 10:31:44
#pragma once

class RotatableToolBar: public QToolBar {
	Q_OBJECT
	public:
		RotatableToolBar(const QString & title, QWidget * parent = 0);
		RotatableToolBar(QWidget * parent = 0);
	public slots:
		void onOrientationChanged(Qt::Orientation orientation);
	protected:
		void maybeRotate(QWidget*w, Qt::Orientation orientation);
};