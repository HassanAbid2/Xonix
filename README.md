# XONIX — DSA Final Project

> A fully-featured territory-capture arcade game built in **C++ with SFML**, demonstrating core Data Structures & Algorithms through real gameplay mechanics.

---

## Table of Contents

- [About the Game](#about-the-game)
- [Game Modes](#game-modes)
- [Controls](#controls)
- [Features](#features)
- [Data Structures Used](#data-structures-used)
- [Algorithms Used](#algorithms-used)
- [Project Structure](#project-structure)
- [How to Build](#how-to-build)
- [Team](#team)

---

## About the Game

XONIX is a classic arcade game reimagined with a complete authentication system, leaderboards, multiplayer, inventory, and save/load functionality. The player controls a character that carves out territory on a 50×80 grid while avoiding enemies. Capture enough of the board to advance to the next level.

The game serves as a practical demonstration of how **Data Structures and Algorithms** power real software — every major feature maps directly to a DSA concept covered in the course.

---

## Game Modes

| Mode | Description |
|---|---|
| **Single Player** | Classic Xonix. Capture territory, avoid enemies, beat your high score. |
| **Local Multiplayer** | Two players on one keyboard — WASD vs Arrow keys. 60-second timed match. |
| **Ranked Matchmaking** | Skill-based opponent pairing using a priority queue. |

---

## Controls

### Single Player
| Key | Action |
|---|---|
| Arrow Keys | Move player |
| `P` | Activate power-up (freezes enemies for 3 seconds) |
| `Ctrl + S` | Save game |
| `Esc` | Pause |
| `R` | Reset current game |

### Local Multiplayer
| Player | Move | Power-up |
|---|---|---|
| Player 1 | `W A S D` | `X` |
| Player 2 | Arrow Keys | `M` |

---

## Features

- **Account system** — sign up, log in, password validation
- **Persistent profiles** — scores, wins, losses, and power-ups saved between sessions
- **Save & Load** — save mid-game state (grid, enemies, score) and resume later
- **Leaderboard** — top 10 players ranked by high score
- **Inventory / Theme system** — 10 selectable map backgrounds with live preview and search
- **Friend system** — send and receive friend requests, manage a friend list
- **Power-ups** — earned by scoring; freeze all enemies for 3 seconds
- **Level progression** — enemy count increases each level; 45 %+ capture triggers next level
- **Animated transitions** — opening screen, level-complete sweep, multiplayer VS screen

---

## Data Structures Used

### 1. Hash Table — `HashTable.h`
Used for **O(1) player lookup by ID** across the entire player database.

- Custom implementation with separate chaining
- Maps `playerID → array index` for instant access without linear search
- Used every time a player logs in or a friend request is resolved

### 2. AVL Tree — `AVLTree.h` / `AVLTree.cpp`
Used to store and retrieve the **10 inventory themes** in sorted order.

- Self-balancing BST; guarantees **O(log n)** search for theme by ID or name
- Automatic rotations (left/right) keep the tree balanced after every insert
- Supports both `searchByID()` and `searchByName()` traversals

### 3. Min-Heap — `MinHeap.h`
Powers the **leaderboard**: efficiently tracks the top 10 high scores.

- `insert()` in **O(log n)**; `getTopEntries()` returns sorted top-10
- Heapify-up/down maintains the heap property after every update
- Scores are loaded fresh from the player database on each leaderboard open

### 4. Priority Queue — `PriorityQueue.h`
Used for **skill-based matchmaking** in ranked multiplayer.

- Wraps a min-heap; players are inserted with a priority derived from their total points
- `getBestMatch()` returns the opponent closest in skill level to the queued player
- Ensures fair pairing without full O(n²) comparison

### 5. Singly Linked List — `FriendSystem.h` / `FriendSystem.cpp` / `LinkedList.cpp`
Every player holds **two linked lists**: a friends list and a pending-requests list.

- `FriendList` — supports `addFriend()`, `removeFriend()`, `isFriend()`, `validate()`
- `FriendRequestList` — supports `addRequest()`, `hasRequest()`, `removeRequest()`
- Copy constructor and assignment operator are properly implemented to prevent shallow-copy bugs
- O(n) lookup is acceptable here as friend lists are small and traversed infrequently

### 6. Dynamic Array — `PlayerData.h` (`PlayerArray`)
Holds the entire player database in memory with **amortized O(1) append**.

- Doubles capacity when full (similar to `std::vector`)
- Stores raw `Player` objects contiguously for cache-friendly iteration
- Used by the leaderboard, hash table, and login screen

### 7. 2D Array (Grid) — `Global.h` / `Global.cpp`
The **50×80 game grid** is the core game state, represented as a global `int grid[50][80]`.

| Cell Value | Meaning |
|---|---|
| `0` | Empty |
| `1` | Captured territory (Player 1 / border) |
| `2` | Player 1 trail |
| `3` | Captured territory (Player 2) |
| `4` | Player 2 trail |
| `-1` / `-2` | Flood-fill markers (transient, during capture resolution) |

---

## Algorithms Used

### Iterative Flood Fill — `GameLogic.h`
The heart of the capture mechanic. When a player closes a loop and returns to the border, a flood fill determines which regions enemies can reach.

- **Iterative BFS/DFS** using an explicit stack (arrays of size `M×N`) — avoids stack overflow on large empty grids
- Called once per enemy per capture event: `drop(enemyY/ts, enemyX/ts)`
- Cells reachable from any enemy are marked `-1` (enemy territory); all remaining `0` and trail cells are converted to `1` (captured)
- Time complexity: **O(M×N)** per fill

```
Player closes loop → drop() from each enemy → mark reachable cells -1
→ sweep grid: -1 → 0  (enemy zone stays empty)
             0/2 → 1  (player captures everything else)
```

### Level Threshold Formula
```
threshold = 0.45 + 0.1 × (level × 0.5)
```
Each level requires a slightly higher percentage of the board to be captured before advancing.

### Scoring with Multipliers — `Player.h`
```
tiles ≥ 10           →  2× base score
reward streak + 5+   →  stacking 2× bonus
power-up active      →  enemies frozen (no score modifier)
```
Power-ups unlock at cumulative score thresholds: 50 → 80 → 110 → …

### Save / Load Serialization — `SaveLoad.h` / `SaveLoad.cpp`
Game state is serialized to a human-readable CSV-style file:

```
Level:1
Grid:              ← 50 rows × 80 comma-separated integers
PlayerX:16
PlayerY:0
EnemyCount:3
Enemies:1228,720,4,-3;571,483,-2,4;677,174,3,-1   ← x,y,dx,dy per enemy
PowerupActive:0
PlayerData:        ← player ID, username, score, high score
```

Backwards-compatible: old saves without `dx,dy` load fine (enemies get fresh random velocities).

---

## Project Structure

```
Xonix/
├── Main.cpp              # Game loop & state machine (29 states)
├── Game.h                # All UI screens (Login, MainMenu, Inventory, etc.)
├── GameLogic.h           # Single-player & multiplayer game loops
├── GameState.h           # Snapshot struct for save/load
│
├── Player.h              # Player model: stats, scoring, power-ups
├── PlayerData.h          # CSV persistence layer (dynamic array)
├── Enemy.h               # Enemy AI with wall-bounce movement
│
├── HashTable.h           # Player ID → array index lookup
├── AVLTree.h / .cpp      # Self-balancing BST for inventory themes
├── MinHeap.h             # Top-10 leaderboard
├── PriorityQueue.h       # Skill-based matchmaking
├── FriendSystem.h / .cpp # Linked list friend & request management
├── LinkedList.cpp        # Linked list base implementation
│
├── Inventory.h / .cpp    # Theme manager (wraps AVL tree)
├── SaveLoad.h / .cpp     # Serialize / deserialize game state
├── Global.h / .cpp       # Shared grid (50×80) and constants
├── Text.h / .cpp         # SFML text rendering helpers
│
├── PlayerData.csv        # Persistent player database
├── Themes.csv            # Theme definitions
├── SavedGames/           # Per-player save files (Game_State_<id>.csv)
│
├── Images/               # All game sprites, UI elements, backgrounds
├── fonts/                # Custom typefaces (Noxis, Joystix, Saphira, …)
└── 5050.ogg              # Background music
```

---

## How to Build

**Requirements**
- Windows 10/11
- Visual Studio 2022
- [SFML 2.6](https://www.sfml-dev.org/download.php) (linked as a dependency)

**Steps**
1. Clone the repository
2. Open `Xonix.vcxproj` in Visual Studio
3. Configure SFML include/library paths in project properties if needed
4. Build → Run (`F5`)

> The executable must be run from the project root so it can find `Images/`, `fonts/`, and `PlayerData.csv` using relative paths.

---

## Team

**Group 39 — FAST NUCES**

| Name | Role |
|---|---|
| Talha | Hash Table · Save/Load · Game Logic |
| Syed | AVL Tree · Inventory System |
| Hamza | Linked List · Friend System |
| Areeba | Priority Queue · Matchmaking |
| Hassan | Min Heap · Leaderboard · UI Screens |

---

*Built as a Final Project for CS-2001 Data Structures & Algorithms — Spring 2025*
