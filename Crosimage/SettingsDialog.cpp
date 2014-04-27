//SettingsDialog.cpp by Kostya Kozachuck as neurocod
#include "pch.h"
#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget*parent): QDialog(parent) {
	setWindowTitle(tr("Settings"));
	VBoxLayout lay(this);
	FormLayout form(lay);
	form->addRow(tr("Shutdown command:"), m_editShutdownCommand);
	form << tr("  For example, rm thumbnails.sqlite - needed while project is in Alpha state");
	lay << tr("TODO: add thumbnails width and height here");
	{
		New<DialogButtonBox> box(this, true);
		box->acceptRejectTo(this);
		lay << box;
	}
	updateSettings(false);
}
void SettingsDialog::createAndShow(QWidget*parent) {
	SettingsDialog d(parent);
	if(d.exec()!=QDialog::Accepted)
		return;
	d.updateSettings(true);
}
void SettingsDialog::updateSettings(bool save) {
	Settings sett;
	sett.updateValue(save, "shutdownCommand", m_editShutdownCommand);
}