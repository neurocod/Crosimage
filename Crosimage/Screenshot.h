#pragma once

class QWidget;
class QString;

namespace Screenshot {

// Capture the widget's client area and retain the two most recent PNGs.
bool save(QWidget *widget, const QString &directory, QString *error = nullptr);

}
