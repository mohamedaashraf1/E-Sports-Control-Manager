#ifndef MATCH_H
#define MATCH_H

#include <QString>

struct Match
{
    int matchId = 0;
    QString player1Name;
    QString player2Name;
    QString winnerName;
    QString score;
    QString round;
    QString timestamp;
};

#endif // MATCH_H
