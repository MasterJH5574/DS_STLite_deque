#ifndef LRH_LINKLIST
#define LRH_LINKLIST

#include "exceptions.hpp"
#include <cstddef>

namespace sjtu {

template <class T>
class LinkList {
public:
    struct node {
        T data;
        node *next, *last;
        node(T _d = T(), node *_n = nullptr, node *_l = nullptr) : data(_d), next(_n), last(_l) {}
    } head, tail;

    size_t _size;

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
            q = new node(p->next->data, nullptr, pp);
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

    LinkList& operator=(const LinkList &o) {
        if (this == &o)
            return *this;

        clear();
        if (o.empty())
            return *this;

        node *p = &o.head, *pp = &head, *q = nullptr;
        while (p->next != &o.tail) {
            q = new node(p->next->data, nullptr, pp);
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
            delete p;
        }
        init();
    }

    // bad
    virtual void insert(size_t pos, const LinkList &L) {
        // insert a whole linklist after pos-th element
        // delete the original LinkList
        if (pos < 0 || pos > _size + 1)
            throw(index_out_of_bound("index out of bound when insert a linklist into another linklist"));

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

    // bad
    virtual void insert(size_t pos, T data) {
        // insert data after pos-th element
        if (pos < 0 || pos > _size + 1)
            throw(index_out_of_bound("index out of bound when insert an element into a linklist"));

        LinkList _list;
        node *p = new node(data, &_list.head, &_list.tail);
        _list.head.next = p;
        _list.tail.last = p;
        _list._size = 1;

        insert(pos, _list);
    }

    virtual T& at(const size_t &pos) {
        if (pos < 0 || pos >= _size)
            throw(index_out_of_bound("index out of bound when accessing specified element"));

        size_t cur = 0;
        node *p = head.next;
        while (cur < pos) {
            cur++;
            p = p->next;
        }
        return p->data;
    }

    virtual const T& at(const size_t &pos) const {
        if (pos < 0 || pos >= _size)
            throw(index_out_of_bound("index out of bound when accessing specified element"));

        size_t cur = 0;
        node *p = head.next;
        while (cur < pos) {
            cur++;
            p = p->next;
        }
        return p->data;
    }

    virtual const T& front() const {
        return head.next->data;
    }

    virtual const T& back() const {
        return tail.last->data;
    }

    // unwritten
    virtual void erase() {
        // without parameter or methods yet
    }

    class const_iterator;
    class iterator {
    public:
        using pointer           = node *;
        using reference         = T &;
        using size_type         = size_t;
        using difference_type   = std::ptrdiff_t;

    private:
        pointer p;
        size_type pos;

    public:
        explicit iterator(pointer _p = nullptr, size_type _pos = 0) : p(_p), pos(_pos) {}
        iterator(const iterator &) = default;
        iterator &operator=(const iterator &) = default;

        iterator operator+(difference_type offset) {
            iterator res = *this;
            while (offset && res.p->next != nullptr) {
                offset--;
                res++;
            }
            if (offset)
                throw runtime_error("the resulting iterator is out of range");
            return res;
        }

        iterator operator-(difference_type offset) {
            iterator res = *this;
            while (offset && res.p->last->last != nullptr) {
                offset--;
                res--;
            }
            if (offset)
                throw runtime_error("the resulting iterator is out of range");
            return res;
        }

        difference_type operator-(const iterator &o) const {
            difference_type res = o.pos - pos;
            return res < 0 ? -res : res;
        }

        iterator &operator+=(difference_type offset) {
            while (offset && p->next != nullptr) {
                offset--;
                *this++;
            }
            if (offset)
                throw runtime_error("the resulting iterator is out of range");
            return *this;
        }

        iterator &operator-=(difference_type offset) {
            while (offset && p->last->last != nullptr) {
                offset--;
                *this--;
            }
            if (offset)
                throw runtime_error("the resulting iterator is out of range");
            return *this;
        }

        iterator &operator++() {
            pos++;
            p = p->next;
            if (p == nullptr)
                throw runtime_error("the resulting iterator is out of range");
            return *this;
        }

        iterator operator++(int) {
            iterator res = *this;
            pos++;
            p = p->next;
            if (p == nullptr)
                throw runtime_error("the resulting iterator is out of range");
            return res;
        }

        iterator &operator--() {
            pos--;
            p = p->last;
            if (p->last == nullptr)
                throw runtime_error("the resulting iterator is out of range");
            return *this;
        }

        iterator operator--(int) {
            iterator res = *this;
            pos--;
            p = p->last;
            if (p->last == nullptr)
                throw runtime_error("the resulting iterator is out of range");
            return res;
        }

        reference operator*() const {
            return p->data;
        }

        T *operator->() const {
            return &(p->data);
        }

        bool operator==(const iterator &o) const {
            return pos == o.pos;
        }
        bool operator==(const const_iterator &o) const {
            return pos == o.pos;
        }
        bool operator!=(const iterator &o) const {
            return !(*this == o);
        }
        bool operator!=(const const_iterator &o) const {
            return !(*this == o);
        }
    };
    class const_iterator {
    public:
        using pointer           = node *;
        using reference         = T &;
        using size_type         = size_t;
        using difference_type   = std::ptrdiff_t;

    private:
        const pointer p;
        const size_type pos;

    public:
        const_iterator() : p(nullptr), pos(0) {}
        const_iterator(const const_iterator &) = default;
        const_iterator &operator=(const const_iterator &) = default;

        bool operator==(const iterator &o) const {
            return pos == o.pos;
        }
        bool operator==(const const_iterator &o) const {
            return pos == o.pos;
        }
        bool operator!=(const iterator &o) const {
            return !(*this == o);
        }
        bool operator!=(const const_iterator &o) const {
            return !(*this == o);
        }
    };

    virtual iterator begin() {
        return iterator(head.next, 0);
    }
    virtual iterator end() {
        return iterator(&tail, _size);
    }
};

}

#endif