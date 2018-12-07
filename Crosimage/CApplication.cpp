//CApplication.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CApplication.h"
#include "CMainWindow.h"
#include "ThumbCache.h"
#include "ThumbWorker.h"
#include "AltTabView.h"
#include "DirDb.h"
#include "TxtLnkProcessor.h"
#include "ThumbVideoWorker.h"
#include "ThumbWorker.h"
#include "CrSettings.h"

CApplication* CApplication::s_inst = 0;
CApplication::CApplication(int & argc, char ** argv): QApplication(argc, argv) {
	s_inst = this;
	QCoreApplication::setApplicationName("Crosimage");
	QCoreApplication::setOrganizationName("neurocod");
	CrSettings::load();
	//ThumbVideoWorker::thumbFromVideo(ThumbWorker::instance(), "C:\\v\\TSN_1+1.(2014_08)\\Tsn 1+1 (01.08.2014) IPTVRip.avi");
	//return;
	QCommandLineParser parser;
	parser.process(*this);
	auto args = parser.positionalArguments();
	if(!args.isEmpty()) {
		QString str = args[0];
		if(str.toLower().endsWith(TxtLnkProcessor::extensionWithDot)) {
			_saveSettingsOnExist = false;
			TxtLnkProcessor::shellExecute(str);
			QTimer::singleShot(1, this, &CApplication::quit);
			return;
		} else {
			msgBox(tr("Unknown command line parameter: %1").arg(str));
		}
	}
	ThumbWorker::instance();
	ThumbCache::instance();
	{
		Action a(tr("Quit"), QKeySequence("Alt+E"));
		a.shortcutContext = Qt::ApplicationShortcut;
		a.connectClicks(qApp, &QApplication::quit);
		_quitAction = a;
	}
	{
		Action a(tr("Windows manager"), QKeySequence("Ctrl+Tab"));
		_actShowAltTab = a;
		a.connectClicks(this, &CApplication::showAltTab);
	}
	qRegisterMetaType<QDir>("QDir");
	{
		Settings sett;
		if(sett.value("firstLaunch", true).toBool()) {
			sett.setValue("firstLaunch", false);
			msgBox(tr("First launch warning: Crosimage creates files named '%1' in each folder it has to show."
				" These files contains thumbnails, sorting order, slideshow settings etc."
				" This is made to accelerate Crosimage work, better handle situations when folder was renamed or moved,"
				" as inner file stays unchanged and info is not lost.").arg(DirDb::dbFileName));
		}
		//session
		quint32 mainWindowsCount = 0;
		sett.load("mainWindowsCount", mainWindowsCount);
		mainWindowsCount = qBound<quint32>(1, mainWindowsCount, 20);
		QList<QWidget*> li;
		//mainWindowsCount = 1;//to ease debug
		for(int i=0; i<mainWindowsCount; ++i) {
			li << new CMainWindow();
		}
		CMainWindow::s_loadingComplete = true;
		std::reverse(li);
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
	if(!strCmd.isEmpty()) {
		QProcess::startDetached(strCmd);
	}
}
void CApplication::quit() {
	if(_saveSettingsOnExist) {
		Settings sett;
		sett.save("mainWindowsCount", CMainWindow::instancesCount());
		CMainWindow::beforeQuit();
		for(auto w: topLevelWidgets()) {
			w->close();
		}
	}
	__super::quit();
}
QList<QAction*> CApplication::globalActions()const {
	QList<QAction*> ret;
	ret << _quitAction;
	ret << _actShowAltTab;
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
	QProcess::startDetached("C:\\Program Files\\Paint.NET\\PaintDotNet.exe", QStringList() << s);
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