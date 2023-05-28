#pragma once

#include <deque>

template <class T>
class QQueue: public std::deque<T>{

	using std::deque<T>::deque;

public:
	T dequeue() {
		T t = this->back();
		this->pop_back();
		return t;

	}

	void enqueue(const T &t) {
		this->push_back(t);
	}

	T& head() {
		return this->back();
	}

	const T& head() const {
		return this->back();
	}

	void swap(QQueue<T> &other) {

	}
	bool isEmpty() const {
		return this->empty();
	}
};
