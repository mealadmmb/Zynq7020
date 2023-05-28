#pragma once
#include <QtGlobal.h>
#include <vector>

template <class T>
class QVector;

template <class T>
struct QVectorIterator : public std::vector<T>::iterator, public T {

	friend QVector<T>;

};

template <class T>
struct QVectorConstIterator : public std::vector<T>::const_iterator, public T {

	friend QVector<T>;
};

//template <class T>
//struct QVectorIterator {
//
//    friend QVector<T>;
//
//    using iterator_category = std::random_access_iterator_tag;
//    using difference_type   = std::ptrdiff_t;
//    using value_type        = T;
//    using pointer           = value_type*;  // or also value_type*
//    using reference         = value_type&;  // or also value_type&
//
//    explicit QVectorIterator(typename std::vector<T>::iterator it) : it(move(it)) {}
//
//    void operator++(int)  { // it++
//        it++;
//    }
//
//    void operator++()  { // ++it
//        ++it;
//    }
//
//    void operator--(int)  { // it--
//        it--;
//    }
//
//    void operator--()  { // --it
//        --it;
//    }
//
//    QVectorIterator operator+(int c) const {
//        QVectorIterator out(*this);
//        out += c;
//        return out;
//    }
//
//    QVectorIterator operator-(int c) const {
//        QVectorIterator out(*this);
//        out -= c;
//        return out;
//    }
//
//    void operator+=(int c) { it += c; }
//    void operator-=(int c) { it -= c; }
//
//    bool operator==(const QVectorIterator<T>& other) const {
//        return it == other.it;
//    }
//
//    bool operator!=(const QVectorIterator<T>& other) const{
//        return it != other.it;
//    }
//
//    reference operator*() const {
//        return it.operator*();
//    }
//
//    pointer operator->() const {
//        return it.operator->();
//    }
//
//    const T* base() const {
//        return it.base();
//    }
//
//
//private:
//    typename std::vector<T>::iterator get_std_iterator(){
//        return it;
//    }
//
//    typename std::vector<T>::iterator it;
//};

//template <class T>
//struct QVectorConstIterator {
//
//    friend QVector<T>;
//
//    using iterator_category = std::random_access_iterator_tag;
//    using difference_type   = std::ptrdiff_t;
//    using value_type        = T;
//    using pointer           = const value_type*;  // or also value_type*
//    using reference         = const value_type&;  // or also value_type&
//
//    explicit QVectorConstIterator(typename std::vector<T>::const_iterator it) : it(move(it)) {}
//
//    void operator++(int) { // it++
//        it++;
//    }
//
//    void operator++() { // ++it
//        ++it;
//    }
//
//    void operator--(int)  { // it--
//        it--;
//    }
//
//    void operator--()  { // --it
//        --it;
//    }
//
//    QVectorConstIterator operator+(int c) const {
//        QVectorConstIterator out(*this);
//        out += c;
//        return out;
//    }
//
//    QVectorConstIterator operator-(int c) const {
//        QVectorConstIterator out(*this);
//        out -= c;
//        return out;
//    }
//
//    void operator+=(int c) { it += c; }
//    void operator-=(int c) { it -= c; }
//
//    bool operator==(const QVectorConstIterator<T>& other) const{
//        return it == other.it;
//    }
//
//    bool operator!=(const QVectorConstIterator<T>& other) const {
//        return it != other.it;
//    }
//
//    reference operator*() const{
//        return it.operator*();
//    }
//
//    pointer operator->() const{
//        return it.operator->();
//    }
//    const T* base() const {
//        return it.base();
//    }
//
//private:
//    typename std::vector<T>::const_iterator get_std_iterator(){
//        return it;
//    }
//
//    typename std::vector<T>::const_iterator it;
//};








template <class T>
class 
QVector: public std::vector<T>{
public:
	using std::vector<T>::vector;
	using std::vector<T>::insert;
	using std::vector<T>::erase;
	void append(const T&  newElement);
	void append(const QVector<T>& vec);
	T& last() ;
	T& first() ;

	const T& last() const ;
	const T& first() const;

	void reserve(const size_t& size) {
		//std::cout << "reserving " << size << " space in QVector\r\n";
		if(size > std::vector<T>::max_size()) {
			//printf("****************** reserving %zu sapce in QVector, max reservable space: %zu ******************\r\n", size, std::vector<T>::max_size());
			return;
		}
		std::vector<T>::reserve(size);
	}


	//    QVectorIterator<T> begin()   {
	//        return QVectorIterator<T>(std::vector<T>::begin());
	//    }
	//    QVectorIterator<T> end()   {
	//        return QVectorIterator<T>(std::vector<T>::end());
	//    }
	//
	//    QVectorConstIterator<T> begin() const {
	//        return QVectorConstIterator<T>(std::vector<T>::begin());
	//    }
	//    QVectorConstIterator<T> end()  const  {
	//        return QVectorConstIterator<T>(std::vector<T>::end());
	//    }
	//
	//    const QVectorConstIterator<T> constBegin() const {
	//        return QVectorConstIterator<T>(std::vector<T>::cbegin());
	//    }
	//    const QVectorConstIterator<T> constEnd() const {
	//        return QVectorConstIterator<T>(std::vector<T>::cend());
	//    }

	void insert(int index, const T& item);
	void removeAt(int i);
	bool isEmpty() const;
	int count(const T &value) const;
	inline int count() const { return this->size();}

	inline QVector& operator<<(const T& item) {
		std::vector<T>::push_back(item);
		return *this;
	}

private:


};

template<class T>
T &QVector<T>::last() {
	return std::vector<T>::operator[](std::vector<T>::size() - 1);
}
template<class T>
bool QVector<T>::isEmpty() const {
	return std::vector<T>::empty();
}

template<class T>
T &QVector<T>::first()  {
	return std::vector<T>::operator[](0);
}

template<class T>
void QVector<T>::append(const T& newElement) {
	std::vector<T>::push_back(newElement);
}

template<class T>
void QVector<T>::append(const QVector<T>& vec) {
	std::vector<T>::insert(std::vector<T>::end(), vec.begin(), vec.end());
}

template<class T>
const T &QVector<T>::last() const {
	return std::vector<T>::operator[](std::vector<T>::size() - 1);
}

template<class T>
const T &QVector<T>::first() const {
	return std::vector<T>::operator[](0);
}

template<class T>
void QVector<T>::removeAt(int i) {
	std::vector<T>::erase(next(this->begin(), i));
}

template<class T>
int QVector<T>::count(const T &value) const {
	int out = 0;
	for (auto item : *this) if (item == value) out++;
	return out;
}

template<class T>
void QVector<T>::insert(int index, const T &item) {
	std::vector<T>::insert(std::vector<T>::begin() + index, item);
}



