#include "pch.h"
#include "CApplication.h"
#include "ImageView.h"
#include "ThumbModel.h"
#include "ThumbView.h"
#include <functional>
#include <memory>
#include <stdexcept>
#include <cstdio>

namespace {
void stage(const char *message) {
	fprintf(stderr, "view_regression: %s\n", message);
	fflush(stderr);
}
void require(bool condition, const char *message) {
	if(!condition)
		throw std::runtime_error(message);
}

bool waitUntil(const std::function<bool()> &ready) {
	QElapsedTimer elapsed;
	elapsed.start();
	while(!ready() && elapsed.elapsed()<60000) {
		QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
		QThread::msleep(1);
	}
	return ready();
}

void loadDirectory(ThumbModel &model, const QString &path) {
	stage("loading directory");
	bool loaded = false;
	auto connection = QObject::connect(&model, &QAbstractItemModel::modelReset,
		&model, [&]() { loaded = true; });
	model.setDir(path, false);
	// setDir resets synchronously before submitting the asynchronous read.
	loaded = false;
	const bool finished = waitUntil([&]() { return loaded; });
	QObject::disconnect(connection);
	require(finished, "directory load timed out");
	stage("directory loaded");
}

void saveImage(const QString &path, Qt::GlobalColor color) {
	QImage image(16, 16, QImage::Format_RGB32);
	image.fill(color);
	require(image.save(path), "could not create test image");
}

class InspectableImageView: public ImageView {
public:
	using ImageView::ImageView;
	using ImageView::centerFile;
};

void triggerShortcut(QWidget &widget, const char *shortcut) {
	for(auto action: widget.actions()) {
		if(action->shortcuts().contains(QKeySequence(shortcut))) {
			action->trigger();
			return;
		}
	}
	throw std::runtime_error("shortcut action not found");
}

void checkImageNavigation(const QDir &root) {
	stage("image navigation");
	const auto source = root.filePath("images");
	const auto other = root.filePath("other");
	const auto empty = root.filePath("empty");
	require(root.mkpath("images/subdirectory") && root.mkpath("other")
		&& root.mkpath("empty"), "could not create image test directories");
	saveImage(QDir(source).filePath("1.png"), Qt::red);
	saveImage(QDir(source).filePath("2.png"), Qt::green);
	saveImage(QDir(source).filePath("3.png"), Qt::blue);
	saveImage(QDir(other).filePath("other.png"), Qt::black);

	auto model = std::make_unique<ThumbModel>();
	loadDirectory(*model, source);
	const QStringList snapshot = model->files();
	require(snapshot.size()==3, "unexpected image snapshot");
	InspectableImageView view(model.get(), nullptr, snapshot.first());
	stage("image view constructed");
	view.setAttribute(Qt::WA_DeleteOnClose, false);
	loadDirectory(*model, other);
	triggerShortcut(view, "End");
	require(view.centerFile()==snapshot.last(), "End followed changed model instead of snapshot");
	model.reset();
	triggerShortcut(view, "Home");
	require(view.centerFile()==snapshot.first(), "Home failed after model destruction");
	triggerShortcut(view, "End");
	require(view.centerFile()==snapshot.last(), "End failed after model destruction");
	view.hide();

	ThumbModel emptyModel;
	loadDirectory(emptyModel, empty);
	InspectableImageView emptyView(&emptyModel, nullptr, {});
	emptyView.setAttribute(Qt::WA_DeleteOnClose, false);
	for(const char *shortcut: {"Home", "End", "Left", "Right"})
		triggerShortcut(emptyView, shortcut);
	require(emptyView.centerFile().isEmpty(), "empty navigation selected an image");
	emptyView.hide();
}

void checkMoveAcrossReset(const QDir &root) {
	stage("move across reset");
	const auto source = root.filePath("move-source");
	const auto other = root.filePath("move-other");
	const auto destination = root.filePath("move-destination");
	require(root.mkpath("move-source") && root.mkpath("move-other")
		&& root.mkpath("move-destination"), "could not create move test directories");
	const auto selectedPath = QDir(source).filePath("selected.png");
	const auto replacementPath = QDir(other).filePath("replacement.png");
	saveImage(selectedPath, Qt::red);
	saveImage(replacementPath, Qt::blue);
	ThumbModel model;
	loadDirectory(model, source);
	ThumbView view(&model);
	view.select(selectedPath);
	require(!view.selectionModel()->selectedIndexes().isEmpty(), "move selection is empty");

	bool resetInDialog = false;
	QString dialogFailure;
	QTimer dialogTimer;
	dialogTimer.setInterval(10);
	QObject::connect(&dialogTimer, &QTimer::timeout, &view, [&]() {
		for(auto widget: QApplication::topLevelWidgets()) {
			auto dialog = qobject_cast<QFileDialog*>(widget);
			if(!dialog || !dialog->isVisible())
				continue;
			dialogTimer.stop();
			stage("move dialog found");
			try {
				loadDirectory(model, other);
				resetInDialog = true;
				auto fileModel = dialog->findChild<QFileSystemModel*>();
				require(fileModel != nullptr, "file dialog filesystem model not found");
				bool directoryLoaded = false;
				const auto connection = QObject::connect(fileModel, &QFileSystemModel::directoryLoaded,
					dialog, [&](const QString &path) {
						if(QDir(path).absolutePath()==QDir(destination).absolutePath())
							directoryLoaded = true;
					});
				dialog->setDirectory(destination);
				const bool ready = waitUntil([&]() { return directoryLoaded; });
				QObject::disconnect(connection);
				require(ready, "destination directory did not load in file dialog");
				require(dialog->directory().absolutePath()==QDir(destination).absolutePath(),
					"file dialog did not navigate to the destination");
				// Select the directory explicitly, after the dialog's own asynchronous
				// model has finished changing its current index and selection.
				auto fileNameEdit = dialog->findChild<QLineEdit*>(QStringLiteral("fileNameEdit"));
				require(fileNameEdit != nullptr, "file dialog filename editor not found");
				fileNameEdit->setText(QDir::toNativeSeparators(destination));
				qInfo() << "Move dialog selection:" << dialog->selectedFiles();
				require(dialog->selectedFiles().size()==1
					&& QDir(dialog->selectedFiles().first()).absolutePath()==QDir(destination).absolutePath(),
					"file dialog selected a different destination");
				QMetaObject::invokeMethod(dialog, "accept", Qt::DirectConnection);
				stage("move dialog accepted");
			} catch(const std::exception &error) {
				dialogFailure = QString::fromUtf8(error.what());
				dialog->reject();
			}
			return;
		}
	});
	QTimer watchdog;
	watchdog.setSingleShot(true);
	QObject::connect(&watchdog, &QTimer::timeout, &view, [&]() {
		dialogFailure = QStringLiteral("move dialog timed out");
		for(auto widget: QApplication::topLevelWidgets())
			if(auto dialog = qobject_cast<QDialog*>(widget))
				dialog->reject();
	});
	dialogTimer.start();
	watchdog.start(120000);
	view.moveFiles();
	stage("move returned");
	dialogTimer.stop();
	watchdog.stop();
	require(dialogFailure.isEmpty(), qPrintable(dialogFailure));
	require(resetInDialog, "model was not reset inside the file dialog");
	qInfo() << "Move destination:" << destination << QDir(destination).entryList();
	require(!QFileInfo::exists(selectedPath), "originally selected file was not moved");
	require(QFileInfo::exists(QDir(destination).filePath("selected.png")), "moved file missing at destination");
	require(QFileInfo::exists(replacementPath), "replacement model's file was moved");
}
}

int main(int argc, char **argv) {
	stage("main");
	if(argc!=2) {
		fprintf(stderr, "Usage: view_regression EMPTY_TEMP_DIRECTORY\n");
		return 2;
	}
	qputenv("QT_QPA_PLATFORM", "offscreen");
	qputenv("QT_FORCE_STDERR_LOGGING", "1");
	QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
	const QDir temporary(QString::fromLocal8Bit(argv[1]));
	if(!temporary.exists() || !temporary.entryList(QDir::AllEntries | QDir::NoDotAndDotDot
		| QDir::Hidden | QDir::System).isEmpty())
		return 2;
	QList<QByteArray> arguments = {"view_regression", "--screenshot", "--manual-run",
		"--folder", temporary.path().toUtf8(), "--screenshot-delay", "600000"};
	QList<char*> pointers;
	for(auto &argument: arguments)
		pointers << argument.data();
	pointers << nullptr;
	int argumentCount = arguments.size();
	CApplication application(argumentCount, pointers.data());
	stage("application constructed");
	application.setQuitOnLastWindowClosed(false);
	try {
		checkImageNavigation(QDir(temporary.path()));
		checkMoveAcrossReset(QDir(temporary.path()));
		qInfo("View regressions passed: snapshot End, destroyed model, empty sequence, move across reset");
		return 0;
	} catch(const std::exception &error) {
		qCritical("View regression failed: %s", error.what());
		return 1;
	}
}
