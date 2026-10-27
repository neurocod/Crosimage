//SettingsDialog.h by Kostya Kozachuck as neurocod
#pragma once

class SettingsDialog: public QDialog {
	public:
		SettingsDialog(QWidget*parent);
		static void createAndShow(QWidget*parent);
	protected:
		LineEdit _editShutdownCommand;
		SpinBox _thumbW;
		SpinBox _thumbH;
#ifdef _DEBUG
		CheckBox _debugSessions;
#endif

		void updateSettings(bool toGui);
};
