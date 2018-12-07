//WidgetAction.h by Kostya Kozachuck as neurocod - 2014.09.21 14:22:40
#pragma once

/* usage example:
	auto row = new WidgetAction(this, tr("Copy: "));
	*row << _actCopySelected << actCopyCell << actionCopyRow;
	addAction(row);
in contextMenuEvent():
	auto li = WidgetAction::removeActionsContainedByMe(actions());
	QMenu::exec(li, ptGlobal, 0, this);
	*/
class WidgetAction: public QWidgetAction {
	public:
		QString _text;
		Qt::ToolButtonStyle _toolButtonStyle;
		WidgetAction & operator<<(QAction*a);
		WidgetAction & add(QAction*a);
		WidgetAction & add(QAction*a, Qt::ToolButtonStyle style);

		WidgetAction(QObject*parent,
			const QString & text = QString(),
			Qt::ToolButtonStyle toolButtonStyle = Qt::ToolButtonTextBesideIcon);
		virtual ~WidgetAction();
		virtual QWidget* createWidget(QWidget * parent)override;
		static QList<QAction*> removeActionsContainedByMe(const QList<QAction*> & actions);

		bool _checkableActionsCloseMenuToo = false;
	protected:
		static QSet<WidgetAction*> s_inst;
		void addMyActionsToParent();

		struct Pair {
			QAction* _act;
			Qt::ToolButtonStyle _style;
		};
		QList<Pair> _actions;
		struct CreatedWidget;
};