//NullIODevice.h by Konstantine Kozachuck as neurocod - 2018.02.17 15:20:57
#pragma once

class NullIODevice: public QIODevice {
	public:
		NullIODevice() {
			setOpenMode(QIODevice::WriteOnly);
		}
		virtual qint64 writeData(const char *data, qint64 maxSize)override {
			return maxSize;
		}
		virtual qint64 readData(char *data, qint64 maxSize)override {
			return 0;
		}
};