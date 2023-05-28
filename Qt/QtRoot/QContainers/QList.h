#pragma once
#include <list>
#include <iostream>

template <class T>
class QList: public std::list<T>{

public:
	using std::list<T>::list;
	void append(const T& item);
	const T& at(const size_t& idx) const;
	T& at(const size_t& idx) ;
	void removeAt(const size_t& idx);
	const T& operator[](const size_t& i) const {
		return at(i);
	}
	T& operator[](const size_t& i) {
		return at(i);
	}
	bool isEmpty() const { return std::list<T>::empty(); }

	void reserve (const size_t& s) {}

};

template<class T>
void QList<T>::append(const T &item) {
	std::list<T>::insert(std::list<T>::end(), item);
}

template<class T>
const T &QList<T>::at(const size_t &idx) const {
	return *(std::next(std::list<T>::cbegin(), idx));
}
template<class T>
T &QList<T>::at(const size_t &idx)  {
	return *(std::next(std::list<T>::begin(), idx));
}

template<class T>
void QList<T>::removeAt(const size_t &idx) {
	std::list<T>::erase(next(std::list<T>::begin(), idx));
}

template<class T>
std::ostream &operator<<(std::ostream &os, const QList<T> &list) {
	if(list.isEmpty()) {
		os << "empty QList";
		return os;
	}
	for (const auto& item : list) {
		os << "\"" << item << "\"";
	}
	return os;
}
