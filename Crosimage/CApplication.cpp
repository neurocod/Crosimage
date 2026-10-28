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
#include "Screenshot.h"
#include "FileSystemThread.h"
#include <cstdio>
#include <cstdlib>

CApplication* CApplication::s_inst = 0;
CApplication::CApplication(int & argc, char ** argv): QApplication(argc, argv) {
	s_inst = this;
	QCoreApplication::setApplicationName("Crosimage");
	QCoreApplication::setOrganizationName("neurocod");
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addOption({"screenshot", "Capture one window and exit (isolated settings)."});
	parser.addOption({"screenshot-delay", "Delay before capture in milliseconds.", "ms", "2000"});
	parser.addOption({"screenshot-dir", "Screenshot output directory.", "path",
		QString::fromUtf8(CROSIMAGE_PROJECT_ROOT) + "/screens"});
	parser.addOption({"folder", "Folder to show in screenshot mode.", "path"});
	parser.addOption({"manual-run", "Keep the screenshot window open after capture."});
	auto commandLineError = [](const QString &message) {
		fprintf(stderr, "%s\n", qPrintable(message));
		std::exit(EXIT_FAILURE);
	};
	if(!parser.parse(arguments()))
		commandLineError(parser.errorText());
	if(parser.isSet("help")) {
		fprintf(stdout, "%s", qPrintable(parser.helpText()));
		std::exit(EXIT_SUCCESS);
	}
	const bool capture = parser.isSet("screenshot");
	if(!capture && (parser.isSet("folder") || parser.isSet("screenshot-delay")
		|| parser.isSet("screenshot-dir") || parser.isSet("manual-run")))
		commandLineError("Screenshot options require --screenshot");
	bool validDelay = false;
	const int screenshotDelay = parser.value("screenshot-delay").toInt(&validDelay);
	if(!validDelay || screenshotDelay < 0)
		commandLineError("--screenshot-delay must be a nonnegative integer");
	QString initialDirectory;
	if(parser.isSet("folder")) {
		const QFileInfo folder(parser.value("folder"));
		if(parser.value("folder").isEmpty() || !folder.isDir())
			commandLineError("--folder must name an existing directory");
		initialDirectory = folder.absoluteFilePath();
	}
	if(capture) {
		if(!parser.positionalArguments().isEmpty())
			commandLineError("Use --folder in screenshot mode, not positional arguments");
		if(parser.value("screenshot-dir").trimmed().isEmpty())
			commandLineError("--screenshot-dir must not be empty");
		_screenshotSettings.reset(new QTemporaryDir());
		if(!_screenshotSettings->isValid())
			commandLineError("Cannot create temporary screenshot settings");
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, _screenshotSettings->path());
		QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, _screenshotSettings->path());
		_saveSettingsOnExit = false;
	}
	CrSettings::load();
#ifdef _DEBUG
	// Keep the session group fixed until restart, even if Settings changes.
	_debugSessions = CrSettings::inst()._debugSessions;
#endif
	//ThumbVideoWorker::thumbFromVideo(ThumbWorker::instance(), "C:\\v\\TSN_1+1.(2014_08)\\Tsn 1+1 (01.08.2014) IPTVRip.avi");
	//return;
	auto args = parser.positionalArguments();
	if(!args.isEmpty()) {
		QString str = args[0];
		
		if(TxtLnkProcessor::seemsMyFile(str)) {
			_saveSettingsOnExit = false;
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
		a.connectClicks(this, &CApplication::quit);
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
		if(!capture && sett.value("firstLaunch", true).toBool()) {
			sett.setValue("firstLaunch", false);
			msgBox(tr("First launch warning: Crosimage creates files named '%1' in each folder it has to show."
				" These files contains thumbnails, sorting order, slideshow settings etc."
				" This is made to accelerate Crosimage work, better handle situations when folder was renamed or moved,"
				" as inner file stays unchanged and info is not lost.").arg(DirDb::dbFileName));
		}
		//session
		beginSessionSettingsGroup(sett);
		quint32 mainWindowsCount = 0;
		sett.load("mainWindowsCount", mainWindowsCount);
		mainWindowsCount = qBound<quint32>(1, mainWindowsCount, 20);
		QList<QWidget*> li;
		//mainWindowsCount = 1;//to ease debug
		for(int i=0; i<mainWindowsCount; ++i) {
			li << new CMainWindow(nullptr, {}, initialDirectory);
		}
		CMainWindow::s_loadingComplete = true;
		std::reverse(li);
		for(auto w: li) {
			setActiveWindow(w);
			processEvents();
			if(_atExit)
				return;
		}
		if(capture) {
			QPointer<QWidget> target = li.first();
			const QString directory = QDir(parser.value("screenshot-dir")).absolutePath();
			const bool manualRun = parser.isSet("manual-run");
			QTimer::singleShot(screenshotDelay, this, [target, directory, manualRun]() {
				QString error;
				const bool saved = Screenshot::save(target, directory, &error);
				if(!saved)
					fprintf(stderr, "Screenshot failed: %s\n", qPrintable(error));
				else
					fprintf(stdout, "Screenshot: %s/latest.png\n", qPrintable(directory));
				if(!saved || !manualRun)
					QCoreApplication::exit(saved ? EXIT_SUCCESS : EXIT_FAILURE);
			});
		}
	}
	New<QTimer> timer(this);
	connect(timer, SIGNAL(timeout()), SLOT(prioritizeThumbs()) );
	timer->start(500);
}
CApplication::~CApplication() {
	// Finish queued sorting writes while QApplication is still alive. Stopping
	// the filesystem event loop directly could discard them after a slow read.
	QMetaObject::invokeMethod(&FileSystemThread::instance(), []() {}, Qt::BlockingQueuedConnection);
	ThumbWorker::instance().exit();
	if(screenshotMode())
		return;
	Settings sett;
	QString strCmd;
	sett.load("shutdownCommand", strCmd);
	if(!strCmd.isEmpty()) {
		QProcess::startDetached(strCmd);
	}
}
void CApplication::quit() {
	_atExit = true;
	if(_saveSettingsOnExit) {
		Settings sett;
		beginSessionSettingsGroup(sett);
		sett.save("mainWindowsCount", CMainWindow::instancesCount());
		CMainWindow::beforeQuit();
		for(auto w: topLevelWidgets()) {
			w->close();
		}
	}
	__super::quit();
}
void CApplication::beginSessionSettingsGroup(QSettings & settings)const {
	if(_debugSessions)
		settings.beginGroup("debug");
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
