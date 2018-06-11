#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>

template <typename T>
struct list
{
private:
    struct node;
    struct valnode;

    template <typename V>
    struct basic_iterator;

public:
    using iterator = basic_iterator<T>;
    using const_iterator = basic_iterator<T const>;
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

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    iterator insert(const_iterator pos, T const& val);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);

    void swap(list& other);

private:
    template <typename V>
    static void triswap(V& a, V& b, V& c);

    void check_invariant();

private:
    node fake;
};

template <typename T>
struct list<T>::node
{
    node();
    node(node* prev, node* next);

    T& value();
    void fixup();
    void reset();

    friend struct list;

private:
    void check_invariant();

private:
    node* prev;
    node* next;
};

template <typename T>
struct list<T>::valnode : node
{
    valnode(node* prev, node* next, T const& value)
        : node(prev, next)
        , value(value)
    {}

private:
    T value;

    friend struct node;
};

template <typename T>
template <typename V>
struct list<T>::basic_iterator
{
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = V*;
    using reference = V&;

    basic_iterator();
    basic_iterator(basic_iterator const&) = default;
    template <typename U>
    basic_iterator(basic_iterator<U> const&, typename std::enable_if<std::is_same<U const, V>::value && std::is_const<V>::value>::type* = nullptr);
    basic_iterator& operator=(basic_iterator const&) = default;

    reference operator*() const;
    pointer operator->() const;

    basic_iterator& operator++() &;
    basic_iterator operator++(int) &;

    basic_iterator& operator--() &;
    basic_iterator operator--(int) &;

    friend bool operator==(basic_iterator const& a, basic_iterator const& b)
    {
        return a.p == b.p;
    }

    friend bool operator!=(basic_iterator const& a, basic_iterator const& b)
    {
        return a.p != b.p;
    }

private:
    basic_iterator(node*);

private:
    node* p;

    friend struct list;
};

template <typename T>
list<T>::list()
{}

template <typename T>
list<T>::list(list const& other)
    : list()
{
    for (T const& v : other)
        push_back(v);
}

template <typename T>
list<T>& list<T>::operator=(list const& rhs)
{
    list copy = rhs;
    swap(copy);
    return *this;
}

template <typename T>
list<T>::~list()
{
    clear();
}

template <typename T>
void list<T>::clear()
{
    if (fake.next == &fake)
        return;

    for (node* p = fake.next; p != &fake;)
    {
        node* n = p->next;
        delete static_cast<valnode*>(p);
        p = n;
    }

    fake.reset();
}

template <typename T>
bool list<T>::empty()
{
    return fake.next == &fake;
}

template <typename T>
void list<T>::push_back(T const& val)
{
    insert(end(), val);
}

template <typename T>
void list<T>::pop_back()
{
    erase(std::prev(end()));
}

template <typename T>
T& list<T>::back()
{
    return *std::prev(end());
}

template <typename T>
T const& list<T>::back() const
{
    return *std::prev(end());
}

template <typename T>
void list<T>::push_front(T const& val)
{
    insert(begin(), val);
}
template <typename T>
void list<T>::pop_front()
{
    erase(begin());
}

template <typename T>
T& list<T>::front()
{
    return *begin();
}

template <typename T>
T const& list<T>::front() const
{
    return *begin();
}

template <typename T>
typename list<T>::iterator list<T>::begin()
{
    return iterator(fake.next);
}

template <typename T>
typename list<T>::const_iterator list<T>::begin() const
{
    return const_iterator(fake.next);
}

template <typename T>
typename list<T>::iterator list<T>::end()
{
    return iterator(&fake);
}

template <typename T>
typename list<T>::const_iterator list<T>::end() const
{
    return const_iterator(const_cast<node*>(&fake));
}

template <typename T>
typename list<T>::reverse_iterator list<T>::rbegin()
{
    return reverse_iterator(end());
}

template <typename T>
typename list<T>::const_reverse_iterator list<T>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <typename T>
typename list<T>::reverse_iterator list<T>::rend()
{
    return reverse_iterator(begin());
}

template <typename T>
typename list<T>::const_reverse_iterator list<T>::rend() const
{
    return const_reverse_iterator(begin());
}

template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, T const& val)
{
    valnode* q = new valnode(pos.p->prev, pos.p, val);
    q->fixup();

    check_invariant();

    return iterator(q);
}

template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator pos)
{
    node* ret = pos.p->next;
    pos.p->prev->next = pos.p->next;
    ret->prev = pos.p->prev;
    delete static_cast<valnode*>(pos.p);

    check_invariant();
    
    return iterator(ret);
}

template <typename T>
typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last)
{
    for (const_iterator i = first; i != last;)
    {
        const_iterator j = std::next(i);
        erase(i);
        i = j;
    }

    return iterator(last.p);
}

template <typename T>
void list<T>::splice(const_iterator pos, list&, const_iterator first, const_iterator last)
{
    if (first == last)
        return;

    triswap(pos.p->prev->next, first.p->prev->next, last.p->prev->next);
    triswap(pos.p->prev, last.p->prev, first.p->prev);

    check_invariant();
}

template <typename T>
void list<T>::swap(list& other)
{
    if (!empty())
    {
        if (!other.empty())
        {
            using std::swap;
            swap(fake, other.fake);
            fake.fixup();
            other.fake.fixup();
        }
        else
        {
            other.fake = fake;
            other.fake.fixup();
            fake.reset();
        }
    }
    else
    {
        if (!other.empty())
        {
            fake = other.fake;
            fake.fixup();
            other.fake.reset();
        }
    }
}

template <typename T> template <typename V>
void list<T>::triswap(V& a, V& b, V& c)
{
    V a_copy = a;
    V b_copy = b;
    V c_copy = c;
    a = b_copy;
    b = c_copy;
    c = a_copy;
}

template <typename T>
void list<T>::check_invariant()
{
#ifndef NDEBUG
    fake.check_invariant();
    for (node* p = fake.next; p != &fake; p = p->next)
    {
        p->check_invariant();
    }
#endif
}

template <typename T>
list<T>::node::node()
    : prev(this)
    , next(this)
{}

template <typename T>
list<T>::node::node(node* prev, node* next)
    : prev(prev)
    , next(next)
{}

template <typename T>
T& list<T>::node::value()
{
    return static_cast<valnode&>(*this).value;
}

template<typename T>
void list<T>::node::fixup()
{
    prev->next = this;
    next->prev = this;
}

template <typename T>
void list<T>::node::reset()
{
    prev = this;
    next = this;
}

template <typename T>
void list<T>::node::check_invariant()
{
    assert(prev->next == this);
    assert(next->prev == this);
}

template <typename T> template <typename V>
list<T>::basic_iterator<V>::basic_iterator()
{}

template <typename T> template <typename V> template <typename U>
list<T>::basic_iterator<V>::basic_iterator(basic_iterator<U> const& other, typename std::enable_if<std::is_same<U const, V>::value && std::is_const<V>::value>::type*)
    : p(other.p)
{}

template <typename T> template <typename V>
typename list<T>::template basic_iterator<V>::reference list<T>::basic_iterator<V>::operator*() const
{
    return p->value();
}

template <typename T> template <typename V>
typename list<T>::template basic_iterator<V>::pointer list<T>::basic_iterator<V>::operator->() const
{
    return &p->value();
}

template <typename T> template <typename V>
list<T>::basic_iterator<V>& list<T>::basic_iterator<V>::operator++() &
{
    p = p->next;
    return *this;
}

template <typename T> template <typename V>
list<T>::basic_iterator<V> list<T>::basic_iterator<V>::operator++(int) &
{
    basic_iterator copy = *this;
    ++*this;
    return copy;
}

template <typename T> template <typename V>
list<T>::basic_iterator<V>& list<T>::basic_iterator<V>::operator--() &
{
    p = p->prev;
    return *this;
}

template <typename T> template <typename V>
list<T>::basic_iterator<V> list<T>::basic_iterator<V>::operator--(int) &
{
    basic_iterator copy = *this;
    --*this;
    return copy;
}

template <typename T> template <typename V>
list<T>::basic_iterator<V>::basic_iterator(node* p)
    : p(p)
{}

template <typename T>
void swap(list<T>& a, list<T>& b)
{
    a.swap(b);
}
