#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <unordered_set>

namespace sjtu {

template <class T>
void swap(T &a, T &b) {
    T c = a;
    a = b, b = c;
}

template<class T>
class LinkList {
private:
    struct node {
        T *data;
        node *next, *last;
        explicit node(T *d = nullptr, node *n = nullptr, node *l = nullptr) : data(d), next(n), last(l) {}
    } head, tail;

    size_t _size;

    int get_pos(const node *x) {
        int res = 0;
        node *p = head.next;
        while (p != x) {
            p = p->next;
            res++;
        }

        return res;
    }

public:
    LinkList() : head(node()), tail(node()), _size(0) {
        head.next = &tail;
        tail.last = &head;
    }

    LinkList(const LinkList<T> &o) : head(node()), tail(node()), _size(o._size) {
        head.next = &tail;
        tail.last = &head;

        if (o.empty())
            return;

        node *p = &o.head, *pp = &head, *q = nullptr;
        while (p->next != &o.tail) {
            q = new node(new T(*(p->next->data)), nullptr, pp);
            pp->next = q;
        }
        pp->next = &tail;
        tail.last = pp;
    }

    virtual ~LinkList() {
        clear();
    }

    virtual bool empty() {
        return _size == 0;
    }

    virtual size_t size() const {
        return _size;
    }

    virtual void init() {
        head.next = &tail;
        tail.last = &head;
        _size = 0;
    }

    LinkList &operator=(const LinkList &o) {
        if (this == &o)
            return *this;

        clear();
        if (o.empty())
            return *this;

        node *p = &o.head, *pp = &head, *q = nullptr;
        while (p->next != &o.tail) {
            q = new node(new T(*(p->next->data)), nullptr, pp);
            pp->next = q;
        }
        pp->next = &tail;
        tail.last = pp;

        return *this;
    }

    virtual void clear() {
        node *p(nullptr);
        while (head->next != &tail) {
            p = head->next;
            head->next = p->next;
            delete p->data;
            delete p;
        }
        init();
    }

    // bad, need to rewrite
    virtual void insert(size_t pos, const LinkList &L) {
        // insert a whole linklist after pos-th element
        // delete the original LinkList
        if (pos < 0 || pos > _size + 1)
            throw index_out_of_bound();

        if (L.empty())
            return;

        size_t cur = 0;
        node *p = &head, *q = nullptr;
        while (cur != pos) {
            pos++;
            p = p->next;
        }
        q = p->next;

        // merge left
        p->next = L.head.next;
        L.head.next->last = p;

        // merge right
        q->last = L.tail.last;
        L.tail.last->next = q;

        _size += L._size;

        L.init();
    }

    // bad, need to rewrite
    virtual void insert(size_t pos, T data) {
        // insert data after pos-th element
        if (pos < 0 || pos > _size + 1)
            throw index_out_of_bound();

        LinkList _list;
        node *p = new node(data, &_list.head, &_list.tail);
        _list.head.next = p;
        _list.tail.last = p;
        _list._size = 1;

        insert(pos, _list);
    }

    // unwritten
    virtual void erase() {
        // without parameter or methods yet
    }

    // really need it?
    virtual T &at(const size_t &pos) {
        if (pos < 0 || pos >= _size)
            throw index_out_of_bound();

        size_t cur = 0;
        node *p = head.next;
        while (cur < pos) {
            cur++;
            p = p->next;
        }
        return *p->data;
    }

    // really need it?
    virtual const T &at(const size_t &pos) const {
        if (pos < 0 || pos >= _size)
            throw index_out_of_bound();

        size_t cur = 0;
        node *p = head.next;
        while (cur < pos) {
            cur++;
            p = p->next;
        }
        return *p->data;
    }

    virtual const T &front() const {
        return *(head.next->data);
    }

    virtual const T &back() const {
        return *(tail.last->data);
    }
};

template <class T>
class deque : public LinkList<LinkList<T> > {
    static const int BLOCK = 500;

public:
	class const_iterator;
	class iterator {
	private:
	    deque *deque_address;
	    typename LinkList<LinkList<T> >::node *linklist_node;

	    typename LinkList<T>::node *p;

	public:
	    iterator() : deque_address(nullptr), linklist_node(nullptr), p(nullptr) {}

        iterator(deque *d_a, LinkList<T> *l_a, typename LinkList<T>::node *_p) {
	        deque_address = d_a;
	        linklist_node = l_a;
	        p = _p;
	    }

	    iterator(const iterator &o) {
            deque_address = o.deque_address;
            linklist_node = o.linklist_node;
            p = o.p;
	    }

		iterator operator+(const int &n) const {
	        if (n == 0)
	            return *this;
	        if (n < 0)
	            return *this - (-n);
            int pos = linklist_node->data->get_pos(p);

	        if (pos + n < linklist_node->data->_size) {
	            typename LinkList<T>::node *q = p;
	            int target_pos = pos + n;
	            while (pos != target_pos) {
	                q = q->next;
	                pos++;
	            }
	            return iterator(deque_address, linklist_node, q);
	        }

	        iterator next_block_1st(deque_address, linklist_node->next, linklist_node->next->data->head.next);
	        return next_block_1st + (n - (linklist_node->data->_size - pos));
		}

		iterator operator-(const int &n) const {
            if (n == 0)
                return *this;
            if (n < 0)
                return *this + (-n);
            int pos = linklist_node->data->get_pos(p);

            if (pos - n >= 0) {
                typename LinkList<T>::node *q = p;
                int target_pos = pos - n;
                while (pos != target_pos) {
                    q = q->last;
                    pos--;
                }
                return iterator(deque_address, linklist_node, q);
            }

            iterator last_block_end(deque_address, linklist_node->last, linklist_node->last->data->tail.last);
            return last_block_end - (n - pos - 1);
		}

		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &o) const {
	        if (deque_address != o.deque_address)
	            throw invalid_iterator();

			iterator a(*this), b(o);
			bool flag = false;
			int pos_a = deque_address->get_pos(linklist_node);
			int pos_b = deque_address->get_pos(o.linklist_node);

            if (pos_a == pos_b) {
                pos_a = linklist_node->data->get_pos(p);
                pos_b = linklist_node->data->get_pos(o.p);
                return pos_a - pos_b;
            }

			if (pos_a > pos_b) {
                swap(a, b);
                swap(pos_a, pos_b);
                flag = true;
            }

			pos_a = linklist_node->data->get_pos(p);
			pos_b = o.linklist_node->data->get_pos(o.p);
            int res = (linklist_node->data->_size - pos_a) + pos_b;

            typename LinkList<LinkList<T> >::node *pointer = linklist_node->next;
            while (pointer != o.linklist_node) {
                res += pointer->data->_size;
                pointer = pointer->next;
            }

            return flag ? -res : res;

		}

		iterator operator+=(const int &n) {
			//TODO
		}
		iterator operator-=(const int &n) {
			//TODO
		}
		iterator operator++(int) {}

		iterator& operator++() {}

		iterator operator--(int) {}

		iterator& operator--() {}

		T& operator*() const {}

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
    //// block size = _size

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
	~deque() = default;

	deque& operator=(const deque &o) {
	    if (this == &o)
	        return *this;

	    element_size = o.element_size;

	    LinkList<LinkList<T> >::operator=(o);
	    return *this;
	}

	// throw index_out_of_bound if out of bound.
	T& at(const size_t &pos) {
	    if (pos < 0 || pos >= element_size)
	        throw index_out_of_bound();

	    typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data->size()) {
            pos -= p->data->size();
            p = p->next;
        }
        return p->data->at(pos);
	}

	const T& at(const size_t &pos) const {
        if (pos < 0 || pos >= element_size)
            throw index_out_of_bound();

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data->size()) {
            pos -= p->data->size();
            p = p->next;
        }
        return p->data->at(pos);
	}

	T& operator[](const size_t &pos) {
	    return at(pos);
	}

	const T& operator[](const size_t &pos) const {
	    return at(pos);
	}

	// throw container_is_empty when the container is empty.
	const T& front() const {
	    if (empty())
	        throw container_is_empty();
	    return this->head.next->data->front();
	}

    // throw container_is_empty when the container is empty.
	const T& back() const {
        if (empty())
            throw container_is_empty();
        return this->tail.last->data->back();
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
