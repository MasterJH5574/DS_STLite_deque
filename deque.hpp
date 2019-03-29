#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <unordered_set>
#include <cassert>

namespace sjtu {

template <class T>
void swap(T &a, T &b) {
    T c = a;
    a = b, b = c;
}

template<class T>
class LinkList {
public:
    static const size_t BLOCK_FULL_SIZE = 500;

    struct node {
        T *data;
        node *next, *last;
        explicit node(T *d = nullptr, node *n = nullptr, node *l = nullptr) : data(d), next(n), last(l) {}
    } head, tail;

    size_t _size;

    int get_pos(const node *x) const {
        int res = 0;
        node *p = head.next;
        while (p != x) {
            p = p->next;
            res++;
        }

        return res;
    }

    bool check_exist(const node* x) const {
        node* p = head.next;
        while (p != &tail) {
            if (p == x)
                return true;
            p = p->next;
        }
        return false;
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

        const node *p = &o.head;
        node *pp = &head, *q = nullptr;
        while (p->next != &o.tail) {
            q = new node(new T(*(p->next->data)), nullptr, pp);
            pp->next = q;

            p = p->next, pp = pp->next;
        }
        pp->next = &tail;
        tail.last = pp;
    }

    virtual ~LinkList() {
        clear();
    }

    virtual bool full() const {
        return _size == BLOCK_FULL_SIZE;
    }

    virtual bool empty() const {
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

        const node *p = &o.head;
        node *pp = &head, *q = nullptr;
        while (p->next != &o.tail) {
            q = new node(new T(*(p->next->data)), nullptr, pp);
            pp->next = q;

            p = p->next, pp = pp->next;
        }
        pp->next = &tail;
        tail.last = pp;

        return *this;
    }

    virtual void clear() {
        node *p(nullptr);
        while (head.next != &tail) {
            p = head.next;
            head.next = p->next;
            delete p->data;
            delete p;
        }
        init();
    }

    virtual node *insert(node *pos, const T &v) {
        node *p = new node(new T(v), pos, pos->last);
        pos->last->next = p;
        pos->last = p;
        _size++;
        return p;
    }

    virtual void erase(node *pos) {
        _size--;
        pos->last->next = pos->next;
        pos->next->last = pos->last;
        delete pos->data;
        delete pos;
    }

    // really need it?
    virtual T &At(const size_t &pos) {
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
    virtual const T &At(const size_t &pos) const {
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
};

template <class T>
class deque : public LinkList<LinkList<T> > {

public:
	class const_iterator;
	class iterator {
	public:
	    deque *deque_address;
	    typename LinkList<LinkList<T> >::node *linklist_node;
	    typename LinkList<T>::node *p;

	public:
        iterator() : deque_address(nullptr), linklist_node(nullptr), p(nullptr) {}

        iterator(deque *d_a, typename LinkList<LinkList<T> >::node *l_a, typename LinkList<T>::node *_p) {
	        deque_address = d_a;
	        linklist_node = l_a;
	        p = _p;
	    }

	    iterator(const iterator &o) {
            deque_address = o.deque_address;
            linklist_node = o.linklist_node;
            p = o.p;
	    }

        iterator(const const_iterator &o) {
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

	        if (pos + n < (int)linklist_node->data->_size) {
	            typename LinkList<T>::node *q = p;
	            int target_pos = pos + n;
	            while (pos != target_pos) {
	                q = q->next;
	                pos++;
	            }
	            return iterator(deque_address, linklist_node, q);
	        }

            iterator end_iterator = deque_address->end();
            if (linklist_node->next == end_iterator.linklist_node)
                return end_iterator;
            else {
                iterator next_block_1st(deque_address, linklist_node->next, linklist_node->next->data->head.next);
                return next_block_1st + (n - (linklist_node->data->_size - pos));
            }
		}

		iterator operator-(const int &n) const {
            if (n == 0)
                return *this;
            if (n < 0)
                return *this + (-n);

            iterator end_iterator = deque_address->end();
            if (*this == end_iterator)
                return iterator(deque_address, linklist_node->last, linklist_node->last->data->tail.last) - (n - 1);

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

            if (*this == o)
                return 0;

            if (*this == deque_address->end())
                return ((*this - 1) - o) + 1;
            else if (o == deque_address->end())
                return (*this - (o - 1)) - 1;

            iterator a(*this), b(o);
            int pos_a = deque_address->get_pos(linklist_node);
            int pos_b = deque_address->get_pos(o.linklist_node);

            if (pos_a == pos_b) {
                pos_a = linklist_node->data->get_pos(p);
                pos_b = linklist_node->data->get_pos(o.p);
                return pos_a - pos_b;
            }

            if (pos_a < pos_b)
                return o - *this;

            pos_a = linklist_node->data->get_pos(p);
            pos_b = o.linklist_node->data->get_pos(o.p);
            int res = (int)(pos_a + (o.linklist_node->data->_size - pos_b));

            typename LinkList<LinkList<T> >::node *pointer = o.linklist_node->next;
            while (pointer != linklist_node) {
                res += pointer->data->_size;
                pointer = pointer->next;
            }

            return res;
        }


        int operator-(const const_iterator &o) const {
            if (deque_address != o.deque_address)
                throw invalid_iterator();

            if (*this == o)
                return 0;

            if (*this == deque_address->end())
                return ((*this - 1) - o) + 1;
            else if (o == deque_address->end())
                return (*this - (o - 1)) - 1;

            iterator a(*this), b(o);
            int pos_a = deque_address->get_pos(linklist_node);
            int pos_b = deque_address->get_pos(o.linklist_node);

            if (pos_a == pos_b) {
                pos_a = linklist_node->data->get_pos(p);
                pos_b = linklist_node->data->get_pos(o.p);
                return pos_a - pos_b;
            }

            if (pos_a < pos_b)
                return o - *this;

            pos_a = linklist_node->data->get_pos(p);
            pos_b = o.linklist_node->data->get_pos(o.p);
            int res = (int)(pos_a + (o.linklist_node->data->_size - pos_b));

            typename LinkList<LinkList<T> >::node *pointer = o.linklist_node->next;
            while (pointer != linklist_node) {
                res += pointer->data->_size;
                pointer = pointer->next;
            }

            return res;
        }

		iterator &operator+=(const int &n) {
            *this = *this + n;
            return *this;
		}

		iterator &operator-=(const int &n) {
            *this = *this - n;
            return *this;
		}

		iterator operator++(int) {
            iterator res = *this;
            *this = *this + 1;
            return res;
        }

		iterator& operator++() {
            *this = *this + 1;
            return *this;
        }

		iterator operator--(int) {
            iterator res = *this;
            *this = *this - 1;
            return res;
        }

		iterator& operator--() {
            *this = *this - 1;
            return *this;
        }

        T& operator*() const {
            if (*this == deque_address->end())
                throw invalid_iterator();
            return *this->p->data;
        }

        T* operator->() const noexcept {
            if (*this == deque_address->end())
                throw invalid_iterator();
            return &(*this->p->data);
        }

        bool operator==(const iterator &o) const {
		    return p == o.p;
		}

		bool operator==(const const_iterator &o) const {
            return p == o.p;
        }

		bool operator!=(const iterator &o) const {
            return p != o.p;
        }

		bool operator!=(const const_iterator &o) const {
            return p != o.p;
        }
	};

	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.

	public:
        const deque *deque_address;
        const typename LinkList<LinkList<T> >::node *linklist_node;
        const typename LinkList<T>::node *p;

	public:
	    const_iterator() : deque_address(nullptr), linklist_node(nullptr), p(nullptr) {}

        const_iterator(const deque *d_a, const typename LinkList<LinkList<T> >::node *l_a, const typename LinkList<T>::node *_p) {
            deque_address = d_a;
            linklist_node = l_a;
            p = _p;
        }

	    const_iterator(const const_iterator &o) {
            deque_address = o.deque_address;
            linklist_node = o.linklist_node;
            p = o.p;
	    }

	    const_iterator(const iterator &o) {
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

            if (pos + n < (int)linklist_node->data->_size) {
                typename LinkList<T>::node *q = p;
                int target_pos = pos + n;
                while (pos != target_pos) {
                    q = q->next;
                    pos++;
                }
                return iterator(deque_address, linklist_node, q);
            }

            iterator end_iterator = deque_address->end();
            if (linklist_node->next == end_iterator.linklist_node)
                return end_iterator;
            else {
                iterator next_block_1st(deque_address, linklist_node->next, linklist_node->next->data->head.next);
                return next_block_1st + (n - (linklist_node->data->_size - pos));
            }
        }

        iterator operator-(const int &n) const {
            if (n == 0)
                return *this;
            if (n < 0)
                return *this + (-n);

            iterator end_iterator = deque_address->end();
            if (*this == end_iterator)
                return iterator(deque_address, linklist_node->last, linklist_node->last->data->tail.last) - (n - 1);

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

            if (*this == o)
                return 0;

            if (*this == deque_address->end())
                return ((*this - 1) - o) + 1;
            else if (o == deque_address->end())
                return (*this - (o - 1)) - 1;

            iterator a(*this), b(o);
            int pos_a = deque_address->get_pos(linklist_node);
            int pos_b = deque_address->get_pos(o.linklist_node);

            if (pos_a == pos_b) {
                pos_a = linklist_node->data->get_pos(p);
                pos_b = linklist_node->data->get_pos(o.p);
                return pos_a - pos_b;
            }

            if (pos_a < pos_b)
                return o - *this;

            pos_a = linklist_node->data->get_pos(p);
            pos_b = o.linklist_node->data->get_pos(o.p);
            int res = (int)(pos_a + (o.linklist_node->data->_size - pos_b));

            typename LinkList<LinkList<T> >::node *pointer = o.linklist_node->next;
            while (pointer != linklist_node) {
                res += pointer->data->_size;
                pointer = pointer->next;
            }

            return res;
        }

        int operator-(const const_iterator &o) const {
            if (deque_address != o.deque_address)
                throw invalid_iterator();

            if (*this == o)
                return 0;

            if (*this == deque_address->end())
                return ((*this - 1) - o) + 1;
            else if (o == deque_address->end())
                return (*this - (o - 1)) - 1;

            iterator a(*this), b(o);
            int pos_a = deque_address->get_pos(linklist_node);
            int pos_b = deque_address->get_pos(o.linklist_node);

            if (pos_a == pos_b) {
                pos_a = linklist_node->data->get_pos(p);
                pos_b = linklist_node->data->get_pos(o.p);
                return pos_a - pos_b;
            }

            if (pos_a < pos_b)
                return o - *this;

            pos_a = linklist_node->data->get_pos(p);
            pos_b = o.linklist_node->data->get_pos(o.p);
            int res = (int)(pos_a + (o.linklist_node->data->_size - pos_b));

            typename LinkList<LinkList<T> >::node *pointer = o.linklist_node->next;
            while (pointer != linklist_node) {
                res += pointer->data->_size;
                pointer = pointer->next;
            }

            return res;
        }

        const_iterator &operator+=(const int &n) {
            *this = *this + n;
            return *this;
        }

        const_iterator &operator-=(const int &n) {
            *this = *this - n;
            return *this;
        }

        const_iterator operator++(int) {
            iterator res = *this;
            *this = *this + 1;
            return res;
        }

        const_iterator& operator++() {
            *this = *this + 1;
            return *this;
        }

        const_iterator operator--(int) {
            iterator res = *this;
            *this = *this - 1;
            return res;
        }

        const_iterator& operator--() {
            *this = *this - 1;
            return *this;
        }

        T& operator*() const {
            return *this->p->data;
        }

        T* operator->() const noexcept {
            return &(*this->p->data);
        }

        bool operator==(const iterator &o) const {
            return p == o.p;
        }

        bool operator==(const const_iterator &o) const {
            return p == o.p;
        }

        bool operator!=(const iterator &o) const {
            return p != o.p;
        }

        bool operator!=(const const_iterator &o) const {
            return p != o.p;
        }
	};

public:
    // the number of elements in this deque
    size_t element_size;
    //// block size = _size


public:

	deque() : LinkList<LinkList<T> >(), element_size(0) { }

	deque(const deque &o) : LinkList<LinkList<T> >(o), element_size(o.element_size) {}

	~deque() = default;

	deque& operator=(const deque &o) {
	    if (this == &o)
	        return *this;

	    LinkList<LinkList<T> >::operator=(o);
        element_size = o.element_size;
	    return *this;
	}

	// throw index_out_of_bound if out of bound.
	T& at(size_t pos) {
	    if (pos < 0 || pos >= element_size)
	        throw index_out_of_bound();

	    typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data->_size) {
            pos -= p->data->_size;
            p = p->next;
        }
        return p->data->At(pos);
	}

	const T& at(size_t pos) const {
        if (pos < 0 || pos >= element_size)
            throw index_out_of_bound();

        typename LinkList<LinkList<T> >::node *p = this->head.next;
        while (pos >= p->data->_size) {
            pos -= p->data->_size;
            p = p->next;
        }
        return p->data->At(pos);
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
	    return *cbegin();
	}

    // throw container_is_empty when the container is empty.
	const T& back() const {
        if (empty())
            throw container_is_empty();
        return *(this->tail.last->data->tail.last->data);
	}

	iterator begin() {
        if (empty())
            return end();
        else
	        return iterator(this, this->head.next, this->head.next->data->head.next);
	}

	const_iterator cbegin() const {
        if (empty())
            return cend();
        else
	        return const_iterator(this, this->head.next, this->head.next->data->head.next);
	}

	iterator end() {
	    //return iterator(this, &this->tail, &(this->tail.data->head));
        return iterator(this, &this->tail, nullptr);
	}

	const_iterator cend() const {
        return const_iterator(this, &this->tail, nullptr);
        //return const_iterator(this, &this->tail, &(this->tail.data->head));
	}

	bool empty() const {
	    return element_size == 0;
	}

	size_t size() const {
	    return element_size;
	}

	void clear() {
	    LinkList<LinkList <T> >::clear();
	    element_size = 0;
	}

    typename LinkList<LinkList<T> >::node *split(typename LinkList<LinkList<T> >::node *o, typename LinkList<T>::node *q) {
	    int cnt = 0;
	    bool flag = false;

	    typename LinkList<T>::node *p = o->data->head.next;
	    while (cnt != LinkList<LinkList<T> >::BLOCK_FULL_SIZE >> 1) {
	        if (p == q)
	            flag = true;
            cnt++;
            p = p->next;
        }

	    auto *a = new typename LinkList<LinkList<T> >::node(new LinkList<T>);
        auto *b = new typename LinkList<LinkList<T> >::node(new LinkList<T>);
        a->last = o->last, b->next = o->next;
        a->next = b, b->last = a;
        o->last->next = a, o->next->last = b;

        a->data->_size = LinkList<LinkList<T> >::BLOCK_FULL_SIZE >> 1;
        b->data->_size = LinkList<LinkList<T> >::BLOCK_FULL_SIZE >> 1;

        o->data->head.next->last = &a->data->head;
        p->last->next = &a->data->tail;
        a->data->head.next = o->data->head.next;
        a->data->tail.last = p->last;

        o->data->tail.last->next = &b->data->tail;
        p->last = &b->data->head;
        b->data->tail.last = o->data->tail.last;
        b->data->head.next = p;

        o->data->head.next = &o->data->tail;
        o->data->tail.last = &o->data->head;

        delete o->data;
        delete o;

        return flag ? a : b;
	}

	typename LinkList<LinkList<T> >::node *merge(typename LinkList<LinkList<T> >::node *a, typename LinkList<LinkList<T> >::node *b) {
	    auto *res = new typename LinkList<LinkList<T> >::node(new LinkList<T>, b->next, a->last);
        res->data->_size = a->data->_size + b->data->_size;

	    a->last->next = res;
	    b->next->last = res;

	    a->data->head.next->last = &res->data->head;
	    a->data->tail.last->next = b->data->head.next;

	    b->data->tail.last->next = &res->data->tail;
	    b->data->head.next->last = a->data->tail.last;

	    res->data->head.next = a->data->head.next;
	    res->data->tail.last = b->data->tail.last;

	    a->data->head.next = &a->data->tail;
	    a->data->tail.last = &a->data->head;
        b->data->head.next = &b->data->tail;
        b->data->tail.last = &b->data->head;

        delete a->data;
        delete b->data;
        delete a;
        delete b;

        return res;
	}

	// inserts value before pos
	// returns an iterator pointing to the inserted value
	iterator insert(const iterator &pos, const T &v) {
	    if (pos == end()) {
            if (element_size == 0) { // also block_size == 0
                element_size++;
                assert(LinkList<LinkList<T> >::_size == 0);
                //typename LinkList<LinkList<T> >::node *p = new typename LinkList<LinkList<T> >::node(new LinkList<T>, &this->tail, &this->head);
                auto *p = new typename LinkList<LinkList<T> >::node(new LinkList<T>, &this->tail, &this->head);
                this->head.next = p;
                this->tail.last = p;

                typename LinkList<T>::node *q = p->data->insert(&p->data->tail, v);
                return iterator(this, p, q);
            } else {
                element_size++;
                typename LinkList<T>::node *q = this->tail.last->data->insert(&this->tail.last->data->tail, v);
                typename LinkList<LinkList<T> >::node *p = this->tail.last;
                if (this->tail.last->data->full())
                    p = split(this->tail.last, q);

                return iterator(this, p, q);
            }
	    }


	    if (pos.deque_address != this)
	        throw invalid_iterator();
	    if (!this->check_exist(pos.linklist_node))
	        throw invalid_iterator();
	    if (!pos.linklist_node->data->check_exist(pos.p))
	        throw invalid_iterator();

        element_size++;
        typename LinkList<T>::node *q = pos.linklist_node->data->insert(pos.p, v);
        typename LinkList<LinkList<T> >::node *p = pos.linklist_node;
        if (p->data->full())
            p = split(p, q);

        return iterator(this, p, q);
	}

	// removes the element at pos.
	// returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	// throw if the container is empty, the iterator is invalid or it points to a wrong place.
	iterator erase(const iterator &pos) {
        if (pos.deque_address != this)
            throw invalid_iterator();
        if (!this->check_exist(pos.linklist_node))
            throw invalid_iterator();
        if (!pos.linklist_node->data->check_exist(pos.p))
            throw invalid_iterator();

        element_size--;
        iterator next_pos = pos + 1;
        typename LinkList<T>::node *p = next_pos.p;
        pos.linklist_node->data->erase(pos.p);

        typename LinkList<LinkList<T> >::node *t = nullptr;
        if (pos.linklist_node->next != &this->tail && pos.linklist_node->data->_size + pos.linklist_node->next->data->_size <= (LinkList<LinkList<T> >::BLOCK_FULL_SIZE << 1) / 3)
            t = merge(pos.linklist_node, pos.linklist_node->next);
        else if (pos.linklist_node->last != &this->head && pos.linklist_node->data->_size + pos.linklist_node->last->data->_size <= (LinkList<LinkList<T> >::BLOCK_FULL_SIZE << 1) / 3)
            t = merge(pos.linklist_node->last, pos.linklist_node);
        else
            t = pos.linklist_node;

        if (next_pos == end())
            return end();
        else if (t->data->check_exist(p))
            return iterator(this, t, p);
        else {
            assert(t->next->data->check_exist(p));
            return iterator(this, t->next, p);
        }
    }

	void push_back(const T &v) {
        insert(end(), v);
	}

	void pop_back() {
	    erase(end() - 1);
	}

	void push_front(const T &v) {
        insert(begin(), v);
	}

	void pop_front() {
	    erase(begin());
	}
};

}

#endif
