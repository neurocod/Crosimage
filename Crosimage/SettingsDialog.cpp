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
	} else {
		sett._shutdownCommand = _editShutdownCommand.text;
		CrSettings::setInst(sett);
	}
}