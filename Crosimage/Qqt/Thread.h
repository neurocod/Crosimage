//Thread.h by Kostya Kozachuck as neurocod - 16.07.2010 2:00:18
#pragma once
#include <QThread>
#include <QPointer>

class Thread: public QThread {
	public:
		static Thread* moveToNewThread(QObject*obj);
		static void assertOwnThread(const QObject*_this);
		static void assertOwnThread(const QObject&_this);
		static void assertMainThread();
		static bool isMainThread();
		bool isMyThread()const;

		//for debug purpose:
		virtual void run()override;
	protected:
		//for debug purpose:
		QPointer<QObject> _lastObj;
};