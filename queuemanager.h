#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <QString>
#include <vector>

#include "node.h"

class QueueManager
{
public:
    QueueManager() = default;
    ~QueueManager();

    QueueManager(const QueueManager&) = delete;
    QueueManager& operator=(const QueueManager&) = delete;

    bool enqueue(const QString& playerId);
    bool dequeue(QString& playerId);
    bool peekFront(QString& playerId) const;
    bool peekSecond(QString& playerId) const;
    bool contains(const QString& playerId) const;
    bool removeById(const QString& playerId);
    std::vector<QString> snapshot() const;
    int size() const;
    bool isEmpty() const;
    void clear();

private:
    Node<QString>* m_front = nullptr;
    Node<QString>* m_rear = nullptr;
    int m_size = 0;
};

#endif // QUEUEMANAGER_H
