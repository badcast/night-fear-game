#pragma once

#include <stdio.h>

#undef NULL
#define NULL 0

//#define _ref std::shared_ptr

/*
template <typename _Ty = Object>
class refin
{
	_Ty* _ptr;
	uint8_t refCount;
public:
	using type = _Ty;
	shared_ptr
	//create empty
	refin() {
		_ptr = NULL; refCount = 0;
	}

	refin(refin& ref) {
		_ptr = ref._ptr;
		refCount = ++refCount;
	}

	~refin() {
		if (!--refCount)
			deallocate_class(_ptr);
	}

	bool isUnique() {
		return refCount == 1;
	}

	void reset() {
		_ptr = NULL;
	}

	explicit bool operator() {
		return _ptr != 0;
	}

	_Ty& opearator* () {
		return _ptr;
	}

	_Ty& operator=(nullptr_t) {
		if (*this())
		{
			reset();
		}
	}
};


*/