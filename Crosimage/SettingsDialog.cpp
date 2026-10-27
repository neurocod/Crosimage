//SettingsDialog.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "SettingsDialog.h"
#include "CrSettings.h"

SettingsDialog::SettingsDialog(QWidget*parent): QDialog(parent) {
	setWindowTitle(tr("Settings"));
	VBoxLayout lay(this);
	FormLayout form(lay);
	form->addRow(tr("Shutdown command:"), _editShutdownCommand);
	form << tr("  For example, rm thumbnails.sqlite - needed while project is in Alpha state");
	form->addRow(tr("Thumbnail width:"), _thumbW);
	form->addRow(tr("Thumbnail height:"), _thumbH);
#ifdef _DEBUG
	_debugSessions.text = tr("Debug sessions (restart required)");
	_debugSessions.toolTip = tr("Save and restore open folders in a separate session for debug builds.");
	form->addRow(_debugSessions);
#endif
	{
		New<DialogButtonBox> box(this, true);
		box->acceptRejectTo(this);
		lay << box;
	}
	updateSettings(true);
}
void SettingsDialog::createAndShow(QWidget*parent) {
	SettingsDialog d(parent);
	if(d.exec()!=QDialog::Accepted)
		return;
	d.updateSettings(false);
}
void SettingsDialog::updateSettings(bool toGui) {
	CrSettings sett = CrSettings::inst();
	sett._thumbW.update(toGui, _thumbW);
	sett._thumbH.update(toGui, _thumbH);
	if(toGui) {
		_editShutdownCommand.text = sett._shutdownCommand;
#ifdef _DEBUG
		_debugSessions.checked = sett._debugSessions;
#endif
	} else {
		sett._shutdownCommand = _editShutdownCommand.text;
#ifdef _DEBUG
		sett._debugSessions = _debugSessions.checked;
#endif
		CrSettings::setInst(sett);
	}
}
