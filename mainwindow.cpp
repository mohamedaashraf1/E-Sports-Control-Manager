#include "mainwindow.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QFile>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
    refreshAllTables();
    updateMatchUiState();
    statusBar()->showMessage("Control center ready.");
}

void MainWindow::buildUi()
{
    setWindowTitle("E-Sports Tournament Manager");
    resize(1380, 820);

    QWidget* central = new QWidget(this);
    QHBoxLayout* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(14);

    // =========================
    // Sidebar
    // =========================
    QWidget* sidebar = new QWidget(central);
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(220);

    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(14, 16, 14, 16);
    sidebarLayout->setSpacing(10);

    QWidget* appBrand = new QWidget(sidebar);
    appBrand->setObjectName("AppBrand");

    QHBoxLayout* appBrandLayout = new QHBoxLayout(appBrand);
    appBrandLayout->setContentsMargins(10, 10, 10, 10);
    appBrandLayout->setSpacing(10);

    QLabel* appIcon = new QLabel(QString::fromUtf8("🏆"), appBrand);
    appIcon->setObjectName("AppBrandIcon");
    appIcon->setTextFormat(Qt::PlainText);

    QWidget* appTextBlock = new QWidget(appBrand);
    QVBoxLayout* appTextLayout = new QVBoxLayout(appTextBlock);
    appTextLayout->setContentsMargins(0, 0, 0, 0);
    appTextLayout->setSpacing(0);

    QLabel* appTitle = new QLabel("ESPORTS", appTextBlock);
    appTitle->setObjectName("AppBrandTitle");
    appTitle->setTextFormat(Qt::PlainText);

    QLabel* appSubTitle = new QLabel("CONTROL", appTextBlock);
    appSubTitle->setObjectName("AppBrandSubTitle");
    appSubTitle->setTextFormat(Qt::PlainText);

    appTextLayout->addWidget(appTitle);
    appTextLayout->addWidget(appSubTitle);

    appBrandLayout->addWidget(appIcon, 0, Qt::AlignTop);
    appBrandLayout->addWidget(appTextBlock, 1);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);

    QPushButton* navDashboard = createNavButton("Dashboard", 0);
    QPushButton* navPlayers = createNavButton("Players", 1);
    QPushButton* navQueue = createNavButton("Queue", 2);
    QPushButton* navMatches = createNavButton("Matches", 3);
    QPushButton* navHistory = createNavButton("History", 4);
    QPushButton* navWinners = createNavButton("Winners", 5);

    sidebarLayout->addWidget(appBrand);
    sidebarLayout->addSpacing(8);
    sidebarLayout->addWidget(navDashboard);
    sidebarLayout->addWidget(navPlayers);
    sidebarLayout->addWidget(navQueue);
    sidebarLayout->addWidget(navMatches);
    sidebarLayout->addWidget(navHistory);
    sidebarLayout->addWidget(navWinners);

    QFrame* sidebarStatus = new QFrame(sidebar);
    sidebarStatus->setObjectName("SidebarStatusCard");

    QVBoxLayout* statusLayout = new QVBoxLayout(sidebarStatus);
    statusLayout->setContentsMargins(10, 10, 10, 10);
    statusLayout->setSpacing(6);

    QLabel* statusTitle = new QLabel("SYSTEM STATUS", sidebarStatus);
    statusTitle->setObjectName("SidebarStatusTitle");
    statusTitle->setTextFormat(Qt::PlainText);

    m_readyBadge = new QLabel("● READY", sidebarStatus);
    m_liveBadge = new QLabel("● LIVE MATCH", sidebarStatus);
    m_queueBadge = new QLabel("● QUEUE ACTIVE", sidebarStatus);

    m_readyBadge->setObjectName("StatusBadge");
    m_liveBadge->setObjectName("StatusBadge");
    m_queueBadge->setObjectName("StatusBadge");

    m_readyBadge->setTextFormat(Qt::PlainText);
    m_liveBadge->setTextFormat(Qt::PlainText);
    m_queueBadge->setTextFormat(Qt::PlainText);

    statusLayout->addWidget(statusTitle);
    statusLayout->addWidget(m_readyBadge);
    statusLayout->addWidget(m_liveBadge);
    statusLayout->addWidget(m_queueBadge);

    sidebarLayout->addStretch();
    sidebarLayout->addWidget(sidebarStatus);

    // =========================
    // Main Content Panel
    // =========================
    QWidget* contentPanel = new QWidget(central);
    contentPanel->setObjectName("ContentPanel");

    QVBoxLayout* contentLayout = new QVBoxLayout(contentPanel);
    contentLayout->setContentsMargins(14, 14, 14, 14);
    contentLayout->setSpacing(12);

    QWidget* header = new QWidget(contentPanel);
    header->setObjectName("TopHeader");

    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(14, 10, 14, 10);
    headerLayout->setSpacing(12);

    QWidget* titleBlock = new QWidget(header);
    QVBoxLayout* titleBlockLayout = new QVBoxLayout(titleBlock);
    titleBlockLayout->setContentsMargins(0, 0, 0, 0);
    titleBlockLayout->setSpacing(2);

    QLabel* title = new QLabel("E-Sports Tournament Manager", titleBlock);
    title->setObjectName("TitleLabel");
    title->setTextFormat(Qt::PlainText);

    m_headerStatusLabel = new QLabel("Dashboard Control Center", titleBlock);
    m_headerStatusLabel->setObjectName("HeaderStatusLabel");
    m_headerStatusLabel->setTextFormat(Qt::PlainText);

    titleBlockLayout->addWidget(title);
    titleBlockLayout->addWidget(m_headerStatusLabel);

    headerLayout->addWidget(titleBlock, 1);

    QWidget* rightHeader = new QWidget(header);
    QVBoxLayout* rightHeaderLayout = new QVBoxLayout(rightHeader);
    rightHeaderLayout->setContentsMargins(0, 0, 0, 0);
    rightHeaderLayout->setSpacing(6);

    // QLabel* modeLabel = new QLabel("Theme Mode", rightHeader);
    // modeLabel->setObjectName("ThemeModeLabel");
    // modeLabel->setTextFormat(Qt::PlainText);

    // m_themeCombo = new QComboBox(rightHeader);
    // m_themeCombo->setMinimumWidth(140);
    // m_themeCombo->addItem("Purple");
    // m_themeCombo->setCurrentText("Purple");

    // connect(m_themeCombo, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this, &MainWindow::handleThemeChanged);

    // rightHeaderLayout->addWidget(modeLabel, 0, Qt::AlignRight);
    // rightHeaderLayout->addWidget(m_themeCombo);

    headerLayout->addWidget(titleBlock, 1);
    headerLayout->addWidget(rightHeader, 0, Qt::AlignRight | Qt::AlignVCenter);

    m_pages = new QStackedWidget(contentPanel);
    m_pages->addWidget(createDashboardPage());
    m_pages->addWidget(createPlayersPage());
    m_pages->addWidget(createQueuePage());
    m_pages->addWidget(createMatchPage());
    m_pages->addWidget(createHistoryPage());
    m_pages->addWidget(createWinnersPage());

    contentLayout->addWidget(header);
    contentLayout->addWidget(m_pages, 1);

    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(contentPanel, 1);

    setCentralWidget(central);

    navDashboard->setChecked(true);
    m_pages->setCurrentIndex(0);

    handleThemeChanged("Purple");
}

QPushButton* MainWindow::createNavButton(const QString& text, int pageIndex)
{
    const QStringList icons = {
        QString::fromUtf8("🏁"),
        QString::fromUtf8("👥"),
        QString::fromUtf8("⏳"),
        QString::fromUtf8("🎮"),
        QString::fromUtf8("📜"),
        QString::fromUtf8("🏆")
    };

    const QString prefix = (pageIndex >= 0 && pageIndex < icons.size()) ? icons.at(pageIndex) : QString();
    QPushButton* button = new QPushButton(prefix + "  " + text, this);
    button->setCheckable(true);
    button->setObjectName("NavButton");
    button->setMinimumHeight(36);

    m_navGroup->addButton(button, pageIndex);

    connect(button, &QPushButton::clicked, this, [this, pageIndex, text]() {
        m_pages->setCurrentIndex(pageIndex);
        m_headerStatusLabel->setText(text + " Control Center");
    });

    return button;
}

QWidget* MainWindow::createDashboardPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    QGridLayout* cardsLayout = new QGridLayout();
    cardsLayout->setSpacing(12);

    auto createCard = [this](const QString& icon, const QString& title, const QString& cardType, QLabel*& valueLabel) {
        QFrame* card = new QFrame();
        card->setObjectName("StatCard");
        card->setProperty("cardType", "players");

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 10, 14, 10);
        cardLayout->setSpacing(14);

        QLabel* iconLabel = new QLabel(icon, card);
        iconLabel->setObjectName("StatIcon");
        iconLabel->setTextFormat(Qt::PlainText);

        QLabel* titleLabel = new QLabel(title, card);
        titleLabel->setObjectName("StatCaption");
        titleLabel->setTextFormat(Qt::PlainText);

        valueLabel = new QLabel("0", card);
        valueLabel->setObjectName("StatValue");
        valueLabel->setTextFormat(Qt::PlainText);

        cardLayout->addWidget(iconLabel);
        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(valueLabel);
        cardLayout->addStretch();

        return card;
    };

    cardsLayout->addWidget(createCard(QString::fromUtf8("👤"), "Total Players", "players", m_totalPlayersValue), 0, 0);
    cardsLayout->addWidget(createCard(QString::fromUtf8("⏱"), "Players in Queue", "queue", m_queuePlayersValue), 0, 1);
    cardsLayout->addWidget(createCard(QString::fromUtf8("⚔"), "Matches Played", "matches", m_matchesPlayedValue), 0, 2);
    cardsLayout->addWidget(createCard(QString::fromUtf8("🏆"), "Winners Count", "winners", m_winnersCountValue), 0, 3);

    QGroupBox* quickInfo = new QGroupBox("Tournament Snapshot", page);
    quickInfo->setObjectName("SnapshotCard");
    quickInfo->setAlignment(Qt::AlignLeft);
    QVBoxLayout* infoLayout = new QVBoxLayout(quickInfo);
    infoLayout->setContentsMargins(12, 12, 12, 12);
    infoLayout->setSpacing(10);

    QLabel* infoText = new QLabel(
        "• Tournament is ready for matches\n",
        quickInfo);
    infoText->setWordWrap(true);
    infoText->setTextFormat(Qt::PlainText);
    infoText->setObjectName("InfoText");

    infoLayout->addWidget(infoText);

    QGroupBox* snapshotCard = new QGroupBox("Live Tournament Snapshot", page);
    snapshotCard->setObjectName("SnapshotCard");
    snapshotCard->setAlignment(Qt::AlignLeft);

    QVBoxLayout* snapshotLayout = new QVBoxLayout(snapshotCard);
    snapshotLayout->setSpacing(10);
    snapshotLayout->setContentsMargins(12, 12, 12, 12);

    m_dashboardNextMatchLabel = new QLabel("• Next Match: Waiting for enough players...", snapshotCard);
    m_dashboardQueuePreviewLabel = new QLabel("• Queue Preview: No players in queue.", snapshotCard);
    m_dashboardRecentWinnerLabel = new QLabel("• Latest Winner: No winner yet.", snapshotCard);
    QLabel* note = new QLabel("• Use theme selector in header for presentation mode", snapshotCard);

    m_dashboardNextMatchLabel->setObjectName("InfoText");
    m_dashboardQueuePreviewLabel->setObjectName("InfoText");
    m_dashboardRecentWinnerLabel->setObjectName("InfoText");
    note->setObjectName("InfoText");

    m_dashboardNextMatchLabel->setWordWrap(true);
    m_dashboardQueuePreviewLabel->setWordWrap(true);
    m_dashboardRecentWinnerLabel->setWordWrap(true);
    note->setWordWrap(true);

    m_dashboardNextMatchLabel->setTextFormat(Qt::PlainText);
    m_dashboardQueuePreviewLabel->setTextFormat(Qt::PlainText);
    m_dashboardRecentWinnerLabel->setTextFormat(Qt::PlainText);
    note->setTextFormat(Qt::PlainText);

    snapshotLayout->addWidget(m_dashboardNextMatchLabel);
    snapshotLayout->addWidget(m_dashboardQueuePreviewLabel);
    snapshotLayout->addWidget(m_dashboardRecentWinnerLabel);
    snapshotLayout->addWidget(note);

    layout->addLayout(cardsLayout);
    layout->addWidget(quickInfo);
    layout->addWidget(snapshotCard);
    layout->addStretch();

    return page;
}

QWidget* MainWindow::createPlayersPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setSpacing(12);

    QGroupBox* formGroup = new QGroupBox("Player Management", page);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    formLayout->setVerticalSpacing(10);
    formLayout->setHorizontalSpacing(12);
    formLayout->setContentsMargins(12, 16, 12, 12);

    m_idInput = new QLineEdit(formGroup);
    m_nameInput = new QLineEdit(formGroup);
    m_ageInput = new QLineEdit(formGroup);
    m_ageInput->setValidator(new QIntValidator(10, 80, m_ageInput));
    m_gameInput = new QLineEdit(formGroup);
    m_rankInput = new QLineEdit(formGroup);

    formLayout->addRow("Player ID", m_idInput);
    formLayout->addRow("Name", m_nameInput);
    formLayout->addRow("Age", m_ageInput);
    formLayout->addRow("Game", m_gameInput);
    formLayout->addRow("Rank", m_rankInput);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0, 6, 0, 0);
    QPushButton* addButton = new QPushButton("Add", formGroup);
    QPushButton* updateButton = new QPushButton("Update", formGroup);
    QPushButton* deleteButton = new QPushButton("Delete", formGroup);
    QPushButton* clearButton = new QPushButton("Clear", formGroup);

    addButton->setProperty("actionType", "success");
    updateButton->setProperty("actionType", "primary");
    deleteButton->setProperty("actionType", "danger");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(clearButton);
    formLayout->addRow(buttonLayout);

    QGroupBox* searchGroup = new QGroupBox("Search Players", page);
    QHBoxLayout* searchLayout = new QHBoxLayout(searchGroup);
    searchLayout->setContentsMargins(10, 10, 10, 10);
    searchLayout->setSpacing(10);

    m_searchInput = new QLineEdit(searchGroup);
    m_searchInput->setPlaceholderText("Search by ID or Name");

    m_searchByCombo = new QComboBox(searchGroup);
    m_searchByCombo->addItem("ID");
    m_searchByCombo->addItem("Name");

    QPushButton* searchButton = new QPushButton("Search", searchGroup);
    QPushButton* showAllButton = new QPushButton("Show All", searchGroup);

    searchButton->setProperty("actionType", "primary");

    searchLayout->addWidget(m_searchInput, 1);
    searchLayout->addWidget(m_searchByCombo);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(showAllButton);

    m_playersTable = new QTableWidget(page);
    m_playersTable->setColumnCount(5);
    m_playersTable->setHorizontalHeaderLabels(QStringList{ "Player ID", "Name", "Age", "Game", "Rank" });
    m_playersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_playersTable->verticalHeader()->setVisible(false);
    m_playersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_playersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_playersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_playersTable->setAlternatingRowColors(true);

    layout->addWidget(formGroup);
    layout->addWidget(searchGroup);
    layout->addWidget(m_playersTable, 1);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::handleAddPlayer);
    connect(updateButton, &QPushButton::clicked, this, &MainWindow::handleUpdatePlayer);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::handleDeletePlayer);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearPlayerForm);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::handleSearchPlayer);
    connect(showAllButton, &QPushButton::clicked, [this]() {
        m_searchInput->clear();
        refreshPlayersTable(m_system.getAllPlayers());
    });

    return page;
}

QWidget* MainWindow::createQueuePage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QGroupBox* controls = new QGroupBox("Queue Operations", page);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controls);

    m_queuePlayerIdInput = new QLineEdit(controls);
    m_queuePlayerIdInput->setPlaceholderText("Enter Player ID");

    QPushButton* addButton = new QPushButton("Add to Queue", controls);
    QPushButton* peekButton = new QPushButton("Check Next Two", controls);
    QPushButton* resetButton = new QPushButton("Reset Queue", controls);

    addButton->setProperty("actionType", "success");
    resetButton->setProperty("actionType", "danger");

    controlsLayout->addWidget(m_queuePlayerIdInput, 1);
    controlsLayout->addWidget(addButton);
    controlsLayout->addWidget(peekButton);
    controlsLayout->addWidget(resetButton);

    m_queueTable = new QTableWidget(page);
    m_queueTable->setColumnCount(6);
    m_queueTable->setHorizontalHeaderLabels(QStringList{ "Order", "Player ID", "Name", "Age", "Game", "Rank" });
    m_queueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_queueTable->verticalHeader()->setVisible(false);
    m_queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_queueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_queueTable->setAlternatingRowColors(true);

    layout->addWidget(controls);
    layout->addWidget(m_queueTable, 1);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::handleAddToQueue);
    connect(peekButton, &QPushButton::clicked, this, &MainWindow::handlePeekNextTwo);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::handleResetQueue);

    return page;
}

QWidget* MainWindow::createMatchPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setSpacing(12);

    QGroupBox* activeGroup = new QGroupBox("Live Match Arena", page);
    QVBoxLayout* activeLayout = new QVBoxLayout(activeGroup);

    m_activeMatchLabel = new QLabel("No active match.", activeGroup);
    m_activeMatchLabel->setObjectName("ActiveMatchLabel");
    m_activeMatchLabel->setTextFormat(Qt::PlainText);

    QHBoxLayout* arenaLayout = new QHBoxLayout();

    QFrame* p1Card = new QFrame(activeGroup);
    p1Card->setObjectName("PlayerCard");
    QVBoxLayout* p1Layout = new QVBoxLayout(p1Card);

    QLabel* p1Title = new QLabel("PLAYER 1", p1Card);
    p1Title->setObjectName("PlayerCardTitle");
    p1Title->setTextFormat(Qt::PlainText);

    m_player1CardLabel = new QLabel("Waiting...", p1Card);
    m_player1CardLabel->setObjectName("PlayerCardValue");
    m_player1CardLabel->setTextFormat(Qt::PlainText);

    p1Layout->addWidget(p1Title);
    p1Layout->addWidget(m_player1CardLabel);

    QLabel* vsLabel = new QLabel("VS", activeGroup);
    vsLabel->setObjectName("VsLabel");
    vsLabel->setTextFormat(Qt::PlainText);

    QFrame* p2Card = new QFrame(activeGroup);
    p2Card->setObjectName("PlayerCard");
    QVBoxLayout* p2Layout = new QVBoxLayout(p2Card);

    QLabel* p2Title = new QLabel("PLAYER 2", p2Card);
    p2Title->setObjectName("PlayerCardTitle");
    p2Title->setTextFormat(Qt::PlainText);

    m_player2CardLabel = new QLabel("Waiting...", p2Card);
    m_player2CardLabel->setObjectName("PlayerCardValue");
    m_player2CardLabel->setTextFormat(Qt::PlainText);

    p2Layout->addWidget(p2Title);
    p2Layout->addWidget(m_player2CardLabel);

    arenaLayout->addWidget(p1Card, 1);
    arenaLayout->addWidget(vsLabel);
    arenaLayout->addWidget(p2Card, 1);

    m_startMatchButton = new QPushButton("Start Next Match", activeGroup);
    m_startMatchButton->setProperty("actionType", "primary");

    activeLayout->addWidget(m_activeMatchLabel);
    activeLayout->addLayout(arenaLayout);
    activeLayout->addWidget(m_startMatchButton);

    QGroupBox* resultGroup = new QGroupBox("Record Match Result", page);
    QFormLayout* resultLayout = new QFormLayout(resultGroup);


    m_winnerCombo = new QComboBox(resultGroup);
    m_winnerCombo->addItem("Select winner");

    m_scoreInput = new QLineEdit(resultGroup);
    m_scoreInput->setPlaceholderText("2-1");

    m_roundInput = new QLineEdit(resultGroup);
    m_roundInput->setPlaceholderText("Quarter Final");

    m_recordResultButton = new QPushButton("Record Result", resultGroup);
    m_recordResultButton->setProperty("actionType", "success");

    resultLayout->addRow("Winner", m_winnerCombo);
    resultLayout->addRow("Score", m_scoreInput);
    resultLayout->addRow("Round", m_roundInput);
    resultLayout->addRow(m_recordResultButton);

    layout->addWidget(activeGroup);
    layout->addWidget(resultGroup);
    layout->addStretch();

    connect(m_startMatchButton, &QPushButton::clicked, this, &MainWindow::handleStartMatch);
    connect(m_recordResultButton, &QPushButton::clicked, this, &MainWindow::handleRecordResult);

    return page;
}

QWidget* MainWindow::createHistoryPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);

    QGroupBox* historyGroup = new QGroupBox("Match History", page);
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);

    QPushButton* clearButton = new QPushButton("Clear Match History", historyGroup);
    clearButton->setProperty("actionType", "danger");

    m_historyTable = new QTableWidget(historyGroup);
    m_historyTable->setColumnCount(7);
    m_historyTable->setHorizontalHeaderLabels(QStringList{ "Match ID", "Player 1", "Player 2", "Winner", "Score", "Round", "Timestamp" });
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setAlternatingRowColors(true);

    historyLayout->addWidget(clearButton);
    historyLayout->addWidget(m_historyTable, 1);

    layout->addWidget(historyGroup, 1);

    connect(clearButton, &QPushButton::clicked, this, &MainWindow::handleClearHistory);

    return page;
}

QWidget* MainWindow::createWinnersPage()
{
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setSpacing(12);

    QFrame* latestWinnerCard = new QFrame(page);
    latestWinnerCard->setObjectName("LatestWinnerCard");

    QVBoxLayout* latestLayout = new QVBoxLayout(latestWinnerCard);

    QLabel* latestTitle = new QLabel("LATEST WINNER", latestWinnerCard);
    latestTitle->setObjectName("LatestWinnerTitle");
    latestTitle->setTextFormat(Qt::PlainText);

    m_latestWinnerLabel = new QLabel("No winners recorded yet.", latestWinnerCard);
    m_latestWinnerLabel->setObjectName("LatestWinnerValue");
    m_latestWinnerLabel->setTextFormat(Qt::PlainText);

    latestLayout->addWidget(latestTitle);
    latestLayout->addWidget(m_latestWinnerLabel);

    m_winnersTable = new QTableWidget(page);
    m_winnersTable->setColumnCount(5);
    m_winnersTable->setHorizontalHeaderLabels(QStringList{ "Win #", "Player ID", "Name", "Game", "Rank" });
    m_winnersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_winnersTable->verticalHeader()->setVisible(false);
    m_winnersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_winnersTable->setAlternatingRowColors(true);

    layout->addWidget(latestWinnerCard);
    layout->addWidget(m_winnersTable, 1);

    return page;
}

bool MainWindow::readPlayerForm(Player& player, QString& errorMessage) const
{
    player.id = m_idInput->text().trimmed();
    player.name = m_nameInput->text().trimmed();
    player.gameName = m_gameInput->text().trimmed();
    player.rank = m_rankInput->text().trimmed();

    bool ageOk = false;
    const int age = m_ageInput->text().trimmed().toInt(&ageOk);
    if (!ageOk)
    {
        errorMessage = "Age must be numeric.";
        return false;
    }

    player.age = age;
    return true;
}

void MainWindow::clearPlayerForm()
{
    m_idInput->clear();
    m_nameInput->clear();
    m_ageInput->clear();
    m_gameInput->clear();
    m_rankInput->clear();
    m_idInput->setFocus();
}

void MainWindow::handleAddPlayer()
{
    Player player;
    QString error;

    if (!readPlayerForm(player, error))
    {
        showError(error);
        return;
    }

    if (!m_system.addPlayer(player, error))
    {
        showError(error);
        return;
    }

    refreshAllTables();
    showSuccess("Player added successfully.");
}

void MainWindow::handleUpdatePlayer()
{
    Player player;
    QString error;

    if (!readPlayerForm(player, error))
    {
        showError(error);
        return;
    }

    if (!m_system.updatePlayer(player, error))
    {
        showError(error);
        return;
    }

    refreshAllTables();
    updateMatchUiState();
    showSuccess("Player updated successfully.");
}

void MainWindow::handleDeletePlayer()
{
    const QString playerId = m_idInput->text().trimmed();
    if (playerId.isEmpty())
    {
        showError("Enter Player ID to delete.");
        return;
    }

    const QMessageBox::StandardButton choice = QMessageBox::question(
        this,
        "Confirm Delete",
        QString("Delete player with ID %1?").arg(playerId));

    if (choice != QMessageBox::Yes)
        return;

    QString error;
    if (!m_system.deletePlayer(playerId, error))
    {
        showError(error);
        return;
    }

    refreshAllTables();
    updateMatchUiState();
    showSuccess("Player deleted successfully.");
}

void MainWindow::handleSearchPlayer()
{
    const QString keyword = m_searchInput->text().trimmed();
    if (keyword.isEmpty())
    {
        showError("Enter a search value.");
        return;
    }

    const bool searchById = (m_searchByCombo->currentText() == "ID");
    const std::vector<Player> results = m_system.searchPlayers(keyword, searchById);

    refreshPlayersTable(results);
    if (results.empty())
        statusBar()->showMessage("No players found.", 3000);
    else
        statusBar()->showMessage(QString("Found %1 player(s).").arg(results.size()), 3000);
}

void MainWindow::handleAddToQueue()
{
    QString error;
    if (!m_system.enqueuePlayer(m_queuePlayerIdInput->text(), error))
    {
        showError(error);
        return;
    }

    refreshAllTables();
    m_queuePlayerIdInput->clear();
    showSuccess("Player added to queue.");
}

void MainWindow::handlePeekNextTwo()
{
    Player first;
    Player second;
    QString error;

    if (!m_system.peekNextTwo(first, second, error))
    {
        showError(error);
        return;
    }

    QMessageBox::information(
        this,
        "Next Two Players",
        QString("1) %1 (%2)\n2) %3 (%4)").arg(first.name, first.id, second.name, second.id));
}

void MainWindow::handleResetQueue()
{
    const QMessageBox::StandardButton choice = QMessageBox::question(
        this,
        "Confirm Reset",
        "Reset the tournament queue?");

    if (choice != QMessageBox::Yes)
        return;

    m_system.resetQueue();
    refreshAllTables();
    showSuccess("Queue reset successfully.");
}

void MainWindow::handleStartMatch()
{
    QString error;
    if (!m_system.startNextMatch(error))
    {
        showError(error);
        return;
    }

    refreshAllTables();
    updateMatchUiState();
    showSuccess("Match started.");
}

void MainWindow::handleRecordResult()
{
    QString winner = m_winnerCombo->currentText();
    if (winner == "Select winner")
        winner.clear();

    QString error;
    if (!m_system.recordMatchResult(winner, m_scoreInput->text(), m_roundInput->text(), error))
    {
        showError(error);
        return;
    }

    m_scoreInput->clear();
    m_roundInput->clear();
    refreshAllTables();
    updateMatchUiState();
    showSuccess("Match result recorded.");
}

void MainWindow::handleClearHistory()
{
    const QMessageBox::StandardButton choice = QMessageBox::warning(
        this,
        "Confirm Clear",
        "This will clear match history and winners list. Continue?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (choice != QMessageBox::Yes)
        return;

    m_system.clearMatchHistory();
    refreshAllTables();
    showSuccess("Match history cleared.");
}

void MainWindow::handleThemeChanged(const QString& themeName)
{
    QString fileName = "styles.qss";
    if (themeName == "Purple")
        fileName = "styles.qss";

    QFile styleFile(fileName);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        showError(QString("Could not load theme file: %1").arg(fileName));
        return;
    }

    qApp->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    styleFile.close();
}

void MainWindow::refreshDashboardCards()
{
    const std::vector<Player> allPlayers = m_system.getAllPlayers();
    const std::vector<Player> queuePlayers = m_system.getQueuePlayers();
    const std::vector<Match> matches = m_system.getMatchHistory();
    const std::vector<Player> winners = m_system.getWinnersList();

    m_totalPlayersValue->setText(QString::number(allPlayers.size()));
    m_queuePlayersValue->setText(QString::number(queuePlayers.size()));
    m_matchesPlayedValue->setText(QString::number(matches.size()));
    m_winnersCountValue->setText(QString::number(winners.size()));

    Player p1;
    Player p2;
    QString peekError;

    if (m_system.peekNextTwo(p1, p2, peekError))
    {
        m_dashboardNextMatchLabel->setText(QString("• Next Match: %1 vs %2").arg(p1.name, p2.name));
    }
    else if (m_system.hasActiveMatch())
    {
        const auto activeNames = m_system.getActivePlayerNames();
        m_dashboardNextMatchLabel->setText(QString("• Next Match: LIVE %1 vs %2").arg(activeNames.first, activeNames.second));
    }
    else
    {
        m_dashboardNextMatchLabel->setText("• Next Match: Waiting for enough players...");
    }

    if (queuePlayers.empty())
    {
        m_dashboardQueuePreviewLabel->setText("• Queue Preview: No players in queue.");
    }
    else
    {
        QString preview;
        const int count = static_cast<int>(queuePlayers.size()) < 3 ? static_cast<int>(queuePlayers.size()) : 3;
        for (int i = 0; i < count; ++i)
        {
            preview += QString::number(i + 1) + ". " + queuePlayers[static_cast<std::size_t>(i)].name;
            if (i < count - 1)
                preview += "\n";
        }
        m_dashboardQueuePreviewLabel->setText(QString("• Queue Preview:\n%1").arg(preview));
    }

    if (winners.empty())
    {
        m_dashboardRecentWinnerLabel->setText("• Latest Winner: No winner yet.");
    }
    else
    {
        const Player& latest = winners.back();
        m_dashboardRecentWinnerLabel->setText(QString("• Latest Winner: %1 (%2)").arg(latest.name, latest.rank));
    }

    m_readyBadge->setProperty("state", "on");
    m_queueBadge->setProperty("state", queuePlayers.empty() ? "off" : "on");
    m_liveBadge->setProperty("state", m_system.hasActiveMatch() ? "live" : "off");

    m_readyBadge->style()->unpolish(m_readyBadge);
    m_readyBadge->style()->polish(m_readyBadge);
    m_queueBadge->style()->unpolish(m_queueBadge);
    m_queueBadge->style()->polish(m_queueBadge);
    m_liveBadge->style()->unpolish(m_liveBadge);
    m_liveBadge->style()->polish(m_liveBadge);
}

void MainWindow::refreshPlayersTable(const std::vector<Player>& players)
{
    m_playersTable->setRowCount(static_cast<int>(players.size()));

    for (int row = 0; row < static_cast<int>(players.size()); ++row)
    {
        const Player& p = players[static_cast<std::size_t>(row)];
        m_playersTable->setItem(row, 0, new QTableWidgetItem(p.id));
        m_playersTable->setItem(row, 1, new QTableWidgetItem(p.name));
        m_playersTable->setItem(row, 2, new QTableWidgetItem(QString::number(p.age)));
        m_playersTable->setItem(row, 3, new QTableWidgetItem(p.gameName));
        m_playersTable->setItem(row, 4, new QTableWidgetItem(p.rank));
    }
}

void MainWindow::refreshQueueTable()
{
    const std::vector<Player> players = m_system.getQueuePlayers();
    m_queueTable->setRowCount(static_cast<int>(players.size()));

    for (int row = 0; row < static_cast<int>(players.size()); ++row)
    {
        const Player& p = players[static_cast<std::size_t>(row)];
        m_queueTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        m_queueTable->setItem(row, 1, new QTableWidgetItem(p.id));
        m_queueTable->setItem(row, 2, new QTableWidgetItem(p.name));
        m_queueTable->setItem(row, 3, new QTableWidgetItem(QString::number(p.age)));
        m_queueTable->setItem(row, 4, new QTableWidgetItem(p.gameName));
        m_queueTable->setItem(row, 5, new QTableWidgetItem(p.rank));
    }
}

void MainWindow::refreshHistoryTable()
{
    const std::vector<Match> matches = m_system.getMatchHistory();
    m_historyTable->setRowCount(static_cast<int>(matches.size()));

    for (int row = 0; row < static_cast<int>(matches.size()); ++row)
    {
        const Match& match = matches[static_cast<std::size_t>(row)];
        m_historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(match.matchId)));
        m_historyTable->setItem(row, 1, new QTableWidgetItem(match.player1Name));
        m_historyTable->setItem(row, 2, new QTableWidgetItem(match.player2Name));
        m_historyTable->setItem(row, 3, new QTableWidgetItem(match.winnerName));
        m_historyTable->setItem(row, 4, new QTableWidgetItem(match.score));
        m_historyTable->setItem(row, 5, new QTableWidgetItem(match.round));
        m_historyTable->setItem(row, 6, new QTableWidgetItem(match.timestamp));
    }
}

void MainWindow::refreshWinnersTable()
{
    const std::vector<Player> winners = m_system.getWinnersList();
    m_winnersTable->setRowCount(static_cast<int>(winners.size()));

    for (int row = 0; row < static_cast<int>(winners.size()); ++row)
    {
        const Player& player = winners[static_cast<std::size_t>(row)];
        m_winnersTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        m_winnersTable->setItem(row, 1, new QTableWidgetItem(player.id));
        m_winnersTable->setItem(row, 2, new QTableWidgetItem(player.name));
        m_winnersTable->setItem(row, 3, new QTableWidgetItem(player.gameName));
        m_winnersTable->setItem(row, 4, new QTableWidgetItem(player.rank));
    }

    if (winners.empty())
    {
        m_latestWinnerLabel->setProperty("winnerState", "empty");
        m_latestWinnerLabel->setText("No winners recorded yet.");
    }
    else
    {
        const Player& latest = winners.back();
        m_latestWinnerLabel->setProperty("winnerState", "featured");
        m_latestWinnerLabel->setText(QString("%1  •  %2").arg(latest.name, latest.rank));
    }

    m_latestWinnerLabel->style()->unpolish(m_latestWinnerLabel);
    m_latestWinnerLabel->style()->polish(m_latestWinnerLabel);
}

void MainWindow::refreshAllTables()
{
    refreshDashboardCards();
    refreshPlayersTable(m_system.getAllPlayers());
    refreshQueueTable();
    refreshHistoryTable();
    refreshWinnersTable();
}

void MainWindow::updateMatchUiState()
{
    m_activeMatchLabel->setText(m_system.getActiveMatchDisplay());

    m_winnerCombo->clear();
    m_winnerCombo->addItem("Select winner");

    const bool active = m_system.hasActiveMatch();
    if (active)
    {
        const std::pair<QString, QString> players = m_system.getActivePlayerNames();
        m_player1CardLabel->setText(players.first);
        m_player2CardLabel->setText(players.second);
        m_winnerCombo->addItem(players.first);
        m_winnerCombo->addItem(players.second);
    }
    else
    {
        m_player1CardLabel->setText("Waiting...");
        m_player2CardLabel->setText("Waiting...");
    }

    m_startMatchButton->setEnabled(!active);
    m_recordResultButton->setEnabled(active);
    m_winnerCombo->setEnabled(active);
    m_scoreInput->setEnabled(active);
    m_roundInput->setEnabled(active);
}

void MainWindow::showError(const QString& message)
{
    statusBar()->showMessage(message, 4000);
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::showSuccess(const QString& message)
{
    statusBar()->showMessage(message, 3000);
}