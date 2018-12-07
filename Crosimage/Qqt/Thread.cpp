//Thread.cpp by Kostya Kozachuck as neurocod - 16.07.2010 2:00:19
#include "pch.h"
#include "Thread.h"

Thread* Thread::moveToNewThread(QObject*obj) {//static
	Thread* ret = new Thread();
	ret->_lastObj = obj;
	ret->start();
	obj->moveToThread(ret);
	return ret;
}
void Thread::run() {
	QThread::run();
}
bool Thread::isMainThread() {//static
	return QCoreApplication::instance()->thread()==QThread::currentThread();
}
void Thread::assertMainThread() {//static
	ASSERT(isMainThread());
}
void Thread::assertOwnThread(const QObject&_this) {
	assertOwnThread(&_this);
}
void Thread::assertOwnThread(const QObject*obj) {//static
#ifdef _DEBUG
	QThread* t1 = obj->thread();
	QThread* t2 = QThread::currentThread();
	ASSERT(t1 == t2);
#endif
}
bool Thread::isMyThread()const {
	return this==QThread::currentThread();
}