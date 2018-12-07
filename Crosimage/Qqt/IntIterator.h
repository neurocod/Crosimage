//IntIterator.h by neurocod - 2013.08.22 17:50:14
#pragma once

template<class T=int>
class IntIterator {
		T n;
	public:
		IntIterator(T _n = 0) { n = _n; }
		IntIterator(const IntIterator & other) { n = other.n; }
		bool operator==(const IntIterator & other)const {
			return n==other.n;
		}
		bool operator!=(const IntIterator & other)const {
			return n!=other.n;
		}
		T operator*() const { return n; }
		IntIterator& operator++() {
			++n;
			return *this;
		}
		IntIterator operator++(int) {
			IntIterator ret = *this;
			++n;
			return ret;
		}
};
template<class T=int>
class XRange {
		T _begin, _end;
	public:
		XRange(T _end): _begin(0), _end(_end) {
			if(_begin>_end)
				_begin = _end;
		}
		XRange(T _begin, T _end): _begin(_begin), _end(_end) {
			if(_begin>_end)
				_begin = _end;
		}
		IntIterator<T> begin()const { return IntIterator<T>(_begin); }
		IntIterator<T> end()const { return IntIterator<T>(_end); }
};
template<class T>
XRange<T> xrange(T t) {
	return XRange<T>(t);
}
template<class T>
XRange<T> xrange(T start, T end) {
	return XRange<T>(start, end);
}
namespace std {
	//too dangerous:
	IntIterator<int> begin(int i);
	IntIterator<int> end(int i);
}
namespace std {
	template<class T>
	T& reverse(T & t) {
		std::reverse(t.begin(), t.end());
		return t;
	}
}