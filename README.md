# 🎮 E-Sports Control Manager

A tournament management system implemented in **C++**, exposing a local web interface through a custom HTTP server.

The project models real-world tournament operations including player management, matchmaking, match execution, and result tracking using fundamental data structures and system-level programming.

---

## 🧠 Overview

This system provides a complete workflow for managing an e-sports tournament:

- Player registration and maintenance  
- Game-based matchmaking queue  
- Match lifecycle handling  
- Result recording and statistics updates  
- Persistent match history and winner tracking  

The backend is implemented entirely in **C++**, serving a browser-based interface via `localhost`.

---

## ⚙️ Tech Stack

- **Language:** C++  
- **Networking:** Winsock (custom HTTP server)  
- **Frontend:** HTML / CSS / JavaScript  
- **Architecture:** Local client-server model  

---

## 🚀 Core Features

### 👥 Player Management
- Add, update, and delete players  
- Store player attributes (ID, name, age, game, rank)  
- Maintain dynamic statistics (wins, losses, win rate)  

---

### 🎯 Queue System
- FIFO queue implementation  
- Queue is **restricted by game type**  
- Prevents invalid cross-game matchmaking  
- Supports real-time updates  

---

### ⚔️ Match System
- Automatically selects players from the queue  
- Handles full match lifecycle (start → result)  
- Records winner, score, and round  
- Updates player statistics immediately  

---

### 🏆 History & Winners
- Match history stored using a **linked list**  
- Maintains complete match records  
- Tracks winners across all matches  

---

## 🏗️ System Architecture

```
C++ Backend (Custom HTTP Server)
        ↓
HTTP Requests (localhost)
        ↓
Frontend Interface (Browser)
        ↓
Dynamic Data Rendering
```

---

## 🧩 Data Structures

- **Queue** → Matchmaking system  
- **Linked List** → Match history & winners  
- **Classes / Structs** → Player and Match models  

---

## ▶️ How to Run

### Requirements
- Windows OS  
- MinGW / g++  

---

### Compile

```bash
g++ -std=c++14 main.cpp -o EsportsManager.exe -lws2_32
```

---

### Run

```bash
EsportsManager.exe
```

---

### Open in Browser

```
http://localhost:8080
```

---

## 🔮 Future Improvements

- Database integration (SQLite / MySQL)  
- Authentication system  
- Online deployment  
- Rank-based matchmaking system  
- Real-time updates using WebSockets  
