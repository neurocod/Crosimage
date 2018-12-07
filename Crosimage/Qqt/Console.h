//Console.h by Kostya Kozachuck as neurocod - 2018.05.15 14:35:58
#pragma once

class Console: public QTextStream {
	public:
		Console();
		
		Console& setTextColor(QRgb color, bool intense = true);
		Console& setTextColor();
		Console& write(const QString & text, QRgb color);
		Console& writeDateTime();
		void setWindowTitle(const QString & s);
};