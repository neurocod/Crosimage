//IntIterator.cpp by neurocod - 2013.08.22 17:50:14
#include "pch.h"
//already included in pch.h #include "IntIterator.h"

namespace std {
	//too dangerous:
	IntIterator<int> begin(int i) {
		return 0;
	}
	IntIterator<int> end(int i) {
		return IntIterator<int>(i);
	}
}