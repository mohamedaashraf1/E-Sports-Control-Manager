#include "tournamentsystem.h"

#include <QDateTime>
#include <QRegularExpression>

TournamentSystem::TournamentSystem()
{
    loadSampleData();
}

bool TournamentSystem::addPlayer(const Player& player, QString& errorMessage)
{
    if (!validatePlayer(player, errorMessage))
        return false;

    if (!isPlayerIdUnique(player.id))
    {
        errorMessage = "Player ID must be unique.";
        return false;
    }

    m_registeredPlayers.pushBack(player);
    return true;
}

bool TournamentSystem::updatePlayer(const Player& player, QString& errorMessage)
{
    if (!validatePlayer(player, errorMessage))
        return false;

    Player* existing = findPlayerById(player.id);
    if (existing == nullptr)
    {
        errorMessage = "Player not found.";
        return false;
    }

    existing->name = player.name;
    existing->age = player.age;
    existing->gameName = player.gameName;
    existing->rank = player.rank;

    return true;
}

bool TournamentSystem::deletePlayer(const QString& playerId, QString& errorMessage)
{
    QString cleanedId = playerId.trimmed();
    if (cleanedId.isEmpty())
    {
        errorMessage = "Player ID is required.";
        return false;
    }

    if (m_matchInProgress &&
        (m_activePlayer1.id.compare(cleanedId, Qt::CaseInsensitive) == 0 ||
         m_activePlayer2.id.compare(cleanedId, Qt::CaseInsensitive) == 0))
    {
        errorMessage = "Cannot delete a player in an active match.";
        return false;
    }

    m_waitingQueue.removeById(cleanedId);

    bool removed = m_registeredPlayers.removeFirstWhere(
        [&](const Player& p) { return p.id.compare(cleanedId, Qt::CaseInsensitive) == 0; });

    if (!removed)
    {
        errorMessage = "Player not found.";
        return false;
    }

    return true;
}

std::vector<Player> TournamentSystem::searchPlayers(const QString& keyword, bool searchById) const
{
    const QString cleaned = keyword.trimmed();
    std::vector<Player> result;

    for (const Player& p : m_registeredPlayers.toVector())
    {
        if (searchById)
        {
            if (p.id.compare(cleaned, Qt::CaseInsensitive) == 0)
                result.push_back(p);
        }
        else
        {
            if (p.name.contains(cleaned, Qt::CaseInsensitive))
                result.push_back(p);
        }
    }

    return result;
}

std::vector<Player> TournamentSystem::getAllPlayers() const
{
    return m_registeredPlayers.toVector();
}

bool TournamentSystem::enqueuePlayer(const QString& playerId, QString& errorMessage)
{
    QString cleanedId = playerId.trimmed();
    if (cleanedId.isEmpty())
    {
        errorMessage = "Player ID is required.";
        return false;
    }

    if (findPlayerById(cleanedId) == nullptr)
    {
        errorMessage = "Register player first before queuing.";
        return false;
    }

    if (!m_waitingQueue.enqueue(cleanedId))
    {
        errorMessage = "Player is already in queue.";
        return false;
    }

    return true;
}

std::vector<Player> TournamentSystem::getQueuePlayers() const
{
    std::vector<Player> queuePlayers;
    const std::vector<QString> ids = m_waitingQueue.snapshot();
    queuePlayers.reserve(ids.size());

    for (const QString& id : ids)
    {
        const Player* player = findPlayerById(id);
        if (player != nullptr)
            queuePlayers.push_back(*player);
    }

    return queuePlayers;
}

bool TournamentSystem::peekNextTwo(Player& first, Player& second, QString& errorMessage) const
{
    QString id1;
    QString id2;

    if (!m_waitingQueue.peekFront(id1) || !m_waitingQueue.peekSecond(id2))
    {
        errorMessage = "At least two players are required.";
        return false;
    }

    const Player* p1 = findPlayerById(id1);
    const Player* p2 = findPlayerById(id2);

    if (p1 == nullptr || p2 == nullptr)
    {
        errorMessage = "Queue contains invalid player references.";
        return false;
    }

    first = *p1;
    second = *p2;
    return true;
}

void TournamentSystem::resetQueue()
{
    m_waitingQueue.clear();
}

bool TournamentSystem::startNextMatch(QString& errorMessage)
{
    if (m_matchInProgress)
    {
        errorMessage = "Finish active match before starting a new one.";
        return false;
    }

    QString id1;
    QString id2;

    if (!m_waitingQueue.dequeue(id1) || !m_waitingQueue.dequeue(id2))
    {
        if (!id1.isEmpty())
            m_waitingQueue.enqueue(id1);

        errorMessage = "At least two players are required to start a match.";
        return false;
    }

    Player* p1 = findPlayerById(id1);
    Player* p2 = findPlayerById(id2);

    if (p1 == nullptr || p2 == nullptr)
    {
        errorMessage = "Invalid queued players encountered.";
        return false;
    }

    m_activePlayer1 = *p1;
    m_activePlayer2 = *p2;
    m_matchInProgress = true;
    return true;
}

bool TournamentSystem::recordMatchResult(const QString& winnerName,
                                         const QString& score,
                                         const QString& round,
                                         QString& errorMessage)
{
    if (!m_matchInProgress)
    {
        errorMessage = "No active match to record.";
        return false;
    }

    const QString cleanedWinner = winnerName.trimmed();
    const QString cleanedScore = score.trimmed();
    const QString cleanedRound = round.trimmed();

    if (cleanedWinner.isEmpty() || cleanedScore.isEmpty() || cleanedRound.isEmpty())
    {
        errorMessage = "Winner, score, and round are required.";
        return false;
    }

    if (cleanedWinner != m_activePlayer1.name && cleanedWinner != m_activePlayer2.name)
    {
        errorMessage = "Winner must be one of the active players.";
        return false;
    }

    if (!isValidScore(cleanedScore))
    {
        errorMessage = "Invalid score format. Use format like 2-1.";
        return false;
    }

    Match match;
    match.matchId = m_nextMatchId++;
    match.player1Name = m_activePlayer1.name;
    match.player2Name = m_activePlayer2.name;
    match.winnerName = cleanedWinner;
    match.score = cleanedScore;
    match.round = cleanedRound;
    match.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    m_matchHistory.pushBack(match);

    const Player winner = (cleanedWinner == m_activePlayer1.name) ? m_activePlayer1 : m_activePlayer2;
    m_winners.pushBack(winner);

    m_matchInProgress = false;
    m_activePlayer1 = Player{};
    m_activePlayer2 = Player{};

    return true;
}

bool TournamentSystem::hasActiveMatch() const
{
    return m_matchInProgress;
}

std::pair<QString, QString> TournamentSystem::getActivePlayerNames() const
{
    if (!m_matchInProgress)
        return {QString(), QString()};

    return {m_activePlayer1.name, m_activePlayer2.name};
}

QString TournamentSystem::getActiveMatchDisplay() const
{
    if (!m_matchInProgress)
        return "No active match.";

    return QString("%1 vs %2").arg(m_activePlayer1.name, m_activePlayer2.name);
}

std::vector<Match> TournamentSystem::getMatchHistory() const
{
    return m_matchHistory.toVector();
}

void TournamentSystem::clearMatchHistory()
{
    m_matchHistory.clear();
    m_winners.clear();
}

std::vector<Player> TournamentSystem::getWinnersList() const
{
    return m_winners.toVector();
}

bool TournamentSystem::validatePlayer(const Player& player, QString& errorMessage) const
{
    if (player.id.trimmed().isEmpty())
    {
        errorMessage = "Player ID cannot be empty.";
        return false;
    }

    if (player.name.trimmed().isEmpty())
    {
        errorMessage = "Player name cannot be empty.";
        return false;
    }

    if (player.age < 10 || player.age > 80)
    {
        errorMessage = "Age must be between 10 and 80.";
        return false;
    }

    if (player.gameName.trimmed().isEmpty())
    {
        errorMessage = "Game name cannot be empty.";
        return false;
    }

    if (player.rank.trimmed().isEmpty())
    {
        errorMessage = "Rank cannot be empty.";
        return false;
    }

    return true;
}

bool TournamentSystem::isPlayerIdUnique(const QString& playerId) const
{
    return findPlayerById(playerId) == nullptr;
}

Player* TournamentSystem::findPlayerById(const QString& playerId)
{
    const QString cleaned = playerId.trimmed();
    return m_registeredPlayers.findFirst(
        [&](const Player& p) { return p.id.compare(cleaned, Qt::CaseInsensitive) == 0; });
}

const Player* TournamentSystem::findPlayerById(const QString& playerId) const
{
    const QString cleaned = playerId.trimmed();
    return m_registeredPlayers.findFirst(
        [&](const Player& p) { return p.id.compare(cleaned, Qt::CaseInsensitive) == 0; });
}

bool TournamentSystem::isValidScore(const QString& score) const
{
    const QRegularExpression regex("^\\d+\\s*-\\s*\\d+$");
    return regex.match(score).hasMatch();
}

void TournamentSystem::loadSampleData()
{
    QString ignored;
    addPlayer({"P001", "Ayaan", 20, "Valorant", "Diamond"}, ignored);
    addPlayer({"P002", "Sara", 21, "Dota 2", "Immortal"}, ignored);
    addPlayer({"P003", "Hamza", 19, "CS2", "Master"}, ignored);
    addPlayer({"P004", "Mina", 22, "League of Legends", "Platinum"}, ignored);

    enqueuePlayer("P001", ignored);
    enqueuePlayer("P002", ignored);
    enqueuePlayer("P003", ignored);
}
