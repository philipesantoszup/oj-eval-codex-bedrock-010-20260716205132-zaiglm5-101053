#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {

template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        
        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        
        node(const T &value, node *p = nullptr, node *n = nullptr) 
            : prev(p), next(n) {
            data = new T(value);
        }
        
        ~node() {
            if (data != nullptr) {
                delete data;
                data = nullptr;
            }
        }
        
        node(const node &other) : prev(nullptr), next(nullptr) {
            if (other.data != nullptr) {
                data = new T(*other.data);
            } else {
                data = nullptr;
            }
        }
    };

protected:
    node *head;  // sentinel head node
    node *tail;  // sentinel tail node
    size_t _size;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        ++_size;
        return cur;
    }
    
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        --_size;
        return pos;
    }

public:
    class const_iterator;
    
    class iterator {
    private:
        node *ptr;
        const list *container;

    public:
        iterator(node *p = nullptr, const list *c = nullptr) 
            : ptr(p), container(c) {}
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator & operator++() {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }
        
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        iterator & operator--() {
            if (ptr == nullptr || ptr->prev == container->head) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }
        
        T & operator *() const {
            if (ptr == nullptr || ptr == container->head || ptr == container->tail) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }
        
        T * operator ->() const {
            if (ptr == nullptr || ptr == container->head || ptr == container->tail) {
                throw invalid_iterator();
            }
            return ptr->data;
        }
        
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        friend class const_iterator;
        friend class list;
    };
    
    class const_iterator {
    private:
        const node *ptr;
        const list *container;

    public:
        const_iterator(const node *p = nullptr, const list *c = nullptr) 
            : ptr(p), container(c) {}
        
        const_iterator(const iterator &other) 
            : ptr(other.ptr), container(other.container) {}
        
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_iterator & operator++() {
            if (ptr == nullptr || ptr == container->tail) {
                throw invalid_iterator();
            }
            ptr = ptr->next;
            return *this;
        }
        
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        const_iterator & operator--() {
            if (ptr == nullptr || ptr->prev == container->head) {
                throw invalid_iterator();
            }
            ptr = ptr->prev;
            return *this;
        }
        
        const T & operator *() const {
            if (ptr == nullptr || ptr == container->head || ptr == container->tail) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }
        
        const T * operator ->() const {
            if (ptr == nullptr || ptr == container->head || ptr == container->tail) {
                throw invalid_iterator();
            }
            return ptr->data;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        friend class iterator;
        friend class list;
    };

    list() : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
    }
    
    list(const list &other) : _size(0) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        
        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
    }
    
    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }
    
    list &operator=(const list &other) {
        if (this == &other) return *this;
        
        clear();
        
        for (node *p = other.head->next; p != other.tail; p = p->next) {
            push_back(*(p->data));
        }
        
        return *this;
    }
    
    const T & front() const {
        if (empty()) throw container_is_empty();
        return *(head->next->data);
    }
    
    const T & back() const {
        if (empty()) throw container_is_empty();
        return *(tail->prev->data);
    }
    
    iterator begin() {
        return iterator(head->next, this);
    }
    
    const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }
    
    iterator end() {
        return iterator(tail, this);
    }
    
    const_iterator cend() const {
        return const_iterator(tail, this);
    }
    
    virtual bool empty() const {
        return _size == 0;
    }
    
    virtual size_t size() const {
        return _size;
    }

    virtual void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) throw invalid_iterator();
        node *newNode = new node(value);
        insert(pos.ptr, newNode);
        return iterator(newNode, this);
    }
    
    virtual iterator erase(iterator pos) {
        if (pos.container != this) throw invalid_iterator();
        if (empty()) throw container_is_empty();
        if (pos.ptr == tail) throw invalid_iterator();
        
        node *nextNode = pos.ptr->next;
        node *removed = erase(pos.ptr);
        delete removed;
        return iterator(nextNode, this);
    }
    
    void push_back(const T &value) {
        insert(tail, new node(value));
    }
    
    void pop_back() {
        if (empty()) throw container_is_empty();
        node *toDelete = tail->prev;
        erase(toDelete);
        delete toDelete;
    }
    
    void push_front(const T &value) {
        insert(head->next, new node(value));
    }
    
    void pop_front() {
        if (empty()) throw container_is_empty();
        node *toDelete = head->next;
        erase(toDelete);
        delete toDelete;
    }
    
    void sort() {
        if (_size <= 1) return;
        
        // Extract all data pointers into an array
        T **arr = new T*[_size];
        size_t idx = 0;
        for (node *p = head->next; p != tail; p = p->next) {
            arr[idx++] = p->data;
        }
        
        // Sort the array using sjtu::sort
        sjtu::sort<T*>(arr, arr + _size, [](const T* a, const T* b) -> bool {
            return *a < *b;
        });
        
        // Put sorted data pointers back into nodes
        idx = 0;
        for (node *p = head->next; p != tail; p = p->next) {
            p->data = arr[idx++];
        }
        
        delete[] arr;
    }
    
    void merge(list &other) {
        if (this == &other) return;
        if (other.empty()) return;
        
        node *p1 = head->next;
        node *p2 = other.head->next;
        
        while (p1 != tail && p2 != other.tail) {
            if (*(p2->data) < *(p1->data)) {
                // Move node from other before p1
                node *next2 = p2->next;
                other.erase(p2);
                insert(p1, p2);
                p2 = next2;
            } else {
                p1 = p1->next;
            }
        }
        
        // Move remaining nodes from other
        while (p2 != other.tail) {
            node *next2 = p2->next;
            other.erase(p2);
            insert(tail, p2);
            p2 = next2;
        }
    }
    
    void reverse() {
        if (_size <= 1) return;
        
        node *p = head->next;
        while (p != tail) {
            node *next = p->next;
            // Swap prev and next pointers
            node *tmp = p->prev;
            p->prev = p->next;
            p->next = tmp;
            p = next;
        }
        
        // Swap head and tail connections
        node *first = head->next;
        node *last = tail->prev;
        
        head->next = last;
        last->prev = head;
        
        tail->prev = first;
        first->next = tail;
    }
    
    void unique() {
        if (_size <= 1) return;
        
        node *p = head->next;
        while (p != tail && p->next != tail) {
            if (*(p->data) == *(p->next->data)) {
                node *toDelete = p->next;
                erase(toDelete);
                delete toDelete;
            } else {
                p = p->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
