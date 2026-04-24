#ifndef TOURNAMENTSYSTEM_H
#define TOURNAMENTSYSTEM_H

#include <QString>
#include <utility>
#include <vector>

#include "linkedlist.h"
#include "match.h"
#include "player.h"
#include "queuemanager.h"

class TournamentSystem
{
public:
    TournamentSystem();

    bool addPlayer(const Player& player, QString& errorMessage);
    bool updatePlayer(const Player& player, QString& errorMessage);
    bool deletePlayer(const QString& playerId, QString& errorMessage);

    std::vector<Player> searchPlayers(const QString& keyword, bool searchById) const;
    std::vector<Player> getAllPlayers() const;

    bool enqueuePlayer(const QString& playerId, QString& errorMessage);
    std::vector<Player> getQueuePlayers() const;
    bool peekNextTwo(Player& first, Player& second, QString& errorMessage) const;
    void resetQueue();

    bool startNextMatch(QString& errorMessage);
    bool recordMatchResult(const QString& winnerName, const QString& score, const QString& round, QString& errorMessage);
    bool hasActiveMatch() const;
    std::pair<QString, QString> getActivePlayerNames() const;
    QString getActiveMatchDisplay() const;

    std::vector<Match> getMatchHistory() const;
    void clearMatchHistory();
    std::vector<Player> getWinnersList() const;

private:
    bool validatePlayer(const Player& player, QString& errorMessage) const;
    bool isPlayerIdUnique(const QString& playerId) const;
    Player* findPlayerById(const QString& playerId);
    const Player* findPlayerById(const QString& playerId) const;
    bool isValidScore(const QString& score) const;
    void loadSampleData();

    LinkedList<Player> m_registeredPlayers;
    QueueManager m_waitingQueue;
    LinkedList<Match> m_matchHistory;
    LinkedList<Player> m_winners;

    bool m_matchInProgress = false;
    Player m_activePlayer1;
    Player m_activePlayer2;
    int m_nextMatchId = 1;
};

#endif // TOURNAMENTSYSTEM_H
