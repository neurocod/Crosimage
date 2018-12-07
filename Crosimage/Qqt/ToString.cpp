//ToString.cpp by Kostya Kozachuck as neurocod - 03.12.2012 21:22:07
#include "pch.h"
#include "ToString.h"

void assertWithFunc(const char *assertion, const char *file, const char *func, int line) {
	QByteArray arr = assertion;
	arr += " in funcion ";
	arr += func;
	qt_assert(arr, file, line);
}
QString rtr(const wchar_t* rus, const char* eng) {
	return QObject::tr(eng);
}
namespace Qqt {
	template<typename T>
	QString toStringT(T t) {
		return QString("%1").arg(t);
	}
	QString toString(decltype(endl)) { return QChar('\n'); }
	QString toString(bool b) { return b ? QStringLiteral("true") : QStringLiteral("false"); }
	QString toString(char n) { return toStringT(n); }
	QString toString(double n) { return toStringT(n); }
	QString toString(qint32 n) { return toStringT(n); }
	QString toString(qint64 n) { return toStringT(n); }
	QString toString(quint32 n) { return toStringT(n); }
	QString toString(quint64 n) { return toStringT(n); }
	QString toString(const char* str) { return QString(str); }
	QString toString(const wchar_t* str) { return QString::fromWCharArray(str); }
	QString toString(const QDateTime & d) { return d.toString(QStringLiteral("yyyy.MM.dd hh:mm:ss")); }
	QString reversed(const QString & str) {
		QString ret;
		for(int i = str.length() - 1; i >= 0; i--)
			ret += str.at(i);
		return ret;
	}
	QString toString(const QSize & sz) {
		QString ret = QStringLiteral("QSize(%1, %2)").arg(sz.width()).arg(sz.height());
		return ret;
	}
	QString toString(const QSizeF & sz) {
		QString ret = QStringLiteral("QSizeF(%1, %2)").arg(sz.width()).arg(sz.height());
		return ret;
	}
	QString toString(const QRect & r) {
		QString str = QStringLiteral("QRect(%1, %2, %3, %4)")
			.arg(r.left())
			.arg(r.top())
			.arg(r.width())
			.arg(r.height());
		return str;
	}
	QString toString(const QRectF & r) {
		QString str = QStringLiteral("QRectF(%1, %2, %3, %4)")
			.arg(r.left())
			.arg(r.top())
			.arg(r.width())
			.arg(r.height());
		return str;
	}
	QString toString(const QPoint & p) {
		return QStringLiteral("QPoint(%1, %2)").arg(p.x()).arg(p.y());
	}
	QString toString(const QPointF & p) {
		return QStringLiteral("QPointF(%1, %2)").arg(p.x()).arg(p.y());
	}
	QString toString(const QString & str) {
		return str;
	}
	QByteArray toByteArray(const QString & str) {
		QByteArray ret;
		ret.append(str);
		return ret;
	}
	QString toString(const QColor & clr) {
		if(!clr.isValid())
			return "#invalid";
		QString ret = QStringLiteral("#%1%2%3")
			.arg(clr.red(), 2, 16, QChar('0'))
			.arg(clr.green(), 2, 16, QChar('0'))
			.arg(clr.blue(), 2, 16, QChar('0'));
		return ret;
	}
	QString toString(const QJsonValue & v) {
		switch(v.type()) {
			case QJsonValue::Null: return QStringLiteral("null");
			case QJsonValue::Bool: return toString(v.toBool());
			case QJsonValue::Double: return toString(v.toDouble());
			case QJsonValue::String: return v.toString();
			case QJsonValue::Array: {
				QJsonDocument doc(v.toArray());
				return doc.toJson();
			}
			case QJsonValue::Object:
			{
				QJsonDocument doc(v.toObject());
				return doc.toJson();
			}
		}
		return QString();
	}
	QString toString(QEvent::Type t) {
#define CASE_RETURN(x) case x: return #x;
		switch(t) {
			CASE_RETURN(QEvent::None)
				CASE_RETURN(QEvent::ActionAdded)
				CASE_RETURN(QEvent::ActionChanged)
				CASE_RETURN(QEvent::ActionRemoved)
				CASE_RETURN(QEvent::ActivationChange)
				CASE_RETURN(QEvent::ApplicationActivate)
				CASE_RETURN(QEvent::ApplicationDeactivate)
				CASE_RETURN(QEvent::ApplicationFontChange)
				CASE_RETURN(QEvent::ApplicationLayoutDirectionChange)
				CASE_RETURN(QEvent::ApplicationPaletteChange)
				CASE_RETURN(QEvent::ApplicationStateChange)
				CASE_RETURN(QEvent::ApplicationWindowIconChange)
				CASE_RETURN(QEvent::ChildAdded)
				CASE_RETURN(QEvent::ChildPolished)
				CASE_RETURN(QEvent::ChildRemoved)
				CASE_RETURN(QEvent::Clipboard)
				CASE_RETURN(QEvent::Close)
				CASE_RETURN(QEvent::CloseSoftwareInputPanel)
				CASE_RETURN(QEvent::ContentsRectChange)
				CASE_RETURN(QEvent::ContextMenu)
				CASE_RETURN(QEvent::CursorChange)
				CASE_RETURN(QEvent::DeferredDelete)
				CASE_RETURN(QEvent::DragEnter)
				CASE_RETURN(QEvent::DragLeave)
				CASE_RETURN(QEvent::DragMove)
				CASE_RETURN(QEvent::Drop)
				CASE_RETURN(QEvent::DynamicPropertyChange)
				CASE_RETURN(QEvent::EnabledChange)
				CASE_RETURN(QEvent::Enter)
#ifdef QT_KEYPAD_NAVIGATION
				CASE_RETURN(QEvent::EnterEditFocus)
#endif
				CASE_RETURN(QEvent::EnterWhatsThisMode)
				CASE_RETURN(QEvent::Expose)
				CASE_RETURN(QEvent::FileOpen)
				CASE_RETURN(QEvent::FocusIn)
				CASE_RETURN(QEvent::FocusOut)
				CASE_RETURN(QEvent::FocusAboutToChange)
				CASE_RETURN(QEvent::FontChange)
				CASE_RETURN(QEvent::Gesture)
				CASE_RETURN(QEvent::GestureOverride)
				CASE_RETURN(QEvent::GrabKeyboard)
				CASE_RETURN(QEvent::GrabMouse)
				CASE_RETURN(QEvent::GraphicsSceneContextMenu)
				CASE_RETURN(QEvent::GraphicsSceneDragEnter)
				CASE_RETURN(QEvent::GraphicsSceneDragLeave)
				CASE_RETURN(QEvent::GraphicsSceneDragMove)
				CASE_RETURN(QEvent::GraphicsSceneDrop)
				CASE_RETURN(QEvent::GraphicsSceneHelp)
				CASE_RETURN(QEvent::GraphicsSceneHoverEnter)
				CASE_RETURN(QEvent::GraphicsSceneHoverLeave)
				CASE_RETURN(QEvent::GraphicsSceneHoverMove)
				CASE_RETURN(QEvent::GraphicsSceneMouseDoubleClick)
				CASE_RETURN(QEvent::GraphicsSceneMouseMove)
				CASE_RETURN(QEvent::GraphicsSceneMousePress)
				CASE_RETURN(QEvent::GraphicsSceneMouseRelease)
				CASE_RETURN(QEvent::GraphicsSceneMove)
				CASE_RETURN(QEvent::GraphicsSceneResize)
				CASE_RETURN(QEvent::GraphicsSceneWheel)
				CASE_RETURN(QEvent::Hide)
				CASE_RETURN(QEvent::HideToParent)
				CASE_RETURN(QEvent::HoverEnter)
				CASE_RETURN(QEvent::HoverLeave)
				CASE_RETURN(QEvent::HoverMove)
				CASE_RETURN(QEvent::IconDrag)
				CASE_RETURN(QEvent::IconTextChange)
				CASE_RETURN(QEvent::InputMethod)
				CASE_RETURN(QEvent::InputMethodQuery)
				CASE_RETURN(QEvent::KeyboardLayoutChange)
				CASE_RETURN(QEvent::KeyPress)
				CASE_RETURN(QEvent::KeyRelease)
				CASE_RETURN(QEvent::LanguageChange)
				CASE_RETURN(QEvent::LayoutDirectionChange)
				CASE_RETURN(QEvent::LayoutRequest)
				CASE_RETURN(QEvent::Leave)
				CASE_RETURN(QEvent::LeaveWhatsThisMode)
				CASE_RETURN(QEvent::LocaleChange)
				CASE_RETURN(QEvent::NonClientAreaMouseButtonDblClick)
				CASE_RETURN(QEvent::NonClientAreaMouseButtonPress)
				CASE_RETURN(QEvent::NonClientAreaMouseButtonRelease)
				CASE_RETURN(QEvent::NonClientAreaMouseMove)
				CASE_RETURN(QEvent::MacSizeChange)
				CASE_RETURN(QEvent::MetaCall)
				CASE_RETURN(QEvent::ModifiedChange)
				CASE_RETURN(QEvent::MouseButtonDblClick)
				CASE_RETURN(QEvent::MouseButtonPress)
				CASE_RETURN(QEvent::MouseButtonRelease)
				CASE_RETURN(QEvent::MouseMove)
				CASE_RETURN(QEvent::MouseTrackingChange)
				CASE_RETURN(QEvent::Move)
				CASE_RETURN(QEvent::OrientationChange)
				CASE_RETURN(QEvent::Paint)
				CASE_RETURN(QEvent::PaletteChange)
				CASE_RETURN(QEvent::ParentAboutToChange)
				CASE_RETURN(QEvent::ParentChange)
				CASE_RETURN(QEvent::PlatformPanel)
				CASE_RETURN(QEvent::Polish)
				CASE_RETURN(QEvent::PolishRequest)
				CASE_RETURN(QEvent::QueryWhatsThis)
				CASE_RETURN(QEvent::RequestSoftwareInputPanel)
				CASE_RETURN(QEvent::Resize)
				CASE_RETURN(QEvent::ScrollPrepare)
				CASE_RETURN(QEvent::Scroll)
				CASE_RETURN(QEvent::Shortcut)
				CASE_RETURN(QEvent::ShortcutOverride)
				CASE_RETURN(QEvent::Show)
				CASE_RETURN(QEvent::ShowToParent)
				CASE_RETURN(QEvent::SockAct)
				CASE_RETURN(QEvent::StateMachineSignal)
				CASE_RETURN(QEvent::StateMachineWrapped)
				CASE_RETURN(QEvent::StatusTip)
				CASE_RETURN(QEvent::StyleChange)
				CASE_RETURN(QEvent::TabletMove)
				CASE_RETURN(QEvent::TabletPress)
				CASE_RETURN(QEvent::TabletRelease)
				CASE_RETURN(QEvent::OkRequest)
				CASE_RETURN(QEvent::TabletEnterProximity)
				CASE_RETURN(QEvent::TabletLeaveProximity)
				CASE_RETURN(QEvent::ThreadChange)
				CASE_RETURN(QEvent::Timer)
				CASE_RETURN(QEvent::ToolBarChange)
				CASE_RETURN(QEvent::ToolTip)
				CASE_RETURN(QEvent::ToolTipChange)
				CASE_RETURN(QEvent::TouchBegin)
				CASE_RETURN(QEvent::TouchCancel)
				CASE_RETURN(QEvent::TouchEnd)
				CASE_RETURN(QEvent::TouchUpdate)
				CASE_RETURN(QEvent::UngrabKeyboard)
				CASE_RETURN(QEvent::UngrabMouse)
				CASE_RETURN(QEvent::UpdateLater)
				CASE_RETURN(QEvent::UpdateRequest)
				CASE_RETURN(QEvent::WhatsThis)
				CASE_RETURN(QEvent::WhatsThisClicked)
				CASE_RETURN(QEvent::Wheel)
				CASE_RETURN(QEvent::WinEventAct)
				CASE_RETURN(QEvent::WindowActivate)
				CASE_RETURN(QEvent::WindowBlocked)
				CASE_RETURN(QEvent::WindowDeactivate)
				CASE_RETURN(QEvent::WindowIconChange)
				CASE_RETURN(QEvent::WindowStateChange)
				CASE_RETURN(QEvent::WindowTitleChange)
				CASE_RETURN(QEvent::WindowUnblocked)
				CASE_RETURN(QEvent::WinIdChange)
				CASE_RETURN(QEvent::ZOrderChange)
		}
		return Qqt::toString((int) t);
	}
}//namespace