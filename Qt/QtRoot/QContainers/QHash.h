#pragma once
#include <utility>
#include <iterator>
#include <unordered_map>
#include <iostream>
#include <QList.h>

template<typename KT, typename VT>
class QHash;

template<typename KT, typename VT>
struct QHashIterator {

	friend QHash<KT,VT>;

	using iterator_category = std::random_access_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = VT;
	using pointer           = value_type*;  // or also value_type*
	using reference         = value_type&;  // or also value_type&

	QHashIterator(typename std::unordered_map<KT, VT>::iterator it) : it(move(it)) {}

	const KT& key()  {
		return it->first;
	}

	VT& value()  {
		return it->second;
	}

	void operator++(int) { // it++
		it++;
	}

	void operator++() { // ++it
		++it;
	}

	bool operator==(const QHashIterator<KT, VT>& other) {
		return it == other.it;
	}

	bool operator!=(const QHashIterator<KT, VT>& other) {
		return it != other.it;
	}

	reference operator*() {
		return it->second;
	}

	pointer operator->() {
		return &it->second;
	}

private:
	typename std::unordered_map<KT, VT>::iterator get_std_iterator(){
		return it;
	}

	typename std::unordered_map<KT, VT>::iterator it;
};

template<typename KT, typename VT>
struct QHashConstIterator {

	friend QHash<KT,VT>;

	using iterator_category = std::random_access_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = VT;
	using pointer           = const value_type*;  // or also value_type*
	using reference         = const value_type&;  // or also value_type&

	QHashConstIterator(typename std::unordered_map<KT, VT>::const_iterator it) : it(move(it)) {}


	const KT& key() const {
		return it->first;
	}

	const VT& value() const {
		return it->second;
	}

	void operator++(int) { // it++
		it++;
	}

	void operator++() { // ++it
		++it;
	}

	bool operator==(const QHashConstIterator<KT, VT>& other)  {
		return it == other.it;
	}

	bool operator!=(const QHashConstIterator<KT, VT>& other)  {
		return it != other.it;
	}

	reference operator*() {
		return it->second;
	}

	pointer operator->() {
		return &it->second;
	}

private:
	typename std::unordered_map<KT, VT>::const_iterator get_std_iterator(){
		return it;
	}

	typename std::unordered_map<KT, VT>::const_iterator it;
};

template<typename KT, typename VT>
class QHash : public std::unordered_map<KT, VT>
{
public:
	using std::unordered_map<KT, VT>::unordered_map;

	bool contains(const KT& key) const ;
	void insert(const KT& key, const VT& value);

    KT& key(const VT& value);
	const KT& key(const VT& value) const;

    VT& value(const KT& key);
	const VT& value(const KT& key) const;


	QList<KT> keys() const;

	int remove(const KT &key);

	const VT& operator[](const KT& key) const {
		return std::unordered_map<KT, VT>::at(key);
	}

	VT& operator[](const KT& key) {
		return std::unordered_map<KT, VT>::operator[](key);
	}
	//    const VT& operator[](const KT& key)  {
	//        return std::unordered_map<KT, VT>::operator[](key);
	//    }






	QHashIterator<KT, VT> begin() {
		return QHashIterator<KT, VT>(std::unordered_map<KT, VT>::begin());
	}
	QHashIterator<KT, VT> end() {
		return QHashIterator<KT, VT>(std::unordered_map<KT, VT>::end());
	}

	const QHashConstIterator<KT, VT> constBegin() const {
		return QHashConstIterator<KT, VT>(std::unordered_map<KT, VT>::cbegin());
	}
	const QHashConstIterator<KT, VT> constEnd() const {
		return QHashConstIterator<KT, VT>(std::unordered_map<KT, VT>::cend());
	}

	QHashIterator<KT, VT> erase(QHashIterator<KT, VT> it) {
		auto std_it = std::unordered_map<KT, VT>::erase(it.get_std_iterator());
		return QHashIterator<KT, VT>(std_it);
	}

	QHashIterator<KT, VT> erase(const KT& key) {
		if(!contains(key)) return end();
		auto foundIterator = this->find(key);
		return this->erase(foundIterator);
	}


	QHashIterator<KT, VT> find(const KT& key) {
		return QHashIterator<KT, VT>(std::unordered_map<KT, VT>::find(key));
	}

	QHashConstIterator<KT, VT> find(const KT& key) const {
		return QHashConstIterator<KT, VT>(std::unordered_map<KT, VT>::find(key));
	}

	bool isEmpty() const { return std::unordered_map<KT, VT>::empty(); }


private:


};

template<typename KT, typename VT>
bool QHash<KT, VT>::contains(const KT &key) const{
	return (std::unordered_map<KT, VT>::find(key) != std::unordered_map<KT, VT>::end());
}
template<typename KT, typename VT>
const VT& QHash<KT, VT>::value(const KT& key) const{
	if(contains(key))
		return std::unordered_map<KT, VT>::at(key);
	return VT();
}

template<typename KT, typename VT>
const KT& QHash<KT, VT>::key(const VT& value) const {
	for(auto it = std::unordered_map<KT, VT>::begin(); it != std::unordered_map<KT, VT>::end(); ++it){
		if(it.value() == value){
			return  it.key();
		}
	}
	return KT();
}

template<typename KT, typename VT>
void QHash<KT, VT>::insert(const KT& key, const VT& value) {
	/*
    std::unordered_map<KT, VT>* p = this;
    (*p)[key] = value;
	 */
	//std::unordered_map<KT, VT>::operator[](key) = value;

	//std::unordered_map<KT, VT>::insert (std::make_pair<KT, VT>(&key, &value));
	std::unordered_map<KT, VT>::insert ({key, value});
}

template<typename KT, typename VT>
int QHash<KT, VT>::remove(const KT &key) {
	int out = 0;
	if(contains(key)) {
		auto hashSizeBeforeErase = std::unordered_map<KT, VT>::size();
		erase(find(key));
		auto hashSizeAfterErase = std::unordered_map<KT, VT>::size();
		out = hashSizeBeforeErase != hashSizeAfterErase;
	} else
		out = 0;
	return out;
}

template<typename KT, typename VT>
QList<KT> QHash<KT, VT>::keys() const {
	QList<KT> out;
	for(auto item = constBegin(); item != constEnd(); item++) {
		out.push_back(item.key());
	}
	return out;
}

template<typename KT, typename VT>
KT &QHash<KT, VT>::key(const VT &value) {
    for(auto& it = std::unordered_map<KT, VT>::begin(); it != std::unordered_map<KT, VT>::end(); ++it){
        if(it.value() == value){
            return  it.key();
        }
    }
//    return KT();
}

template<typename KT, typename VT>
VT &QHash<KT, VT>::value(const KT &key) {
	return std::unordered_map<KT, VT>::at(key);
}


template<typename KT, typename VT>
inline std::ostream& operator<<(std::ostream& os, const QHash<KT, VT>& hash) {

	if (hash.empty()){
		os << "empty QHash()\n";
		return os;
	}
	for (auto item = hash.constBegin(); item != hash.constEnd(); item++) {
		os << "\"" << item.key() << "\" : \"" << item.value() << "\"" << std::endl;
	}
	return os;
}







