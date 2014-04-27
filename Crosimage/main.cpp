//main.cpp by Kostya Kozachuck as neurocod - 29.03.2013 8:15:33
#include "pch.h"
#include "CMainWindow.h"
#include "CApplication.h"

QtMessageHandler g_oldQtMessageHandler;
void myMsgHandler(QtMsgType type, const QMessageLogContext & context, const QString &msg) {
	bool prevent = msg.contains("Corrupt JPEG data");
	g_oldQtMessageHandler(type, context, msg);
	QString str;
	switch (type) {
		case QtDebugMsg: str = "Qt Debug"; break;
		case QtWarningMsg: str = "Qt Warning"; break;
		case QtCriticalMsg: str = "Qt Critical"; break;
		case QtFatalMsg: str = "Qt Fatal"; break;
	}
	str += QString(" function %1 file %2 line %3: %4").arg(context.function).arg(context.file).arg(context.line).arg(msg);
	//LogFile::debug() << QDateTime::currentDateTime() << str << endl;
	if(!prevent) {
		if(QCoreApplication::instance()->thread() == QThread::currentThread()) {
			msgBox("Qt message", str);
		} else {
			trace(str);
		}
	}
	//if(QtFatalMsg==type)
    //     abort();
}
int main(int argc, char *argv[]) {
	g_oldQtMessageHandler = qInstallMessageHandler(myMsgHandler);
	CApplication a(argc, argv);
	return a.exec();
}