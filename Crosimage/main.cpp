//main.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CMainWindow.h"
#include "CApplication.h"
#include <cstdio>
#include <cstring>

QtMessageHandler g_oldQtMessageHandler = {};
bool g_screenshotMode = false;
void myMsgHandler(QtMsgType type, const QMessageLogContext & context, const QString &msg) {
	const bool prevent = msg.contains(QStringLiteral("Corrupt JPEG data"))
		|| msg.contains(QStringLiteral("known incorrect sRGB profile"))
		|| msg.contains(QStringLiteral("Unsupported ICC profile"))
		|| msg.contains(QStringLiteral("fromIccProfile: failed general sanity check"))
		|| msg.contains(QStringLiteral("failed minimal tag size sanity"))
		|| msg.contains(QStringLiteral("invalid tag offset alignment"))
		|| msg.contains(QStringLiteral("ShGetFileInfoBackground() timed out for "));
	if(g_oldQtMessageHandler)
		g_oldQtMessageHandler(type, context, msg);
	if(g_screenshotMode || !QCoreApplication::instance()) {
		fprintf(stderr, "%s\n", qPrintable(msg));
		return;
	}
	QString str = [type]()->QString {
		switch (type) {
		case QtDebugMsg: return "Qt Debug";
		case QtWarningMsg: return "Qt Warning";
		case QtCriticalMsg: return "Qt Critical";
		case QtFatalMsg: return "Qt Fatal";
		}
		return "unknown";
	} ();
	str += QStringLiteral(" function %1 file %2 line %3: %4").arg(context.function).arg(context.file).arg(context.line).arg(msg);
	//LogFile::debug() << QDateTime::currentDateTime() << str << Qt::endl;
	if(!prevent) {
		if(QCoreApplication::instance()->thread() == QThread::currentThread()) {
			msgBox("Qt message", str);
		} else {
			//TODO send to main thread
			//trace(str);
		}
	}
	//if(QtFatalMsg==type)
	//     abort();
}
int main(int argc, char *argv[]) {
	// QApplication itself can warn (for example, about an unavailable style).
	// Detect automation before constructing it so those warnings cannot block.
	for(int i = 1; i < argc && std::strcmp(argv[i], "--") != 0; ++i) {
		if(std::strcmp(argv[i], "--screenshot") == 0 || std::strcmp(argv[i], "-screenshot") == 0)
			g_screenshotMode = true;
	}
	g_oldQtMessageHandler = qInstallMessageHandler(myMsgHandler);
	// This attribute must be set before QGuiApplication is constructed:
	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
	QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );
	CApplication a(argc, argv);
	return a.exec();
}
