//CtorProcessorT.h by Kostya Kozachuck as neurocod - 10.03.2012 20:05:49
#pragma once

template<class T>
class CtorProcessorT {
	public:
		int _nStr = 0;
		T & _d;
		CtorProcessorT(T&d): _d(d) {}
		void process_() {}
		template<class Arg, typename ... Args>
		void process_(Arg arg, Args...args) {
			process(arg);
			process_(args...);
		}
		template<class T2>
		void process(QPointer<T2> p) {
			T2* t = p;
			process(t);
		}
		void process() {}
		void process(QObject*parent) {
			ASSERT(_d->parent()==0);
			_d->setParent(parent);
		}
		void process(QWidget*parent) {
			ASSERT(0 == _d->parentWidget());
			_d->setParent(parent);
		}
		void process(const QImage& img) {
			_d.set(img);
		}
		void process(const QIcon& img) {
			_d.set(img);
		}
		void process(const QPixmap& img) {
			_d.set(img);
		}
		void process(Qt::ShortcutContext c) {
			_d->setShortcutContext(c);
		}
		void process(Qt::Orientation ori) {
			_d->setOrientation(ori);
		}
		void process(Qt::ToolButtonStyle style) {
			_d->setToolButtonStyle(style);
		}
		void process(const QKeySequence & c) {
			_d->setShortcut(c);
		}
		void process(const QList<QKeySequence> & c) {
			_d->setShortcuts(c);
		}
		void process(const QString & str) {
			switch(_nStr) {
			case 0:
				_d->setText(str);
				break;
			case 1:
				_d->setToolTip(str);
				break;
			default:
				ASSERT(0);
			}
			_nStr++;
		}
};