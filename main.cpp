
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;


#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socket_t = SOCKET;
static constexpr socket_t BAD_SOCKET = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
static constexpr socket_t BAD_SOCKET = -1;
#endif

static bool running = true;

static void closeSocket(socket_t s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

template <typename T>
struct Node {
    T data;
    Node* next = nullptr;
    explicit Node(const T& value) : data(value) {}
};

template <typename T>
class LinkedList {
public:
    ~LinkedList() { clear(); }

    LinkedList() = default;
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void pushBack(const T& value) {
        Node<T>* n = new Node<T>(value);
        if (!tail) head = tail = n;
        else { tail->next = n; tail = n; }
        ++count;
    }

    template <typename Pred>
    bool removeFirstWhere(Pred pred) {
        Node<T>* prev = nullptr;
        Node<T>* cur = head;
        while (cur) {
            if (pred(cur->data)) {
                if (prev) prev->next = cur->next;
                else head = cur->next;
                if (cur == tail) tail = prev;
                delete cur;
                --count;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    template <typename Pred>
    T* findFirst(Pred pred) {
        Node<T>* cur = head;
        while (cur) {
            if (pred(cur->data)) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    template <typename Pred>
    const T* findFirst(Pred pred) const {
        const Node<T>* cur = head;
        while (cur) {
            if (pred(cur->data)) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    vector<T> toVector() const {
        vector<T> v;
        v.reserve(count);
        const Node<T>* cur = head;
        while (cur) {
            v.push_back(cur->data);
            cur = cur->next;
        }
        return v;
    }

    void clear() {
        Node<T>* cur = head;
        while (cur) {
            Node<T>* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        count = 0;
    }

private:
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;
    size_t count = 0;
};

struct Player {
    string id;
    string name;
    int age = 0;
    string gameName;
    string rank;
    int wins = 0;
    int losses = 0;

    double winRate() const {
        const int total = wins + losses;
        return total == 0 ? 0.0 : (wins * 100.0) / total;
    }
};

struct Match {
    int matchId = 0;
    string player1Name;
    string player2Name;
    string winnerName;
    string score;
    string round;
    string timestamp;
};

class QueueManager {
public:
    ~QueueManager() { clear(); }

    bool enqueue(const string& id) {
        if (id.empty() || contains(id)) return false;
        Node<string>* n = new Node<string>(id);
        if (!rear) front = rear = n;
        else { rear->next = n; rear = n; }
        ++count;
        return true;
    }

    bool dequeue(string& id) {
        if (!front) return false;
        Node<string>* old = front;
        id = old->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete old;
        --count;
        return true;
    }

    bool peekFront(string& id) const {
        if (!front) return false;
        id = front->data;
        return true;
    }

    bool peekSecond(string& id) const {
        if (!front || !front->next) return false;
        id = front->next->data;
        return true;
    }

    bool contains(const string& id) const {
        const Node<string>* cur = front;
        while (cur) {
            if (cur->data == id) return true;
            cur = cur->next;
        }
        return false;
    }

    bool removeById(const string& id) {
        Node<string>* prev = nullptr;
        Node<string>* cur = front;
        while (cur) {
            if (cur->data == id) {
                if (prev) prev->next = cur->next;
                else front = cur->next;
                if (cur == rear) rear = prev;
                delete cur;
                --count;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    vector<string> snapshot() const {
        vector<string> ids;
        const Node<string>* cur = front;
        while (cur) {
            ids.push_back(cur->data);
            cur = cur->next;
        }
        return ids;
    }

    void clear() {
        Node<string>* cur = front;
        while (cur) {
            Node<string>* next = cur->next;
            delete cur;
            cur = next;
        }
        front = rear = nullptr;
        count = 0;
    }

private:
    Node<string>* front = nullptr;
    Node<string>* rear = nullptr;
    int count = 0;
};

class TournamentSystem {
public:
    TournamentSystem() { loadSampleData(); }

    bool addPlayer(const Player& p, string& error) {
        if (!validatePlayer(p, error)) return false;
        if (findPlayerById(p.id)) {
            error = "Player ID must be unique.";
            return false;
        }
        players.pushBack(p);
        return true;
    }

    bool updatePlayer(const Player& p, string& error) {
        if (!validatePlayer(p, error)) return false;
        Player* existing = findPlayerById(p.id);
        if (!existing) {
            error = "Player not found.";
            return false;
        }
        existing->name = p.name;
        existing->age = p.age;
        existing->gameName = p.gameName;
        existing->rank = p.rank;
        return true;
    }

    bool deletePlayer(const string& id, string& error) {
        if (id.empty()) {
            error = "Player ID is required.";
            return false;
        }
        if (matchInProgress && (active1.id == id || active2.id == id)) {
            error = "Cannot delete a player in an active match.";
            return false;
        }
        queue.removeById(id);
        bool removed = players.removeFirstWhere([&](const Player& p){ return p.id == id; });
        if (!removed) {
            error = "Player not found.";
            return false;
        }
        return true;
    }

    bool enqueuePlayer(const string& id, string& error) {
        if (id.empty()) {
            error = "Player ID is required.";
            return false;
        }

        Player* newPlayer = findPlayerById(id);
        if (!newPlayer) {
            error = "Register player first before queuing.";
            return false;
        }

        const vector<string> queuedIds = queue.snapshot();
        if (!queuedIds.empty()) {
            const Player* firstQueuedPlayer = findPlayerById(queuedIds.front());

            if (firstQueuedPlayer && firstQueuedPlayer->gameName != newPlayer->gameName) {
                error = "Queue is locked to " + firstQueuedPlayer->gameName +
                        ". Only players registered for the same game can join this queue.";
                return false;
            }
        }

        if (!queue.enqueue(id)) {
            error = "Player is already in queue.";
            return false;
        }
        return true;
    }

    vector<Player> getAllPlayers() const { return players.toVector(); }

    vector<Player> getQueuePlayers() const {
        vector<Player> result;
        for (const auto& id : queue.snapshot()) {
            const Player* p = findPlayerById(id);
            if (p) result.push_back(*p);
        }
        return result;
    }

    bool peekNextTwo(Player& a, Player& b, string& error) const {
        string id1, id2;
        if (!queue.peekFront(id1) || !queue.peekSecond(id2)) {
            error = "At least two players are required.";
            return false;
        }
        const Player* p1 = findPlayerById(id1);
        const Player* p2 = findPlayerById(id2);
        if (!p1 || !p2) {
            error = "Queue contains invalid player references.";
            return false;
        }
        a = *p1;
        b = *p2;
        return true;
    }

    void resetQueue() { queue.clear(); }

    bool startNextMatch(string& error) {
        if (matchInProgress) {
            error = "Finish active match before starting a new one.";
            return false;
        }

        string id1, id2;
        if (!queue.dequeue(id1) || !queue.dequeue(id2)) {
            if (!id1.empty()) queue.enqueue(id1);
            error = "At least two players are required to start a match.";
            return false;
        }

        Player* p1 = findPlayerById(id1);
        Player* p2 = findPlayerById(id2);

        if (!p1 || !p2) {
            error = "Invalid queued players encountered.";
            return false;
        }

        active1 = *p1;
        active2 = *p2;
        matchInProgress = true;
        return true;
    }

    bool recordMatchResult(const string& winner, const string& score, const string& round, string& error) {
        if (!matchInProgress) {
            error = "No active match to record.";
            return false;
        }
        if (winner.empty() || score.empty() || round.empty()) {
            error = "Winner, score, and round are required.";
            return false;
        }
        if (winner != active1.name && winner != active2.name) {
            error = "Winner must be one of the active players.";
            return false;
        }

        Player* p1 = findPlayerById(active1.id);
        Player* p2 = findPlayerById(active2.id);

        if (!p1 || !p2) {
            error = "Active players are no longer available.";
            return false;
        }

        if (winner == p1->name) {
            p1->wins++;
            p2->losses++;
            winners.pushBack(*p1);
        } else {
            p2->wins++;
            p1->losses++;
            winners.pushBack(*p2);
        }

        Match m;
        m.matchId = nextMatchId++;
        m.player1Name = active1.name;
        m.player2Name = active2.name;
        m.winnerName = winner;
        m.score = score;
        m.round = round;
        m.timestamp = "Current Session";
        history.pushBack(m);

        matchInProgress = false;
        active1 = Player{};
        active2 = Player{};
        return true;
    }

    bool hasActiveMatch() const { return matchInProgress; }

    pair<string, string> getActivePlayerNames() const {
        if (!matchInProgress) return {"", ""};
        return {active1.name, active2.name};
    }

    vector<Match> getMatchHistory() const { return history.toVector(); }
    vector<Player> getWinnersList() const { return winners.toVector(); }

    void clearMatchHistory() {
        history.clear();
        winners.clear();
    }

private:
    bool validatePlayer(const Player& p, string& error) const {
        if (p.id.empty()) { error = "Player ID cannot be empty."; return false; }
        if (p.name.empty()) { error = "Player name cannot be empty."; return false; }
        if (p.age < 10 || p.age > 80) { error = "Age must be between 10 and 80."; return false; }
        if (p.gameName.empty()) { error = "Game name cannot be empty."; return false; }
        if (p.rank.empty()) { error = "Rank cannot be empty."; return false; }
        return true;
    }

    Player* findPlayerById(const string& id) {
        return players.findFirst([&](const Player& p){ return p.id == id; });
    }

    const Player* findPlayerById(const string& id) const {
        return players.findFirst([&](const Player& p){ return p.id == id; });
    }

    void loadSampleData() {
        string ignored;
        addPlayer({"P001", "Ayaan", 20, "Valorant", "Diamond"}, ignored);
        addPlayer({"P002", "Sara", 21, "Dota 2", "Immortal"}, ignored);
        addPlayer({"P003", "Omar", 19, "League", "Gold"}, ignored);
        addPlayer({"P004", "Lina", 22, "CS2", "Master"}, ignored);
    }

    LinkedList<Player> players;
    LinkedList<Match> history;
    LinkedList<Player> winners;
    QueueManager queue;
    bool matchInProgress = false;
    Player active1;
    Player active2;
    int nextMatchId = 1;
};

static string escapeJson(const string& s) {
    string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}

static string jstr(const string& s) {
    return "\"" + escapeJson(s) + "\"";
}

static string playerJson(const Player& p) {
    ostringstream o;
    o << "{"
      << "\"id\":" << jstr(p.id) << ","
      << "\"name\":" << jstr(p.name) << ","
      << "\"age\":" << p.age << ","
      << "\"gameName\":" << jstr(p.gameName) << ","
      << "\"rank\":" << jstr(p.rank) << ","
      << "\"wins\":" << p.wins << ","
      << "\"losses\":" << p.losses << ","
      << "\"winRate\":" << p.winRate()
      << "}";
    return o.str();
}

static string playersJson(const vector<Player>& players) {
    ostringstream o;
    o << "[";
    for (size_t i = 0; i < players.size(); ++i) {
        if (i) o << ",";
        o << playerJson(players[i]);
    }
    o << "]";
    return o.str();
}

static string matchesJson(const vector<Match>& matches) {
    ostringstream o;
    o << "[";
    for (size_t i = 0; i < matches.size(); ++i) {
        if (i) o << ",";
        const auto& m = matches[i];
        o << "{"
          << "\"matchId\":" << m.matchId << ","
          << "\"player1Name\":" << jstr(m.player1Name) << ","
          << "\"player2Name\":" << jstr(m.player2Name) << ","
          << "\"winnerName\":" << jstr(m.winnerName) << ","
          << "\"score\":" << jstr(m.score) << ","
          << "\"round\":" << jstr(m.round) << ","
          << "\"timestamp\":" << jstr(m.timestamp)
          << "}";
    }
    o << "]";
    return o.str();
}

static string urlDecode(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            string hex = s.substr(i + 1, 2);
            out += static_cast<char>(strtol(hex.c_str(), nullptr, 16));
            i += 2;
        } else if (s[i] == '+') out += ' ';
        else out += s[i];
    }
    return out;
}

static map<string, string> parseForm(const string& body) {
    map<string, string> m;
    stringstream ss(body);
    string pair;
    while (getline(ss, pair, '&')) {
        size_t eq = pair.find('=');
        if (eq == string::npos) continue;
        m[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
    }
    return m;
}

static string readFile(const string& path) {
    ifstream f(path, ios::binary);
    if (!f) return "";
    ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static bool endsWith(const string& s, const string& suffix) {
    return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static string mime(const string& path) {
    if (endsWith(path, ".html")) return "text/html; charset=utf-8";
    if (endsWith(path, ".css")) return "text/css; charset=utf-8";
    if (endsWith(path, ".js")) return "application/javascript; charset=utf-8";
    return "text/plain; charset=utf-8";
}

static string response(int code, const string& status, const string& type, const string& body) {
    ostringstream r;
    r << "HTTP/1.1 " << code << " " << status << "\r\n"
      << "Content-Type: " << type << "\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "Connection: close\r\n"
      << "Access-Control-Allow-Origin: *\r\n\r\n"
      << body;
    return r.str();
}

static string ok() { return "{\"success\":true}"; }
static string err(const string& e) { return "{\"success\":false,\"error\":" + jstr(e) + "}"; }

static string dashboardJson(TournamentSystem& sys) {
    auto players = sys.getAllPlayers();
    auto queue = sys.getQueuePlayers();
    auto history = sys.getMatchHistory();
    auto winners = sys.getWinnersList();

    sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        if (a.wins == b.wins) return a.losses < b.losses;
        return a.wins > b.wins;
    });

    const Player* best = nullptr;
    for (const auto& p : players) {
        if (p.wins + p.losses == 0) continue;
        if (!best || p.winRate() > best->winRate()) best = &p;
    }

    Player p1, p2;
    string error;
    bool hasNext = sys.peekNextTwo(p1, p2, error);

    ostringstream o;
    o << "{"
      << "\"totalPlayers\":" << players.size() << ","
      << "\"queueCount\":" << queue.size() << ","
      << "\"queueGame\":" << (queue.empty() ? "null" : jstr(queue.front().gameName)) << ","
      << "\"matchesPlayed\":" << history.size() << ","
      << "\"winnersCount\":" << winners.size() << ","
      << "\"hasActiveMatch\":" << (sys.hasActiveMatch() ? "true" : "false") << ",";

    o << "\"topPlayer\":" << (players.empty() ? "null" : playerJson(players[0])) << ",";
    o << "\"bestWinRate\":" << (best ? playerJson(*best) : "null") << ",";

    if (sys.hasActiveMatch()) {
        auto names = sys.getActivePlayerNames();
        o << "\"activeMatch\":{\"player1\":" << jstr(names.first) << ",\"player2\":" << jstr(names.second) << "},";
    } else {
        o << "\"activeMatch\":null,";
    }

    if (hasNext) {
        o << "\"nextMatch\":{\"player1\":" << playerJson(p1) << ",\"player2\":" << playerJson(p2) << "}";
    } else {
        o << "\"nextMatch\":null";
    }

    o << "}";
    return o.str();
}

static string apiGet(const string& path, TournamentSystem& sys) {
    if (path == "/api/dashboard") return dashboardJson(sys);
    if (path == "/api/players") return playersJson(sys.getAllPlayers());
    if (path == "/api/queue") return playersJson(sys.getQueuePlayers());
    if (path == "/api/history") return matchesJson(sys.getMatchHistory());
    if (path == "/api/winners") return playersJson(sys.getWinnersList());
    if (path == "/api/match") {
        ostringstream o;
        o << "{\"active\":" << (sys.hasActiveMatch() ? "true" : "false");
        if (sys.hasActiveMatch()) {
            auto names = sys.getActivePlayerNames();
            o << ",\"player1\":" << jstr(names.first) << ",\"player2\":" << jstr(names.second);
        }
        o << "}";
        return o.str();
    }
    return err("Unknown endpoint");
}

static string apiPost(const string& path, const string& body, TournamentSystem& sys) {
    auto f = parseForm(body);
    string error;
    auto v = [&](const string& key) { return f.count(key) ? f[key] : ""; };

    if (path == "/api/players/add") {
        Player p{v("id"), v("name"), atoi(v("age").c_str()), v("gameName"), v("rank")};
        return sys.addPlayer(p, error) ? ok() : err(error);
    }

    if (path == "/api/players/update") {
        Player p{v("id"), v("name"), atoi(v("age").c_str()), v("gameName"), v("rank")};
        return sys.updatePlayer(p, error) ? ok() : err(error);
    }

    if (path == "/api/players/delete") return sys.deletePlayer(v("id"), error) ? ok() : err(error);
    if (path == "/api/queue/add") return sys.enqueuePlayer(v("id"), error) ? ok() : err(error);
    if (path == "/api/queue/reset") { sys.resetQueue(); return ok(); }
    if (path == "/api/match/start") return sys.startNextMatch(error) ? ok() : err(error);
    if (path == "/api/match/result") return sys.recordMatchResult(v("winner"), v("score"), v("round"), error) ? ok() : err(error);
    if (path == "/api/history/clear") { sys.clearMatchHistory(); return ok(); }

    return err("Unknown endpoint");
}

static void handleClient(socket_t client, TournamentSystem& sys) {
    char buffer[8192];
    int n = recv(client, buffer, sizeof(buffer), 0);
    if (n <= 0) { closeSocket(client); return; }

    string request(buffer, buffer + n);
    size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == string::npos) { closeSocket(client); return; }

    string header = request.substr(0, headerEnd);
    string body = request.substr(headerEnd + 4);

    istringstream hs(header);
    string method, path, version;
    hs >> method >> path >> version;

    int contentLength = 0;
    string line;
    while (getline(hs, line)) {
        auto pos = line.find("Content-Length:");
        if (pos != string::npos) contentLength = atoi(line.substr(pos + 15).c_str());
    }

    while ((int)body.size() < contentLength) {
        n = recv(client, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        body.append(buffer, buffer + n);
    }

    string res;

    if (path.rfind("/api/", 0) == 0) {
        string json = method == "GET" ? apiGet(path, sys) : apiPost(path, body, sys);
        res = response(200, "OK", "application/json; charset=utf-8", json);
    } else {
        string filePath = path == "/" ? "/index.html" : path;
        if (filePath.find("..") != string::npos) {
            res = response(403, "Forbidden", "text/plain", "Forbidden");
        } else {
            string content = readFile("web" + filePath);
            if (content.empty()) res = response(404, "Not Found", "text/plain", "Not found");
            else res = response(200, "OK", mime(filePath), content);
        }
    }

    send(client, res.c_str(), (int)res.size(), 0);
    closeSocket(client);
}

static void stop(int) { running = false; }

int main() {
    signal(SIGINT, stop);

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    TournamentSystem system;

    socket_t server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == BAD_SOCKET) {
        cerr << "Could not create socket.\n";
        return 1;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    if (bind(server, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "Port 8080 is busy.\n";
        closeSocket(server);
        return 1;
    }

    listen(server, 16);

    cout << "Running at http://localhost:8080\n";
    cout << "Press Ctrl+C to stop.\n";

    while (running) {
        sockaddr_in clientAddr{};
#ifdef _WIN32
        int len = sizeof(clientAddr);
#else
        socklen_t len = sizeof(clientAddr);
#endif
        socket_t client = accept(server, (sockaddr*)&clientAddr, &len);
        if (client != BAD_SOCKET) handleClient(client, system);
    }

    closeSocket(server);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
