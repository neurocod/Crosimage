//CApplication.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CApplication.h"
#include "CMainWindow.h"
#include "ThumbCache.h"
#include "ThumbWorker.h"
#include "AltTabView.h"

CApplication* CApplication::s_inst = 0;
CApplication::CApplication(int & argc, char ** argv): QApplication(argc, argv) {
	s_inst = this;
	QCoreApplication::setApplicationName("Crosimage");
	QCoreApplication::setOrganizationName("neurocod");
	ThumbWorker::instance();
	ThumbCache::instance();
	{
		Action a("Quit", QKeySequence("Alt+E"));
		a.shortcutContext = Qt::ApplicationShortcut;
		a.connectClicks(qApp, SLOT(quit()));
		m_quitAction = a;
	}
	{
		Action a(tr("Switch window"), QKeySequence("Ctrl+Tab"));
		m_actShowAltTab = a;
		a.connectClicks(this, SLOT(showAltTab()));
	}
	qRegisterMetaType<QDir>("QDir");
	{
		//session
		quint32 mainWindowsCount = 0;
		Settings sett;
		sett.load("mainWindowsCount", mainWindowsCount);
		mainWindowsCount = qBound<quint32>(1, mainWindowsCount, 20);
		QList<QWidget*> li;
		for(int i=0; i<mainWindowsCount; ++i) {
			li << New<CMainWindow>();
		}
		CMainWindow::s_loadingComplete = true;
		li = reversed(li);
		for(auto w: li) {
			setActiveWindow(w);
			processEvents();
		}
	}
	New<QTimer> timer(this);
	connect(timer, SIGNAL(timeout()), SLOT(prioritizeThumbs()) );
	timer->start(500);
}
CApplication::~CApplication() {
	ThumbWorker::instance().exit();
	Settings sett;
	QString strCmd;
	sett.load("shutdownCommand", strCmd);
	if(!strCmd.isEmpty())
		QProcess::startDetached(strCmd);
}
void CApplication::quit() {
	Settings sett;
	sett.save("mainWindowsCount", CMainWindow::instancesCount());
	CMainWindow::beforeQuit();
	for(auto w: topLevelWidgets()) {
		w->close();
	}
	__super::quit();
}
QList<QAction*> CApplication::globalActions()const {
	QList<QAction*> ret;
	ret << m_quitAction;
	ret << m_actShowAltTab;
	return ret;
}
void CApplication::showAltTab() {
	AltTabView::showInstance();
}
void CApplication::viewExternally(const QString & s) {
	if(s.isEmpty())
		return;
	QDesktopServices::openUrl(QUrl::fromLocalFile(s));
}
void CApplication::editExternally(const QString & s) {
	if(s.isEmpty())
		return;
	QProcess::startDetached("A:\\Program Files\\Paint.NET\\PaintDotNet.exe", QStringList() << s);
}
void CApplication::prioritizeThumbs() {
	auto w = activeWindow();
	if(!w)
		return;
	auto mw = qobject_cast<CMainWindow*>(w);
	if(!mw)
		return;
	mw->prioritizeThumbs();
}