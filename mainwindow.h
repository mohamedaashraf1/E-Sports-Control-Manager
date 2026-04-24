#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QStackedWidget>
#include <QTableWidget>

#include <vector>

#include "tournamentsystem.h"

class QComboBox;
class QButtonGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void handleAddPlayer();
    void handleUpdatePlayer();
    void handleDeletePlayer();
    void handleSearchPlayer();
    void handleAddToQueue();
    void handlePeekNextTwo();
    void handleResetQueue();
    void handleStartMatch();
    void handleRecordResult();
    void handleClearHistory();
    void handleThemeChanged(const QString& themeName);

private:
    void buildUi();
    QWidget* createDashboardPage();
    QWidget* createPlayersPage();
    QWidget* createQueuePage();
    QWidget* createMatchPage();
    QWidget* createHistoryPage();
    QWidget* createWinnersPage();
    QPushButton* createNavButton(const QString& text, int pageIndex);

    bool readPlayerForm(Player& player, QString& errorMessage) const;
    void clearPlayerForm();

    void refreshDashboardCards();
    void refreshPlayersTable(const std::vector<Player>& players);
    void refreshQueueTable();
    void refreshHistoryTable();
    void refreshWinnersTable();
    void refreshAllTables();
    void updateMatchUiState();

    void showError(const QString& message);
    void showSuccess(const QString& message);

    TournamentSystem m_system;

    QStackedWidget* m_pages = nullptr;
    QButtonGroup* m_navGroup = nullptr;
    QLabel* m_headerStatusLabel = nullptr;
    QLabel* m_readyBadge = nullptr;
    QLabel* m_liveBadge = nullptr;
    QLabel* m_queueBadge = nullptr;
    QComboBox* m_themeCombo = nullptr;

    QLabel* m_totalPlayersValue = nullptr;
    QLabel* m_queuePlayersValue = nullptr;
    QLabel* m_matchesPlayedValue = nullptr;
    QLabel* m_winnersCountValue = nullptr;
    QLabel* m_dashboardNextMatchLabel = nullptr;
    QLabel* m_dashboardQueuePreviewLabel = nullptr;
    QLabel* m_dashboardRecentWinnerLabel = nullptr;

    QLineEdit* m_idInput = nullptr;
    QLineEdit* m_nameInput = nullptr;
    QLineEdit* m_ageInput = nullptr;
    QLineEdit* m_gameInput = nullptr;
    QLineEdit* m_rankInput = nullptr;
    QLineEdit* m_searchInput = nullptr;
    QComboBox* m_searchByCombo = nullptr;
    QTableWidget* m_playersTable = nullptr;

    QLineEdit* m_queuePlayerIdInput = nullptr;
    QTableWidget* m_queueTable = nullptr;

    QLabel* m_activeMatchLabel = nullptr;
    QLabel* m_player1CardLabel = nullptr;
    QLabel* m_player2CardLabel = nullptr;
    QComboBox* m_winnerCombo = nullptr;
    QLineEdit* m_scoreInput = nullptr;
    QLineEdit* m_roundInput = nullptr;
    QPushButton* m_startMatchButton = nullptr;
    QPushButton* m_recordResultButton = nullptr;

    QLabel* m_latestWinnerLabel = nullptr;
    QTableWidget* m_historyTable = nullptr;
    QTableWidget* m_winnersTable = nullptr;
};

#endif // MAINWINDOW_H
