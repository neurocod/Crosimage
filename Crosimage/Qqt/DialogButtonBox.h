//DialogButtonBox.h by neurocod - 13.12.2012 9:09:01
#pragma once

class DialogButtonBox: public QDialogButtonBox {
	public:
		DialogButtonBox(QDialog*parent=0, bool addOkCancelAcceptReject=false);
		QPushButton* addOk();
		QPushButton* addCancel();
		void acceptRejectTo(QDialog*dlg);
		static const QPixmap& cancelIcon();
		static const QPixmap& okIcon();
};