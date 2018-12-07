//Action.h by Kostya Kozachuck as neurocod - 11.11.2011 22:08:22
#pragma once

class Action {
	public:
		static void insertToBegin(QWidget*w, QAction*act);
		static void insertToBegin(QWidget*w, const QList<QAction*> & lact);
		template<typename... Args>
		Action(Args...args) {
			d = new QAction(0);
			toolTip.init(d);
			autoRepeat.init(d);
			checkable.init(d);
			checked.init(d);
			enabled.init(d);
			iconVisibleInMenu.init(d);
			visible.init(d);
			menuRole.init(d);
			priority.init(d);
			shortcutContext.init(d);
			shortcut.init(d);
			shortcuts.init(d);
			font.init(d);
			icon.init(d);
			iconText.init(d);
			statusTip.init(d);
			text.init(d);
			whatsThis.init(d);
			separator.init(d);

			CtorProcessor p(*this);
			p.process_(args...);
		}
		virtual ~Action() {}
		void connectClicksS(QObject*obj, const char* slot, Qt::ConnectionType conn=Qt::AutoConnection);
		template<class T1, class T2>
		void connectClicks(T1*receiver, const T2 slot, Qt::ConnectionType type = Qt::AutoConnection) {
			QAction* btn = icon.destination();
			QObject::connect(btn, &QAction::triggered, receiver, slot, type);
		}

		PROPERTY_REDIRECTV(QAction, bool, autoRepeat, autoRepeat, setAutoRepeat);
		PROPERTY_REDIRECTV(QAction, bool, checkable, isCheckable, setCheckable);
		PROPERTY_REDIRECTV(QAction, bool, checked, isChecked, setChecked);
		PROPERTY_REDIRECTV(QAction, bool, enabled, isEnabled, setEnabled);
		PROPERTY_REDIRECTV(QAction, bool, iconVisibleInMenu, isIconVisibleInMenu, setIconVisibleInMenu);
		PROPERTY_REDIRECTV(QAction, bool, visible, isVisible, setVisible);
		PROPERTY_REDIRECTV(QAction, bool, separator, isSeparator, setSeparator);
		PROPERTY_REDIRECTV(QAction, QAction::MenuRole, menuRole, menuRole, setMenuRole);
		PROPERTY_REDIRECTV(QAction, QAction::Priority, priority, priority, setPriority);
		PROPERTY_REDIRECTV(QAction, Qt::ShortcutContext, shortcutContext, shortcutContext, setShortcutContext);
		PROPERTY_REDIRECT(QAction, QKeySequence, shortcut, shortcut, setShortcut);
		PROPERTY_REDIRECT(QAction, QList<QKeySequence>, shortcuts, shortcuts, setShortcuts);
		PROPERTY_REDIRECT (QAction, QFont, font, font, setFont);
		PROPERTY_REDIRECT (QAction, QIcon, icon, icon, setIcon);
		PROPERTY_REDIRECT (QAction, QString, iconText, iconText, setIconText);
		PROPERTY_REDIRECT (QAction, QString, statusTip, statusTip, setStatusTip);
		PROPERTY_REDIRECT (QAction, QString, text, text, setText);
		PROPERTY_REDIRECT (QAction, QString, toolTip, toolTip, setToolTip);
		PROPERTY_REDIRECT (QAction, QString, whatsThis, whatsThis, setWhatsThis);

		enum ActionFlags {
			SetShortcutToTooltip = 1<<0,//This parameter must be somewhere after shorctuct parameter
			SetCheckable = 1<<1,
		};

		void set(const QIcon & i) { d->setIcon(i); }
		template<typename... Args>
		void set(Args...args) {
			CtorProcessor p(*this);
			p.process_(args...);
		}

		struct CtorProcessor: public CtorProcessorT<Action> {
			CtorProcessor(Action&d): CtorProcessorT(d) { }
			using CtorProcessorT::process;
			void process(ActionFlags t) {
				if(t & SetShortcutToTooltip) {
					QKeySequence seq = _d->shortcut();
					if(seq.isEmpty())
						return;
					QString str = QObject::tr("Shortcut: %1").arg(seq.toString());
					_d->setToolTip(str);
				}
				if(t & SetCheckable) {
					_d->setCheckable(true);
				}
			}
			void process_() { }
			template<class Arg, typename ... Args>
			void process_(Arg arg, Args...args) {
				process(arg);
				process_(args...);
			}
		};
		EMBED_QPOINTER_AND_CAST(QAction);
};