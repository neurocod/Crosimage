#include "pch.h"
#include "Screenshot.h"

bool Screenshot::save(QWidget *widget, const QString &directory, QString *error) {
	auto fail = [error](const QString &message) {
		if(error)
			*error = message;
		return false;
	};
	if(!widget || !widget->isVisible())
		return fail(QStringLiteral("Screenshot target is not visible"));
	QDir dir(directory);
	if(!dir.mkpath(QStringLiteral("old")))
		return fail(QStringLiteral("Cannot create screenshot directory: %1").arg(directory));
	const QString latest = dir.filePath(QStringLiteral("latest.png"));
	const QString previous = dir.filePath(QStringLiteral("previous.png"));
	// Encode first: a failed capture must not rotate the existing screenshots.
	QSaveFile output(latest);
	const QPixmap pixmap = widget->grab();
	if(pixmap.isNull() || !output.open(QIODevice::WriteOnly)
		|| !pixmap.save(&output, "PNG"))
		return fail(QStringLiteral("Cannot save screenshot: %1 (%2)").arg(latest, output.errorString()));
	QString archived;
	if(QFile::exists(previous)) {
		const QString stem = dir.filePath(QStringLiteral("old/%1")
			.arg(QFileInfo(previous).lastModified().toString(QStringLiteral("yyyy.MM.dd-HH.mm.ss.zzz"))));
		archived = stem + QStringLiteral(".png");
		for(int suffix = 1; QFile::exists(archived); ++suffix)
			archived = stem + QStringLiteral("-%1.png").arg(suffix);
		if(!QFile::rename(previous, archived))
			return fail(QStringLiteral("Cannot archive screenshot: %1").arg(previous));
	}
	const bool hadLatest = QFile::exists(latest);
	if(hadLatest && !QFile::rename(latest, previous)) {
		if(!archived.isEmpty())
			QFile::rename(archived, previous);
		return fail(QStringLiteral("Cannot rotate screenshot: %1").arg(latest));
	}
	if(!output.commit()) {
		if(hadLatest)
			QFile::rename(previous, latest);
		if(!archived.isEmpty())
			QFile::rename(archived, previous);
		return fail(QStringLiteral("Cannot commit screenshot: %1 (%2)").arg(latest, output.errorString()));
	}
	return true;
}
