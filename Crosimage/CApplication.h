﻿//CApplication.h by Kostya Kozachuck as neurocod
#pragma once

class CApplication: public QApplication {
	Q_OBJECT
	public:
		CApplication(int & argc, char ** argv);
		virtual ~CApplication();
		static CApplication* s_inst;
		QList<QAction*> globalActions()const;
		static void viewExternally(const QString & s);
		static void editExternally(const QString & s);
	public slots:
		void quit();
		void showAltTab();
		void prioritizeThumbs();
	protected:
		QAction* _quitAction;
		QAction* _actShowAltTab;
};