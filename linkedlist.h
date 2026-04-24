#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <cstddef>
#include <functional>
#include <vector>

#include "node.h"

template <typename T>
class LinkedList
{
public:
    LinkedList() = default;
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void pushBack(const T& value)
    {
        Node<T>* newNode = new Node<T>(value);
        if (m_tail == nullptr)
        {
            m_head = m_tail = newNode;
        }
        else
        {
            m_tail->next = newNode;
            m_tail = newNode;
        }
        ++m_size;
    }

    void append(const T& value)
    {
        pushBack(value);
    }

    bool removeFirstWhere(const std::function<bool(const T&)>& predicate, T* removedData = nullptr)
    {
        Node<T>* previous = nullptr;
        Node<T>* current = m_head;

        while (current)
        {
            if (predicate(current->data))
            {
                if (removedData)
                    *removedData = current->data;

                if (previous == nullptr)
                    m_head = current->next;
                else
                    previous->next = current->next;

                if (current == m_tail)
                    m_tail = previous;

                delete current;
                --m_size;
                return true;
            }

            previous = current;
            current = current->next;
        }
        return false;
    }

    bool removeFirst(const std::function<bool(const T&)>& predicate, T* removedData = nullptr)
    {
        return removeFirstWhere(predicate, removedData);
    }

    bool updateFirst(const std::function<bool(const T&)>& predicate, const std::function<void(T&)>& updater)
    {
        Node<T>* current = m_head;

        while (current)
        {
            if (predicate(current->data))
            {
                updater(current->data);
                return true;
            }
            current = current->next;
        }
        return false;
    }

    T* findFirst(const std::function<bool(const T&)>& predicate)
    {
        Node<T>* current = m_head;

        while (current)
        {
            if (predicate(current->data))
                return &current->data;
            current = current->next;
        }
        return nullptr;
    }

    const T* findFirst(const std::function<bool(const T&)>& predicate) const
    {
        const Node<T>* current = m_head;

        while (current)
        {
            if (predicate(current->data))
                return &current->data;
            current = current->next;
        }
        return nullptr;
    }

    bool contains(const std::function<bool(const T&)>& predicate) const
    {
        const Node<T>* current = m_head;
        while (current)
        {
            if (predicate(current->data))
                return true;
            current = current->next;
        }
        return false;
    }

    bool any(const std::function<bool(const T&)>& predicate) const { return contains(predicate); }

    std::vector<T> toVector() const
    {
        std::vector<T> values;
        values.reserve(m_size);

        const Node<T>* current = m_head;
        while (current)
        {
            values.push_back(current->data);
            current = current->next;
        }
        return values;
    }

    void clear()
    {
        Node<T>* current = m_head;

        while (current)
        {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    std::size_t size() const { return m_size; }

    bool isEmpty() const { return m_size == 0; }

private:
    Node<T>* m_head = nullptr;
    Node<T>* m_tail = nullptr;
    std::size_t m_size = 0;
};

#endif // LINKEDLIST_H
