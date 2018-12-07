//ByteArrayWithDataStream.h by Kostya Kozachuck as neurocod - 01.06.2012 21:12:00
#pragma once

class ByteArrayWithDataStream {
	public:
		QByteArray _arr;
		QDataStream _stream;
		//write mode
		ByteArrayWithDataStream(): _stream(&_arr, QIODevice::WriteOnly) {}
		//read mode
		ByteArrayWithDataStream(const QByteArray & arr): _arr(arr), _stream(&_arr, QIODevice::ReadOnly) {}
		template<class T>
		ByteArrayWithDataStream & operator<<(const T & t) {
			_stream << t;
			return *this;
		}
		template<class T>
		ByteArrayWithDataStream & operator>>(T & t) {
			_stream >> t;
			return *this;
		}
		operator QDataStream &(){ return _stream; }
		operator QByteArray(){ return _arr; }
};
template<class TValue>
class SerializableToQDataStream {
	public:
		virtual operator TValue()const=0;
		virtual TValue operator=(const TValue & val)=0;
};
template<class T>
QDataStream & operator>>(QDataStream & stream, SerializableToQDataStream<T> & t) {
	T t2;
	stream >> t2;
	t = t2;
	return stream;
}
template<class T>
QDataStream & operator<<(QDataStream & stream, SerializableToQDataStream<T> & t) {
	T t2 = t;
	stream << t2;
	return stream;
}