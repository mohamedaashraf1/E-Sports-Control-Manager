#ifndef NODE_H
#define NODE_H

template <typename T>
struct Node
{
    T data;
    Node<T>* next;

    explicit Node(const T& value)
        : data(value), next(nullptr)
    {
    }
};

#endif // NODE_H
