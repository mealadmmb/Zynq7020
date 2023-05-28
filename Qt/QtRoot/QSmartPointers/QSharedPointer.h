#pragma once
#include <memory>
template <typename T>
class QSharedPointer : std::shared_ptr<T> {
	using std::shared_ptr<T>::shared_ptr;

public:
	QSharedPointer() = default;
};
