//Two.h by Kostya Kozachuck as neurocod - 01.08.2012 20:13:28
#pragma once

template<class T>
class Two {
	public:
		T a, b;

		Two() {
		}
		template<class T1, class T2>
		Two(T1 t1, T2 t2): a(t1), b(t2) {
		}
		template<class T1, class T2>
		void set(T1 t1, T2 t2) {
			a = t1;
			b = t2;
		}
		T& operator[](int i) { ASSERT(i==0 || i==1); return i==0 ? a : b; }
};