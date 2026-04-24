#include "queuemanager.h"

QueueManager::~QueueManager()
{
    clear();
}

bool QueueManager::enqueue(const QString& playerId)
{
    QString cleanedId = playerId.trimmed();
    if (cleanedId.isEmpty() || contains(cleanedId))
        return false;

    Node<QString>* newNode = new Node<QString>(cleanedId);

    if (m_rear == nullptr)
    {
        m_front = m_rear = newNode;
    }
    else
    {
        m_rear->next = newNode;
        m_rear = newNode;
    }

    ++m_size;
    return true;
}

bool QueueManager::dequeue(QString& playerId)
{
    if (m_front == nullptr)
        return false;

    Node<QString>* removedNode = m_front;
    playerId = removedNode->data;

    m_front = m_front->next;
    if (m_front == nullptr)
        m_rear = nullptr;

    delete removedNode;
    --m_size;
    return true;
}

bool QueueManager::peekFront(QString& playerId) const
{
    if (m_front == nullptr)
        return false;

    playerId = m_front->data;
    return true;
}

bool QueueManager::peekSecond(QString& playerId) const
{
    if (m_front == nullptr || m_front->next == nullptr)
        return false;

    playerId = m_front->next->data;
    return true;
}

bool QueueManager::contains(const QString& playerId) const
{
    const QString cleanedId = playerId.trimmed();
    const Node<QString>* current = m_front;

    while (current)
    {
        if (current->data.compare(cleanedId, Qt::CaseInsensitive) == 0)
            return true;

        current = current->next;
    }

    return false;
}

bool QueueManager::removeById(const QString& playerId)
{
    const QString cleanedId = playerId.trimmed();
    Node<QString>* previous = nullptr;
    Node<QString>* current = m_front;

    while (current)
    {
        if (current->data.compare(cleanedId, Qt::CaseInsensitive) == 0)
        {
            if (previous == nullptr)
                m_front = current->next;
            else
                previous->next = current->next;

            if (current == m_rear)
                m_rear = previous;

            delete current;
            --m_size;
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

std::vector<QString> QueueManager::snapshot() const
{
    std::vector<QString> ids;
    ids.reserve(static_cast<std::size_t>(m_size));

    const Node<QString>* current = m_front;
    while (current)
    {
        ids.push_back(current->data);
        current = current->next;
    }

    return ids;
}

int QueueManager::size() const
{
    return m_size;
}

bool QueueManager::isEmpty() const
{
    return m_size == 0;
}

void QueueManager::clear()
{
    Node<QString>* current = m_front;

    while (current)
    {
        Node<QString>* next = current->next;
        delete current;
        current = next;
    }

    m_front = nullptr;
    m_rear = nullptr;
    m_size = 0;
}
