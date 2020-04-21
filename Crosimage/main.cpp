//main.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CMainWindow.h"
#include "CApplication.h"

QtMessageHandler g_oldQtMessageHandler = {};
void myMsgHandler(QtMsgType type, const QMessageLogContext & context, const QString &msg) {
	bool prevent = msg.contains(QStringLiteral("Corrupt JPEG data"))
		|| msg.contains(QStringLiteral("known incorrect sRGB profile"))
		|| msg.contains(QStringLiteral("Unsupported ICC profile"))
		|| msg.contains(QStringLiteral("fromIccProfile: failed general sanity check"))
		|| msg.contains(QStringLiteral("failed minimal tag size sanity"))
		|| msg.contains(QStringLiteral("ShGetFileInfoBackground() timed out for "));
	g_oldQtMessageHandler(type, context, msg);
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
	//LogFile::debug() << QDateTime::currentDateTime() << str << endl;
	if(!prevent) {
		if(QCoreApplication::instance()->thread() == QThread::currentThread()) {
			msgBox("Qt message", str);
		} else {
			//TODO: send to main thread
			//trace(str);
		}
	}
	//if(QtFatalMsg==type)
	//     abort();
}
int main(int argc, char *argv[]) {
	g_oldQtMessageHandler = qInstallMessageHandler(myMsgHandler);
	// This attribute must be set before QGuiApplication is constructed:
	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
	QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );
#ifdef Q_OS_WIN
	QCoreApplication::setAttribute( Qt::AA_DisableWindowContextHelpButton );
#endif
	CApplication a(argc, argv);
	return a.exec();
}