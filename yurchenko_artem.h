#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <memory>
#include <experimental/optional>
#include <list>
#include <cassert>

template<typename T>
class List : public std::allocator<T> {
public:
    struct iterator;
    struct const_iterator;
    using value_type  = typename List<T>::value_type;
    using size_type = typename List<T>::size_type;
    using pointer = typename List<T>::pointer;
    using reference  = typename List<T>::reference;
    using const_reference  = typename List<T>::const_reference;
//    using const_iterator  = iterator const;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator const;

private:
    struct Node {
        std::experimental::optional<T> data = {};
        std::shared_ptr<Node> next = nullptr;
        std::shared_ptr<Node> prev = nullptr;

        static void link(std::shared_ptr<Node> a, std::shared_ptr<Node> b) {
            if (a) { a->next = b; }
            if (b) { b->prev = a; }
        }

        static std::shared_ptr<Node>
        insert(std::shared_ptr<Node> single, std::shared_ptr<Node> successor) {
            assert(successor != nullptr);
            assert(single != nullptr);

            auto prev_copy = successor->prev;
            link(prev_copy, single);
            link(single, successor);

            return single;
        }

        static std::shared_ptr<Node>
        erase(std::shared_ptr<Node> node) {
            link(node->prev, node->next);
            return node->next;
        }

        Node() = default;

        explicit Node(const_reference value) : data(value) {}
    };

public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, int> {
    private:
        std::shared_ptr<Node> base;

        iterator() noexcept : base(std::make_shared<Node>()) {};

        explicit iterator(const_reference value) : base(std::make_shared<Node>(value)) {}

        iterator(std::shared_ptr<Node> node) : base(node) {}

    public:

        iterator(const_iterator const &other) { base = other.base; }

        iterator &operator++() {
            base = base->next;
            return *this;
        }

        iterator const operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        iterator &operator--() {
            base = base->prev;
            return *this;
        }

        iterator const operator--(int) {
            auto result = *this;
            --*this;
            return result;
        }

        reference operator*() { return *base->data; }

        const_reference operator*() const { return *base->data; }

        bool operator==(iterator const &other) const {
            return base->data == other.base->data;
        }

        bool operator!=(iterator const &other) const {
            return not(*this == other);
        }

        friend List;
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, int> {
    private:
        std::shared_ptr<Node> base;

        const_iterator() noexcept : base(std::make_shared<Node>()) {};

        explicit const_iterator(const_reference value) : base(std::make_shared<Node>(value)) {}

        const_iterator(std::shared_ptr<Node> node) : base(node) {}

    public:

        const_iterator(iterator const &other) : base(other.base) {}

        const_iterator const &operator++() {
            base = base->next;
            return *this;
        }

        const_iterator const operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        const_iterator const &operator--() {
            base = base->prev;
            return *this;
        }

        const_iterator const operator--(int) {
            auto result = *this;
            --*this;
            return result;
        }

        const_reference operator*() const { return *base->data; }

        bool operator==(const_iterator other) const {
            return base->data == other.base->data;
        }

        bool operator!=(const_iterator other) const {
            return not(*this == other);
        }

        friend List;
    };

    iterator head = nullptr;
    iterator tail = nullptr;
    std::size_t size_ = 0;

public:
    List() noexcept : head(), tail(head.base), size_(0) {}

    List(std::initializer_list<T> list) : List() {
        insert(begin(), list.begin(), list.end());
    }

    reference back() { return *std::prev(end()); }

    reference front() { return *begin(); }

    const_reference back() const { return *std::prev(end()); }

    const_reference front() const { return *begin(); }

    iterator begin() { return head; }

    iterator end() { return tail; }

    const_iterator begin() const { return head; }

    const_iterator end() const { return tail; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return reverse_iterator(end()); }

    const_reverse_iterator rend() const { return reverse_iterator(begin()); }

    size_type size() const { return size_; }

    bool empty() const { return size_ == 0; }

    void clear() {
        size_ = 0;
        head = iterator();
        tail = head;
    }

    iterator insert(const_iterator pos, const_reference value) {
        if (head.base == nullptr) {
            head = iterator();
            tail = head;
        }
        ++size_;
        auto result = Node::insert(std::make_shared<Node>(value), pos.base);
        if (pos == begin()) { head = result; }
        return result;
    }

    template<typename It>
    iterator insert(const_iterator const_pos, It first, It last) {
        auto pos = const_pos;
        for (; first != last; ++first, ++pos) {
            pos = insert(pos, *first);
        }
        return pos;
    }

    iterator erase(const_iterator pos) {
        assert(not empty());

        --size_;

        auto result = Node::erase(pos.base);
        if (pos == begin()) {
            head = result;
        };
        return result;
    }

    iterator erase(const_iterator first, const_iterator last) {
        auto pos = first;
        while (pos != last) { pos = erase(pos); }
        return pos;
    }

    void splice(const_iterator const_pos, List &other, const_iterator first, const_iterator last_exclusive) {
        if (head.base == nullptr) {
            head = iterator();
            tail = head;
        }
        if (&other == this) {
            auto pos = const_pos;
            auto last_inclusive_base = last_exclusive.base->prev;
            Node::link(first.base->prev, last_exclusive.base);
            Node::link(pos.base->prev, first.base);
            Node::link(last_inclusive_base, pos.base);
            if (first == begin()) { head = last_exclusive; }
            if (const_pos == begin()) { head = first; }
        } else {
            insert(const_pos, first, last_exclusive);
        }
    }

    void push_back(const_reference value) { insert(end(), value); }

    void push_front(const_reference value) { insert(begin(), value); }

    void pop_back() { erase(std::prev(end())); }

    void pop_front() { erase(begin()); }

    void swap(List &other) {
        using std::swap;
        swap(head, other.head);
        swap(tail, other.tail);
        swap(size_, other.size_);
    }
};

template<typename T>
void swap(List<T> &a, List<T> &b) { a.swap(b); }


#endif //LIST_LIST_H
