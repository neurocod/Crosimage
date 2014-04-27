//SettingsDialog.h by Kostya Kozachuck as neurocod
#pragma once

class SettingsDialog: public QDialog {
	public:
		SettingsDialog(QWidget*parent);
		virtual ~SettingsDialog() {}
		static void createAndShow(QWidget*parent);
	protected:
		LineEdit m_editShutdownCommand;
		void updateSettings(bool save);
};