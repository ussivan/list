#pragma once

#include <iterator>

template <typename T>
struct list {

private:

    struct node {
        node *left;
        node *right;

        node(node *left, node *right) : left(left), right(right) {};
        node() : left(this), right(this) {};
    };

    struct fullnode : node {
        T val;
        fullnode(T const& value, node *left, node *right) : node(left, right), val(value) {};
    };

    template <typename V>
    struct myiterator : std::iterator<std::bidirectional_iterator_tag, V> {
        friend struct list;
    public:
        node* cur;

        myiterator() = default;
        myiterator(myiterator const& other) : cur(other.cur) {};
        myiterator& operator++() {
            cur = cur->right;
            return *this;
        }

        operator myiterator<V const>() const noexcept {
            return myiterator<V const>(cur);
        }

        const myiterator operator++(int) {
            myiterator<V> copy(*this);
            ++*this;
            return copy;
        }

        myiterator& operator--() {
            cur = cur->left;
            return *this;
        }

        const myiterator operator--(int) {
            myiterator<V> copy(*this);
            --*this;
            return copy;
        }
        V& operator*() const { return static_cast<fullnode*>(cur)->val; }

        V* operator->() const { return &static_cast<fullnode*>(cur)->val; }

        template <typename U>
        bool operator==(myiterator<U> const& other) const {
            return cur == other.cur;
        }
        template <typename U>
        bool operator!=(myiterator<U> const& other) const {
            return cur != other.cur;
        }

    private:
        explicit myiterator(node* n) : cur(n) {};
    };

    node fake;

public:
    using iterator = myiterator<T>;
    using const_iterator = myiterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list();
    list(list const&);
    list& operator=(list const&);
    ~list();

    void clear();
    bool empty();

    void push_back(T const& val);
    void pop_back();
    T& back();
    T const& back() const;

    void push_front(T const& val);
    void pop_front();
    T& front();
    T const& front() const;

    iterator begin() {
        return iterator(fake.right);
    }
    const_iterator begin() const {
        return const_iterator(fake.right);
    }

    iterator end() {
        return iterator(&fake);
    }
    const_iterator end() const {
        return const_iterator(const_cast<node*>(&fake));
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator pos, T const& val) {
        fullnode * n = new fullnode(val, pos.cur->left, pos.cur);
        pos.cur->left = n;
        n->left->right = n;
        return iterator(n);
    }
    iterator erase(const_iterator pos) {
        node* n = pos.cur;
        n->right->left = n->left;
        n->left->right = n->right;
        iterator ans(n->right);
        delete static_cast<fullnode*>(n);
        return ans;
    }
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);

    void swap(list& other);

    friend void swap(list& a, list& b) {
        a.swap(b);
    }
};

template<typename T>
list<T>::list() = default;

template<typename T>
list<T>::list(list const & other) : list() {
    for(T const &v : other) {
        push_back(v);
    }
}

template<typename T>
list<T>::~list() {
    clear();
}

template<typename T>
void list<T>::clear() {
    node* cur = fake.right;
    while (cur != &fake) {
        node* to_del = cur;
        cur = cur->right;
        delete static_cast<fullnode*>(to_del);
    }
    fake.right = fake.left = &fake;
}

template<typename T>
bool list<T>::empty() {
    return fake.right == &fake;
}

template<typename T>
void list<T>::push_back(const T &val) {
    auto * v = new fullnode(val, fake.left, &fake);
    fake.left->right = v;
    fake.left = v;
}

template<typename T>
void list<T>::pop_back() {
    if(empty()) {
        return;
    }
    node *l = fake.left;
    fake.left = l->left;
    l->left->right = &fake;
    delete static_cast<fullnode*>(l);
}

template<typename T>
T &list<T>::back() {
    return (static_cast<fullnode*>(fake.left))->val;
}

template<typename T>
T const &list<T>::back() const {
    return (static_cast<fullnode const*>(fake.left))->val;
}

template<typename T>
void list<T>::push_front(const T &val) {
    auto * v = new fullnode(val, &fake, fake.right);
    fake.right->left = v;
    fake.right = v;
}

template<typename T>
T &list<T>::front() {
    return (static_cast<fullnode*>(fake.right))->val;
}

template<typename T>
void list<T>::pop_front() {
    if(empty()) {
        return;
    }
    node *r = fake.right;
    fake.right = r->right;
    r->right->left = &fake;
    delete static_cast<fullnode*>(r);
}

template<typename T>
T const &list<T>::front() const {
    return (static_cast<fullnode const*>(fake.right))->val;
}

template<typename T>
list<T> &list<T>::operator=(list const & other) {
    list<T> t = other;
    swap(t);
    return *this;
}

template<typename T>
void list<T>::swap(list &other) {
    node* a_l = fake.left;
    node* a_r = fake.right;
    node* b_l = other.fake.left;
    node* b_r = other.fake.right;
    a_l->right = &other.fake;
    a_r->left = &other.fake;
    b_l->right = &fake;
    b_r->left = &fake;
    std::swap(fake, other.fake);
}

template<typename T>
void list<T>::splice(list::const_iterator pos, list &other, list::const_iterator first, list::const_iterator last) {
    node* l = first.cur->left;

    pos.cur->left->right = first.cur;
    first.cur->left = pos.cur->left;

    last.cur->left->right = pos.cur;
    pos.cur->left = last.cur->left;

    last.cur->left = l;
    l->right = last.cur;
}

