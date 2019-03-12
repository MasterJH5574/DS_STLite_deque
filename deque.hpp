#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include "linklist.hpp"
#include <cstddef>

namespace sjtu {



template <class T>
class deque : public LinkList<LinkList<T> > {
    static const int SIZE = 500;

public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
		}
		iterator operator-(const int &n) const {
			//TODO
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
		}
		iterator operator+=(const int &n) {
			//TODO
		}
		iterator operator-=(const int &n) {
			//TODO
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {}
		/**
		 * TODO *it
		 */
		T& operator*() const {}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(const const_iterator &other) {
				// TODO
			}
			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};

private:
    // the number of elements in this deque
    size_t element_size;

public:

	/**
	 * TODO Constructors
	 */
	deque() : LinkList<LinkList<T> >(), element_size(0) {
	}

	deque(const deque &o) : LinkList<LinkList<T> >(o), element_size(o.element_size) {}

	/**
	 * TODO Deconstructor
	 */
	~deque() {}

	/**
	 * TODO assignment operator
	 */
	deque& operator=(const deque &o) {
	    if (this == &o)
	        return *this;

	    element_size = o.element_size;

	    LinkList<LinkList<T> >::operator=(o);
	    return *this;
	}

	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 * 0-base default
	 */
	T& at(const size_t &pos) {
	    if (pos < 0 || pos >= element_size)
	        throw(index_out_of_bound("index out of bound when accessing specified element"));

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data.size()) {
            pos -= p->data.size();
            p = p->next;
        }
        return p->data.at(pos);
	}

	const T& at(const size_t &pos) const {
        if (pos < 0 || pos >= element_size)
            throw(index_out_of_bound("index out of bound when accessing specified element"));

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data.size()) {
            pos -= p->data.size();
            p = p->next;
        }
        return p->data.at(pos);
	}

	T& operator[](const size_t &pos) {
        if (pos < 0 || pos >= element_size)
            throw(index_out_of_bound("index out of bound when accessing specified element"));

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data.size()) {
            pos -= p->data.size();
            p = p->next;
        }
        return p->data.at(pos);
	}

	const T& operator[](const size_t &pos) const {
        if (pos < 0 || pos >= element_size)
            throw(index_out_of_bound("index out of bound when accessing specified element"));

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data.size()) {
            pos -= p->data.size();
            p = p->next;
        }
        return p->data.at(pos);
	}

	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T& front() const {
	    if (empty())
	        throw container_is_empty("the container is empty");
	    return this->head.next->data.front();
	}

	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T& back() const {
        if (empty())
            throw container_is_empty("the container is empty");
        return this->tail.last->data.back();
	}

	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {}
	const_iterator cend() const {}

	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
	    return element_size == 0;
	}

	/**
	 * returns the number of elements
	 */
	size_t size() const {
	    return element_size;
	}

	/**
	 * clears the contents
	 */
	void clear() {
	    LinkList<LinkList <T> >::clear();
	    element_size = 0;
	}

	/**
	 * inserts elements at the specified locate on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {}

	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {}

	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {}

	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {}

	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {}

	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {}
};

}

#endif
