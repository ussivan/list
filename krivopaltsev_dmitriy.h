#ifndef EXAM_LIST_H
#define EXAM_LIST_H

#include <iterator>
#include <iostream>
#include <cassert>

template<typename T>
class list {
    struct node1 {
        node1 *next, *prev;

        node1() : next(nullptr), prev(nullptr) {}

        node1(node1 *prev, node1 *next) : next(next), prev(prev) {}
    } neutral_node;

    struct node : node1 {
        T value;

        node(const T &data) : node1(), value(data) {}

        node(const T &data, node1 *prev, node1 *next) : node1(prev, next), value(data) {}

        ~node() = default;
    };

    node1 *neutral = &neutral_node;
public:
    template<typename S>
    struct iterator1 {
        friend class list;

    private:
        node1 *temp;

        iterator1(node1 *data) : temp(data) {}

    public:
        template<typename C>
        iterator1(const iterator1<C> &data) {
            temp = data.temp;
        }

        S &operator*() {
            return ((node *) temp)->value;
        }

        iterator1 &operator++() {
            temp = temp->next;
            return *this;
        }

        iterator1 &operator--() {
            temp = temp->prev;
            return *this;
        }

        iterator1 operator++(int) {
            node1 *cur = temp;
            temp = temp->next;
            return iterator1(cur);
        }

        iterator1 operator--(int) {
            node1 *cur = temp;
            temp = temp->prev;
            return iterator1(cur);
        }

        bool operator==(iterator1 second) const {
            return temp == second.temp;
        }

        bool operator!=(iterator1 second) const {
            return temp != second.temp;
        }

        typedef std::ptrdiff_t difference_type;
        typedef S value_type;
        typedef S *pointer;
        typedef S &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

    };


    typedef iterator1<T> iterator;
    typedef iterator1<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    list() {
        neutral_node.next = neutral;
        neutral_node.prev = neutral;
    }

    list(const list &other) : list() {
        node1 *oth = other.neutral->next;
        while (oth != other.neutral) {
            push_back(((node *) oth)->value);
            oth = oth->next;
        }
    }

    ~list() {
        clear();
    }

    list &operator=(const list &other) {
        list oth(other);
//        swap(oth);
        swap(*this, oth);
        return *this;
    }

    void clear() {
        while (!empty()) {
            pop_back();
        }
    }

    bool empty() const {
        return neutral->prev == neutral;
    }

    void push_back(const T &data) {
        node *new_node_ptr = new node(data, neutral->prev, neutral);
//        node *new_node_ptr = &new_node;
        neutral->prev->next = new_node_ptr;
        neutral->prev = new_node_ptr;
    }

    void pop_back() {
        node *dlt = ((node *) neutral->prev);
        dlt->prev->next = neutral;
        neutral->prev = dlt->prev;
        delete dlt;
    }

    void push_front(const T &data) {
        node1 *new_node_ptr = new node(data, neutral, neutral->next);
//        node *new_node_ptr = &new_node;
        neutral->next->prev = new_node_ptr;
        neutral->next = new_node_ptr;
    }

    void pop_front() {
        node *dlt = ((node *) neutral->next);
        dlt->next->prev = neutral;
        neutral->next = dlt->next;
        delete dlt;
    }

    T &back() {
        return ((node *) neutral->prev)->value;
    }

    T &back() const {
        return ((node *) neutral->prev)->value;
    }

    T &front() {
        return ((node *) neutral->next)->value;
    }

    T &front() const {
        return ((node *) neutral->next)->value;
    }

    iterator begin() {
        return iterator(neutral->next);
    }

    const_iterator begin() const {
        return const_iterator(neutral->next);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    iterator end() {
        return iterator(neutral);
    }

    const_iterator end() const {
        return const_iterator(neutral);
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator pos, const T &data) {
        node *new_node_ptr = new node(data, pos.temp->prev, pos.temp);
//        node *new_node_ptr = &new_node;
        pos.temp->prev->next = new_node_ptr;
        pos.temp->prev = new_node_ptr;
        return iterator(new_node_ptr);
    }

    iterator erase(const_iterator pos) {
        iterator new_iter = iterator(pos.temp->next);
        pos.temp->prev->next = pos.temp->next;
        pos.temp->next->prev = pos.temp->prev;
        //assert(new_iter == ++pos);
        delete (node *) pos.temp;
        return new_iter;
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator iter = iterator(first.temp);
        while (last != iter) {
            iter = erase(iter);
        }
//        iter = erase(iter);
        return iter;
    }

    void splice(const_iterator pos, list &oth, const_iterator first, const_iterator last) {
        if (first == last)
            return;
        node1 *back = last.temp->prev;
        first.temp->prev->next = last.temp;
        last.temp->prev = first.temp->prev;

        node1 *cur = pos.temp->prev;
        pos.temp->prev = back;
        back->next = pos.temp;

        cur->next = first.temp;
        first.temp->prev = cur;
    }

    template<typename S>
    friend void swap(list<S> &first, list<S> &second);
};

template<typename T>
void swap(list<T> &first, list<T> &second) {
    bool f_empty = first.empty();
    bool s_empty = second.empty();
    std::swap(first.neutral_node, second.neutral_node);

    if (s_empty) {
        first.neutral_node.prev = &first.neutral_node;
        first.neutral_node.next = &first.neutral_node;
    }

    if (f_empty) {
        second.neutral_node.prev = &second.neutral_node;
        second.neutral_node.next = &second.neutral_node;
    }

    first.neutral->prev->next = &first.neutral_node;
    second.neutral->prev->next = &second.neutral_node;
    first.neutral->next->prev = &first.neutral_node;
    second.neutral->next->prev = &second.neutral_node;

}

#endif //EXAM_LIST_H
