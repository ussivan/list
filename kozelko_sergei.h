//
// Created by rsbat on 6/10/18.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <memory>
#include <cassert>

template <typename T>
class list {
    struct node {
        std::unique_ptr<T> data; // node owns object
        std::unique_ptr<node> next; // node owns next node
        node* prev; // always valid because we are owned by someone

        node() : data(nullptr), next(nullptr), prev(nullptr) {};
        explicit node(const T& data) : data(std::make_unique<T>(data)), next(nullptr), prev(nullptr) {};
    };

    /* for c++ < 17
    template <bool b, typename T1, typename T2>
    struct template_if {};
    template <typename T1, typename T2>
    struct template_if<true, T1, T2> { using type = T1; };
    template <typename T1, typename T2>
    struct template_if<false, T1, T2> { using type = T2; };
    */

    template <bool is_const>
    class iterator_impl {
        node* ptr;

        explicit iterator_impl(node* ptr) : ptr(ptr) {}

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = long long;
        using pointer = typename std::conditional<is_const, const T*, T*>::type;
        using reference = typename std::conditional<is_const, const T&, T&>::type;
        using value_type = T;

        friend list;

        iterator_impl() = default;

        template<bool is_other_const, typename  U = std::enable_if_t<is_const || !is_other_const>>
        iterator_impl(const iterator_impl<is_other_const>& other) :ptr(other.ptr) {}

        template<bool is_other_const>
        std::enable_if_t<is_const || !is_other_const, iterator_impl&> operator=(const iterator_impl<is_other_const>& other) {
            ptr = other.ptr;
            return *this;
        }

        iterator_impl operator++(int) {
            iterator_impl tmp = *this;
            ptr = (ptr->next).get();
            return tmp;
        }

        iterator_impl& operator++() {
            ptr = (ptr->next).get();
            return *this;
        }

        iterator_impl operator--(int) {
            iterator_impl tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        iterator_impl operator--() {
            ptr = ptr->prev;
            return *this;
        }

        reference operator*() const {
            return *(ptr->data);
        }

        pointer operator->() const {
            return (ptr->data).get();
        }

        template <bool is_other_const>
        bool operator==(const iterator_impl<is_other_const>& b) const {
            return ptr == b.ptr;
        }

        template <bool is_other_const>
        bool operator!=(const iterator_impl<is_other_const>& b) const {
            return !(*this == b);
        }
    };

    mutable std::unique_ptr<node> head;
    mutable node* tail;

    void insert(node* nd, const T& val) {
        std::unique_ptr<node> tmp_node = std::make_unique<node>(val);
        node* last = nd->prev;
        tmp_node->next = std::move(last->next);
        tmp_node->prev = last;
        last->next = std::move(tmp_node);
        nd->prev = last->next.get();
    }

    void init_empty() const {
        assert(tail == nullptr);
        head = std::make_unique<node>();
        tail = head.get();
    }

public:
    using iterator = iterator_impl<false>;
    using const_iterator = iterator_impl<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list() noexcept : head(nullptr), tail(nullptr) {}

    list(const list<T>& other) :list() {
        for (const auto& val : other) { this->push_back(val); }
    }

    list& operator=(const list<T>& other) {
        list cpy(other);
        swap(cpy);
        return *this;
    }

    void push_back(const T& data) {
        if (tail == nullptr) { init_empty(); }

        if (empty()) { // list is empty
            std::unique_ptr<node> tmp_node = std::make_unique<node>(data);
            tmp_node->next = std::move(head);
            head = std::move(tmp_node);
            tail->prev = head.get();
        } else { // we have elements
            insert(tail, data);
        }
    }

    void pop_back() {
        if (empty()) { return; } // same as if (tail == nullptr || tail->prev == nullptr) { return; }

        tail->prev = tail->prev->prev;

        if (!empty()) {
            tail->prev->next = std::move(tail->prev->next->next);
        }
    }

    T& back() {
        assert(!empty()); // same as assert(tail != nullptr && tail->prev != nullptr);
        return *(tail->prev->data);
    }

    const T& back() const {
        assert(!empty()); // same as assert(tail != nullptr && tail->prev != nullptr);
        return *(tail->prev->data);
    }

    void push_front(const T& data) {
        if (tail == nullptr) { init_empty(); }

        std::unique_ptr<node> tmp_node = std::make_unique<node>(data);
        if (empty()) { // list is empty
            tmp_node->next = std::move(head);
            head = std::move(tmp_node);
            tail->prev = head.get();
        } else { // we have elements
            tmp_node->next = std::move(head);
            head = std::move(tmp_node);
            head->next->prev = head.get();
        }
    }

    void pop_front() {
        if (empty()) { return; }

        head = std::move(head->next);
        head->prev = nullptr;

        if (empty()) {
            tail->prev = nullptr;
        }
    }

    T& front() {
        assert(!empty());
        return *(head->data);
    }

    const T& front() const {
        assert(!empty());
        return *(head->data);
    }

    bool empty() const {
        return tail == nullptr || tail->prev == nullptr;
    }


    void clear() {
        if (empty()) { return; }
        head = std::move(tail->prev->next);
        tail->prev = nullptr;
    }

    iterator begin() const {
        if (tail == nullptr) { init_empty(); }
        return iterator(head.get());
    }

    iterator end() const {
        if (tail == nullptr) { init_empty(); }
        return iterator(tail);
    }

    const_iterator cbegin() const {
        if (tail == nullptr) { init_empty(); }
        return const_iterator(head.get());
    }

    const_iterator cend() const {
        if (tail == nullptr) { init_empty(); }
        return const_iterator(tail);
    }

    reverse_iterator rbegin() const {
        if (tail == nullptr) { init_empty(); }
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() const {
        if (tail == nullptr) { init_empty(); }
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        if (tail == nullptr) { init_empty(); }
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        if (tail == nullptr) { init_empty(); }
        return std::make_reverse_iterator(cbegin());
    }

    iterator insert(const_iterator pos, const T& val) {
        if (tail == nullptr) { init_empty(); }

        node* ptr = pos.ptr;

        if (empty()) {
            push_back(val);
        } else if (ptr->prev == nullptr) {
            push_front(val);
        } else {
            insert(ptr, val);
        }
        return iterator(ptr->prev);
    }

    iterator erase(const_iterator pos) {
        assert(!empty());

        node* ptr = pos.ptr;
        if (ptr->prev == nullptr) {
            pop_front();
            return begin();
        } else {
            node* nxt = ptr->next.get();
            ptr->next->prev = ptr->prev;
            ptr->prev->next = std::move(ptr->next); // ptr is deleted here!
            return iterator(nxt);
        }
    }

    iterator erase(const_iterator first, const_iterator second) {
        while (first != second) { // can be optimized
            first = erase(first);
        }

        return iterator(first.ptr);
    }

    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) {
        if (first == last) { return; }

        std::unique_ptr<node> tmp;
        node* ptr_last = last.ptr->prev;
        if (first.ptr->prev == nullptr) {
            tmp = std::move(other.head);
            other.head = std::move(last.ptr->prev->next);
            last.ptr->prev = nullptr;
        } else {
            tmp = std::move(first.ptr->prev->next);
            first.ptr->prev->next = std::move(last.ptr->prev->next);
            last.ptr->prev = first.ptr->prev;
            tmp->prev = nullptr;
        }

        if (pos.ptr->prev == nullptr) {
            head->prev = ptr_last;
            ptr_last->next = std::move(head);
            head = std::move(tmp);
            head->prev = nullptr;
        } else {
            ptr_last->next = std::move(pos.ptr->prev->next);
            tmp->prev = ptr_last->next->prev;
            pos.ptr->prev->next = std::move(tmp);
            ptr_last->next->prev = ptr_last;
        }
    }

    void swap(list& other) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
    }
};

template <typename T>
void swap(list<T>& a, list<T>& b) {
    a.swap(b);
}

#endif //LIST_LIST_H
