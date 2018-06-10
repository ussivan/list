#ifndef LIST_MY_LIST_H
#define LIST_MY_LIST_H

#include <memory>
#include <list>

template <typename T>
struct my_list {
private:
    struct node_without_data {
        node_without_data *left;

        node_without_data *right;

        node_without_data() : left(nullptr), right(nullptr) {}

        node_without_data(node_without_data *left, node_without_data *right) : left(left), right(right) {}

        virtual ~node_without_data() {}
    };

    struct node : node_without_data {
        T data;

        node(const T &value, node_without_data *left, node_without_data *right): node_without_data(left, right), data(value) {}

        ~node() {}
    };

    node_without_data fake_node;

public:
    my_list();

    my_list(const my_list &other);

    ~my_list();

    bool empty() const;

    void clear();

    void push_back(const T& value);

    T pop_back();

    T& back();

    void push_front(const T& value);

    T pop_front();

    T& front();

private:
    template <typename V>
    struct my_iterator : std::iterator<std::bidirectional_iterator_tag, V, ptrdiff_t, V*, V&> {

        node_without_data *cur_pos;

        my_iterator() : cur_pos(nullptr) {}

        explicit my_iterator(node_without_data *cur_pos) : cur_pos(cur_pos) {}

        my_iterator(const my_iterator &it) : cur_pos(it.cur_pos) {}

        my_iterator& operator++() {
            cur_pos = cur_pos->right;
            return *this;
        }

        my_iterator operator++(int) {
            my_iterator result = *this;
            ++(*this);
            return result;
        }

        my_iterator& operator--() {
            cur_pos = cur_pos->left;
            return *this;
        }

        my_iterator operator--(int) {
            my_iterator result = *this;
            --(*this);
            return result;
        }

        bool operator==(my_iterator const &other) const {
            return cur_pos == other.cur_pos;
        }

        bool operator!=(my_iterator const &other) const {
            return !(*this == other);
        }

        T& operator*() const {
            return static_cast<node*>(cur_pos)->data;
        }
    };

public:

    typedef my_iterator<T> iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef my_iterator<T const> const_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() {
        return iterator(fake_node.right);
    }

    iterator end() {
        return iterator(&fake_node);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_iterator begin() const {
        return const_iterator(fake_node.right);
    }

    const_iterator end() const {
        return const_iterator(&fake_node);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    // before pos
    iterator insert(iterator pos, T const &value) {
        node *new_node = new node(value, pos.cur_pos->left, pos.cur_pos);
        pos.cur_pos->left->right = new_node;
        pos.cur_pos->left = new_node;
        return iterator(new_node);
    }

    iterator erase(iterator pos) {
        node *cur_node = static_cast<node*>(pos.cur_pos);
        cur_node->left->right = cur_node->right;
        cur_node->right->left = cur_node->left;

        iterator result(cur_node->right);
        delete cur_node;
        return result;
    }

    iterator erase(iterator beg_pos, iterator end_pos) {
        iterator tmp(beg_pos);

        while (beg_pos != end_pos) {
            erase(tmp++);
        }
        return end_pos;
    }

    void splice(iterator pos, my_list &other, iterator beg_pos, iterator end_pos) {
        node_without_data *cur_node = pos.cur_pos;
        iterator end_p = end_pos;
        node_without_data *tmp = end_pos.cur_pos->left;
        beg_pos.cur_pos->left->right = end_pos.cur_pos;
        end_p.cur_pos->left->right = pos.cur_pos;
        end_pos.cur_pos->left = beg_pos.cur_pos->left;
        beg_pos.cur_pos->left = pos.cur_pos->left;

        pos.cur_pos->left->right = beg_pos.cur_pos;
        pos.cur_pos->left = tmp;
    }

    template <typename TT>
    friend void swap(my_list<TT> &first, my_list<TT> &second);
};

template <typename T>
my_list<T>::my_list() : fake_node() {
    fake_node.left = fake_node.right = &fake_node;
}
template <typename T>
my_list<T>::my_list(const my_list &other) {
    my_list();
    for (auto i = other.begin(); i != other.end(); i++) {
        push_back(*i);
    }
}

template <typename T>
my_list<T>::~my_list() {
    clear();
}

template <typename T>
void my_list<T>::clear() {
    while (!empty()) {
        pop_back();
    }
}

template <typename T>
bool my_list<T>::empty() const {
    return &fake_node == fake_node.left;
}

template <typename T>
void my_list<T>::push_back(const T &value) {
    node *new_node = new node(value, fake_node.left, &fake_node);
    fake_node.left->right = new_node;
    fake_node.left = new_node;
}

template <typename T>
T& my_list<T>::back() {
    return static_cast<node *>(fake_node.left)->data;
}

template <typename T>
T my_list<T>::pop_back() {
    node *cur_node = static_cast<node*>(fake_node.left);
    T data(cur_node->data);
    fake_node.left = cur_node->left;
    cur_node->left->right = &fake_node;
    delete cur_node;

    return data;
}

template <typename T>
void my_list<T>::push_front(const T &value) {
    node *new_node = new node(value, &fake_node, fake_node.right);
    fake_node.right->left = new_node;
    fake_node.right = new_node;
}

template <typename T>
T& my_list<T>::front() {
    return static_cast<node *>(fake_node.right)->data;
}

template <typename T>
T my_list<T>::pop_front() {
    node *cur_node = static_cast<node*>(fake_node.right);
    T data(cur_node->data);
    fake_node.right = cur_node->right;
    cur_node->right->left = &fake_node;
    delete cur_node;

    return data;
}


template <typename TT>
void swap(my_list<TT> &first, my_list<TT> &second) {
    typename my_list<TT>::node_without_data *f_l = first.fake_node.left,
            *f_r = second.fake_node.right,
            *s_l = second.fake_node.left,
            *s_r = second.fake_node.right;
    first.fake_node.left->right = &second.fake_node;
    first.fake_node.left = s_l;
    first.fake_node.right->left = &second.fake_node;
    first.fake_node.right = s_r;

    second.fake_node.left->right = &first.fake_node;
    second.fake_node.left = f_l;
    second.fake_node.right->left = &first.fake_node;
    second.fake_node.right = f_r;
}


#endif //LIST_MY_LIST_H
