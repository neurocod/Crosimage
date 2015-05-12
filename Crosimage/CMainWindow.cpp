//CMainWindow.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "CMainWindow.h"
#include "ThumbView.h"
#include "ThumbModel.h"
#include "ThumbWorker.h"
#include "ImageView.h"
#include "CApplication.h"
#include "SettingsDialog.h"

QList<CMainWindow*> CMainWindow::s_inst;
bool CMainWindow::s_loadingComplete = false;
class CComboBox: public QComboBox {
	public:
		virtual void enterEvent(QEvent * event)override {
			setFocus(Qt::MouseFocusReason);
		}
};
CMainWindow::CMainWindow(QWidget *parent, Qt::WindowFlags flags): QMainWindow(parent, flags) {
	setAttribute(Qt::WA_DeleteOnClose);
	_model = new ThumbModel(this);
	_view = new ThumbView(_model);
	_nInst = s_inst.count();
	s_inst << this;
	setWindowTitle();
	Widget w;
	VBoxLayout00 lay(w);
	{
		HBoxLayout00 lay2(lay);

		{
			ToolButton label;
			auto f = font();
			f.setPixelSize(20);
			f.setPointSize(20);
			f.setPointSizeF(20);
			label.font = f;
			label.text = tr("wnd %1").arg(_nInst+1);
			lay2 << label;
			QTimer::singleShot(1000, label, SLOT(deleteLater()));
		}
		QSize szBtn(24, 24);

		lay2 << _menu;

		ToolButton btnGoUp("", tr("Parent dir"), QIcon(":/qt-project.org/styles/commonstyle/images/up-32.png"), QKeySequence("Alt+Up"));
		btnGoUp.addShortcutToTooltip();
		btnGoUp.iconSize = szBtn;
		btnGoUp.connectClicks(this, SLOT(goUp()));
		lay2 << btnGoUp;

		ToolButton btnGoBack("", tr("Go back"), QKeySequence::Back, QIcon(":/qt-project.org/styles/commonstyle/images/left-32.png"));
		btnGoBack.addShortcutToTooltip();
		btnGoBack.iconSize = szBtn;
		btnGoBack.connectClicks(this, SLOT(goBack()));
		lay2 << btnGoBack;

		ToolButton btnGoFwd("", tr("Go forward"), QKeySequence::Forward, QIcon(":/qt-project.org/styles/commonstyle/images/right-32.png"));
		btnGoFwd.addShortcutToTooltip();
		btnGoFwd.iconSize = szBtn;
		btnGoFwd.connectClicks(this, SLOT(goFwd()));
		lay2 << btnGoFwd;

		ToolButton btnRefresh("", tr("Refresh"), QKeySequence("F5"), QIcon(":/qt-project.org/styles/commonstyle/images/refresh-32.png"));
		btnRefresh.addShortcutToTooltip();
		btnRefresh.iconSize = szBtn;
		btnRefresh.connectClicks(_model, SLOT(refresh()));
		lay2 << btnRefresh;
		{
			ToolButton b("", tr("Show prev sibling directory"), QKeySequence("Ctrl+Up"), QIcon(":/images/folder-up-24.png"));
			b.addShortcutToTooltip();
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(goUpSibling()));
			lay2 << b;
		}
		{
			ToolButton b("", tr("Show next sibling directory"), QKeySequence("Ctrl+Down"), QIcon(":/images/folder-down-24.png"));
			b.addShortcutToTooltip();
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(goDownSibling()));
			lay2 << b;
		}
		{
			ToolButton b("", tr("Show in Explorer"), QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-open-32.png"));
			b.addShortcutToTooltip();
			b.iconSize = szBtn;
			b.connectClicks(this, SLOT(openInExplorer()));
			lay2 << b;
		}
		_boxSortBy.toolTip = tr("Sort by ...");
		_boxSortBy->addItem(tr("Time"), QVariant(QDir::Time));
		_boxSortBy->addItem(tr("Name"), QVariant(QDir::Name));
		_boxSortBy->addItem(tr("Size"), QVariant(QDir::Size));
		_boxSortBy->addItem(tr("Type"), QVariant(QDir::Type));
		_boxSortBy.sizeAdjustPolicy = QComboBox::AdjustToContents;
		_boxSortBy->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		connect(_boxSortBy, SIGNAL(currentIndexChanged(int)), SLOT(sortingUpdated()) );
		lay2 << _boxSortBy;

		_checkSortReversed.text = tr("Reversed");
		_checkSortReversed.sizePolicy = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		_checkSortReversed.connectClicks(this, SLOT(sortingUpdated()));
		lay2 << _checkSortReversed;

		_editPath = new CComboBox();
		_editPath->setEditable(1);
		_editPath->setMaxVisibleItems(30);
		{
			QCompleter *completer = new QCompleter(this);
			auto dirModel = new QDirModel(completer);
			dirModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
			completer->setModel(dirModel);
			_editPath->setCompleter(completer);
		}
		lay2 << _editPath;
		connect(_editPath, SIGNAL(currentTextChanged(const QString &)), SLOT(go(QString)) );
	}
	lay << _view;
	setCentralWidget(w);
	//createFileTreePanel();
	showFullScreen();
	updateSettings(false);
	{
		Action a(tr("New window"), QKeySequence("Ctrl+N"), QIcon(":/qt-project.org/styles/commonstyle/images/file-16.png"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				new CMainWindow;
			});
		addAction(a);
	}
	{
		Action a(tr("Toggle fullscreen"), QKeySequence("F11"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				if(windowState()==Qt::WindowFullScreen)
					showMaximized();
				else
					showFullScreen();
			});
		addAction(a);
	}
	{
		Action a(tr("Settings..."), QIcon(":/qt-project.org/styles/commonstyle/images/computer-16.png"));
		QObject::connect(a, &QAction::triggered, this, [=]() {
				SettingsDialog::createAndShow(this);
			});
		addAction(a);
	}
	addActions(CApplication::s_inst->globalActions());
	_menu << actions();
}
CMainWindow::~CMainWindow() {
	bool b = s_inst.removeOne(this);
	ASSERT(b);
	updateSettings(true);
}
void CMainWindow::updateSettings(bool save) {
	auto li = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	if(!li.isEmpty())
		_rootDir = li[0];
	Settings sett;
	sett.beginGroup("MainWindow");
	sett.beginGroup(toString(_nInst));
	if(save) {
		QString path = _model->dir().absolutePath();
		sett.save("dir", path);
	} else {
		QString path = _rootDir;
		sett.load("dir", path);
		go(path);
	}
}
void CMainWindow::createFileTreePanel() {
	QFileSystemModel* dirModel = new QFileSystemModel(this);
	dirModel->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);//QDir::AllDirs QDir::Drives
	//setIconProvider QFileIconProvider
	dirModel->setRootPath(QDir::separator());

	QTreeView* dirView = new QTreeView(this);
	dirView->setModel(dirModel);
	for(int i=1; i < dirModel->columnCount(); ++i) { //hide unnecessary columns
		dirView->hideColumn(i);
	}
	QDockWidget* dock = new QDockWidget(tr("File tree"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->setWidget(dirView);
}
void CMainWindow::goUp() {
	QDir dir(_model->dir());
	if(!dir.cdUp())
		return;
	go(dir.absolutePath());
}
void CMainWindow::goBack() {
	if(_navigateBack.isEmpty())
		return;
	QString str = _navigateBack.last();;
	_navigateBack.removeLast();
	go(str, 1);
}
void CMainWindow::goFwd() {
	if(_navigateFwd.isEmpty())
		return;
	QString str = _navigateFwd.first();;
	_navigateFwd.removeFirst();
	go(str, -1);
}
void CMainWindow::go(const QString & _path, int addCurrentToHistory) {
	QFileInfo file(_path);
	if(!file.exists())
		return;
	QString path = QDir::toNativeSeparators(file.canonicalFilePath());
	if(path==_rootDir && !path.endsWith('/') && !path.endsWith('\\'))
		path += '\\';
	if(file.isDir()) {
		const QString oldDir = _model->dir().absolutePath();
		if(oldDir==file.absoluteFilePath())
			return;
		if(!oldDir.isEmpty()) {
			if(addCurrentToHistory<0)
				_navigateBack << oldDir;
			else if(addCurrentToHistory>0)
				_navigateFwd.prepend(oldDir);
		}
		_view->selectLater(oldDir);
		_model->setDir(path);
		setWindowTitle();
		if(needSetTextToLineEdit(path))
			_editPath->setCurrentText(path);
	} else {
		display(path);
	}
}
void CMainWindow::go(const QString & path) {
	go(path, -1);
}
void CMainWindow::display(const QString & file) {
	QImage im;
	if(QImageReader(file).read(&im)) {
		new ImageView(_model, _view, file);
	} else {
		QDesktopServices::openUrl(QUrl::fromLocalFile(file));
	}
}
CMainWindow* CMainWindow::instance(QWidget*child) {
	for(auto p = child; p; p = p->parentWidget()) {
		if(auto ret = qobject_cast<CMainWindow*>(p))
			return ret;
	}
	return 0;
}
void CMainWindow::goUpSibling() {
	goSibling(false);
}
void CMainWindow::goDownSibling() {
	goSibling(true);
}
bool compareByNameAndNumber(const QString & s1, const QString & s2) {
	static QCollator collator;
	collator.setNumericMode(true);
	return collator.compare(s1, s2)<=0;
}
void CMainWindow::goSibling(bool next) {
	QDir parentDir = _model->dir().absoluteFilePath("..");
	QStringList siblings = parentDir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
	if(siblings.isEmpty()) {
		ASSERT(0);
		return;
	}
	qSort(siblings.begin(), siblings.end(), compareByNameAndNumber);
	Qt::CaseSensitivity cs = Qt::CaseSensitive;
#ifdef Q_OS_WIN
	cs = Qt::CaseInsensitive;
#endif
	const QString curDir = _model->dir().dirName();
	int index = -1;
	for(int i = 0; i<siblings.count(); ++i) {
		if(0==curDir.compare(siblings[i], cs)) {
			index = i;
			break;
		}
	}
	if(-1==index) {
		ASSERT(0);//when this happens? when dir is deleted?
		index = 0;
	}
	index += (next ? 1 : -1);
	index = qBound(0, index, siblings.count()-1);
	QString strNew = parentDir.absoluteFilePath(siblings[index]);
	go(strNew);
}
void CMainWindow::openInExplorer() {
	QString file;
	auto index = _view->selectionModel()->currentIndex();
	if(index.isValid()) {
		file = index.data().toString();
		FileFacility::showDirWithFile(file);
	} else {
		FileFacility::showDirWithFile(_model->dir().absolutePath());
	}
}
bool CMainWindow::needSetTextToLineEdit(QString str)const {
	if(!_editPath->hasFocus())
		return true;
	QString str2 = _editPath->currentText();
	if(str.length()>str2.length())
		qSwap(str, str2);
	if(str+QDir::separator() == str2)
		return false;
	return true;
}
void CMainWindow::focusInEvent(QFocusEvent * event) {
	onFocus();
	__super::focusInEvent(event);
}
void CMainWindow::enterEvent(QEvent *e) {
	__super::enterEvent(e);
	onFocus();
}
void CMainWindow::onFocus() {
	if(!s_loadingComplete)
		return;
	s_inst.removeOne(this);
	s_inst.prepend(this);
}
void CMainWindow::beforeQuit() {
	for(int i=0; i<s_inst.count(); ++i) {
		s_inst[i]->_nInst = i;
	}
}
void CMainWindow::setWindowTitle() {
	QString str = //qApp->applicationName() + " " +
		//toString(_nInst) +
		_model->dir().absolutePath();
	setWindowTitle(str);
	str = toString(_nInst);
	setWindowIconText(str);
}
void CMainWindow::sortingUpdated() {
	int index = _boxSortBy->currentIndex();
	auto v = _boxSortBy->itemData(index);
	QDir::SortFlags sort = (QDir::SortFlag)v.toInt();;
	if(_checkSortReversed.checked)
		sort |= QDir::Reversed;
	_model->setSortFlags(sort);
}
void CMainWindow::prioritizeThumbs() {
	_view->prioritizeThumbs();
}