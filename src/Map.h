#pragma once
#include <cstdlib>

template <typename K, typename V>
class KeyValuePair {
public:
    K key;
    V value;

    void set(K k, V v) {
        key = k;
        value = v;
    }
};

template <typename K, typename V>
class Map {
public:

    Map() {
        init(1000);
    }

    Map(const size_t capacity) {
        init(capacity);
    }

    void init(const size_t capacity) {
        _capacity = capacity;
        _data = (KeyValuePair<K, V>*) malloc(sizeof (KeyValuePair<K, V>) * capacity);
        _size = 0;
    }

    virtual ~Map() {
        free(_data);
    }

    const bool put(const K &key, const V &value) {
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i].key == key) {
                _data[i].value = value;
                return true;
            }
        }
        return add(key, value);
    }

    const bool get(const K &key, V &value) {
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i].key == key) {
                value = _data[i].value;
                return true;
            }
        }
        return false;
    }

    const bool contains(const K &key) const {
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i].key == key) {
                return true;
            }
        }
        return false;
    }

    const bool remove(const K &key) {
        for (size_t i = 0; i < _size; ++i) {
            if (_data[i].key == key) {
                _data[i].key = _data[_size - 1].key;
                _data[i].value = _data[_size - 1].value;
                --_size;
                return true;
            }
        }
        return false;
    }

    const size_t size() const {
        return _size;
    }

    const size_t capacity() const {
        return _capacity;
    }

private:
    KeyValuePair<K, V> *_data;
    size_t _capacity, _size;

    const bool add(K key, V value) {
        if (_size == _capacity) {
            return false;
        }
        _data[_size++].set(key, value);
        return true;
    }

    Map(const Map& orig);

};


