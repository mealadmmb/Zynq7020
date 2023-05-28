#pragma once

#include <memory>
template <typename T>
class QUniquePointer : std::unique_ptr<T> {
	using std::unique_ptr<T>::unique_ptr;
public:

};
