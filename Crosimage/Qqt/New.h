//New.h by Kostya Kozachuck as neurocod - 27.09.2011 16:01:22
#pragma once

template<class Class>
class New {
	private:
		Class*d;
	public:
		template<typename ... Args>
		New(Args...args) {
			d = new Class(args...);
		}
		Class* operator->() { return d; }
		const Class* operator->()const { return d; }
		operator Class* () { return d; }
		operator const Class* ()const { return d; }
};
template<class T>
T* autoNew(T*&var) {
	T*tmp = new T();
	var = tmp;
	return tmp;
}
template<class T>class QPointer;
template<class T>
T* autoNew(QPointer<T> &var) {
	T*tmp = new T();
	var = tmp;
	return tmp;
}

template<class List>
bool isValidIndex(const List & list, int index) {
	return index>=0 && index<list.count();
}