//
// Created by Михаил Терентьев on 10/06/2018.
//
#ifndef LIST_EWF_H
#define LIST_EWF_H

#include <type_traits>
#include <iterator>
#include <iostream>

template<typename T>
class List {
private:
    struct Neutral {
        Neutral *prev;
        Neutral *next;

        virtual ~Neutral() = default;

        Neutral(Neutral *prev, Neutral *next) : prev(prev), next(next) {}

        Neutral() : prev(nullptr), next(nullptr) {};
    };

    void free_circle(const Neutral &fake_node) {
        Neutral *n = fake_node.next->next, *cur = fake_node.next;
        while (cur != &fake_node) {
            delete cur;
            cur = n;
            n = n->next;
        }
    }

    struct Element : virtual Neutral {
        T value;

        ~Element() = default;

        Element(const T &data, Neutral *prev, Neutral *next) : Neutral(prev, next), value(data) {};
    };

    Neutral root;
    Neutral *root_ptr = &root;

public:
    void clear() {
        Neutral *cur = root_ptr->next;
        while (cur != root_ptr) {
            Neutral *to_del = cur;
            cur = cur->next;
            delete to_del;
        }
        root_ptr->next = root_ptr;
        root_ptr->prev = root_ptr;
    }

    bool empty() const {
        return root_ptr == root.next;
    }

    template<typename Z>
    struct List_iterator : public std::iterator<std::bidirectional_iterator_tag, Z> {
        friend class List<T>;

        List_iterator &operator++() {
            it_ptr = it_ptr->next;
            return *this;
        }

        template<typename C>
        List_iterator(const List_iterator<C> &other)
                : it_ptr(other.it_ptr) {
        }

        List_iterator &operator--() {
            it_ptr = it_ptr->prev;
            return *this;
        }

        List_iterator operator++(int) {
            List_iterator tmp(it_ptr);
            ++(*this);
            return tmp;
        }

        List_iterator operator--(int) {
            List_iterator tmp(it_ptr);
            --(*this);
            return tmp;
        }

        Z &operator*() const {
            return dynamic_cast<Element *>(it_ptr)->value;
        }

        //template <typename P>
        bool operator==(const List_iterator &rhs) const {
            return it_ptr == rhs.it_ptr;
        }

        //template <typename P>
        bool operator!=(const List_iterator &rhs) const {
            return it_ptr != rhs.it_ptr;
        }

//iterator 633:56
        typedef std::ptrdiff_t difference_type;
        typedef Z value_type;
        typedef Z *pointer;
        typedef Z &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

    private:
        explicit List_iterator(Neutral *cur) : it_ptr(cur) {};
        Neutral *it_ptr;
    };

    typedef List_iterator<T> iterator;
    typedef List_iterator<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    void friend swap(List<T> &frst, List<T> &sec) noexcept {
        std::swap(frst.root, sec.root);
        if (frst.root.prev == sec.root_ptr) {
            frst.root.prev = frst.root.next = &frst.root;
        }
        frst.root.prev->next = frst.root.next->prev = &frst.root;
        if (sec.root.prev == frst.root_ptr) {
            sec.root.prev = sec.root.next = &sec.root;
        }
        sec.root.prev->next = sec.root.next->prev = &sec.root;
    }

    T &get_value(Neutral *elmnt) const {
        return static_cast<Element *>(elmnt)->value;
    }

    List() {
        root.prev = root.next = root_ptr;
    };

    List(const List &other) : List() {
        Neutral *other_elem = other.root.next;
        while (other_elem != other.root_ptr) {
            push_back(dynamic_cast<Element *>(other_elem)->value);
            other_elem = other_elem->next;
        }
    }

    List &operator=(const List &other) {
        List tmp(other);
        swap(*this, tmp);
        return *this;
    }

    ~List() {
        clear();
    }

    T &back() {
        return *(--end());
    }

    const T &back() const {
        return *(--end());
    }

    const T &front() const {
        return *(begin());
    }

    T &front() {
        return *(begin());
    }

    void push_back(T const &elem) {
        auto x = new Element(elem, root.prev, root_ptr);
        root.prev->next = x;
        root.prev = x;
    }

    void push_front(T const &elem) {
        auto x = new Element(elem, root_ptr, root.next);
        root.next->prev = x;
        root.next = x;
    }

    void pop_back() {
        Neutral *trash = root.prev;
        root.prev->prev->next = root_ptr;
        root.prev = root.prev->prev;
        delete trash;
    }

    void pop_front() {
        Neutral *trash = root.next;
        root.next->next->prev = root_ptr;
        root.next = root.next->next;
        delete trash;
    }

    iterator begin() {
        return iterator(root.next);
    }

    const_iterator begin() const {
        return const_iterator(root.next);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    iterator end() {
        return iterator(root_ptr);
    }

    const_iterator end() const {
        return const_iterator(root_ptr);
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator splice(const_iterator pos, List &src_list, const_iterator begin_it, const_iterator end_it) {
        Neutral *tmp = end_it.it_ptr->prev->next;
        end_it.it_ptr->prev->next = pos.it_ptr->prev->next;
        pos.it_ptr->prev->next = begin_it.it_ptr->prev->next;
        begin_it.it_ptr->prev->next = tmp;
        tmp = pos.it_ptr->prev;
        pos.it_ptr->prev = end_it.it_ptr->prev;
        end_it.it_ptr->prev = begin_it.it_ptr->prev;
        begin_it.it_ptr->prev = tmp;
        return iterator(pos.it_ptr);
    }

    iterator insert(const_iterator it, const T &val) {
        auto *elmt = new Element(val, it.it_ptr->prev, it.it_ptr);
        it.it_ptr->prev->next = elmt;
        it.it_ptr->prev = elmt;
        return iterator(elmt);
    }

    iterator erase(const_iterator it) {
        iterator res = iterator(it.it_ptr->next);
        it.it_ptr->prev->next = it.it_ptr->next;
        it.it_ptr->next->prev = it.it_ptr->prev;
        delete it.it_ptr;
        return res;
    }

    iterator erase(const_iterator begin, const_iterator end) {
        auto erasing_root = new Neutral(end.it_ptr->prev, begin.it_ptr);
        begin.it_ptr->prev->next = end.it_ptr;
        end.it_ptr->prev->next = erasing_root;
        end.it_ptr->prev = begin.it_ptr->prev;
        begin.it_ptr->prev = erasing_root;
        free_circle(*erasing_root);
        delete erasing_root;
        return iterator(end.it_ptr);
    }
};

#endif //LIST_EWF_H
