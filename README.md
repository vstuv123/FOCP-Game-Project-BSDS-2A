# TicTacToe Game

## Group Members
- **Muhammad Abdullah**
- **Zaid Abbasi**
- **Ubaid Sukera**

## Project Description
The TicTacToe Game is a GUI-based implementation of the classic TicTacToe game using **SFML (Simple and Fast Multimedia Library)** for graphical rendering. It provides a fun and interactive way for users to enjoy the game in a modern interface with additional features and customization options. 

This game supports multiplayer mode and is designed for players who wish to challenge themselves or their friends in a best-of-series format. Players can also choose the difficulty level and grid size to match their preferences.

## Features
- **Multiplayer Mode**: Two players can play against each other.
- **Best-of-Series Option**: Choose to play best-of-1, best-of-3, or best-of-5 match games.
- **Difficulty Levels**:
  - **Easy**: 3x3 grid.
  - **Medium**: 4x4 grid.
  - **Hard**: 6x6 grid.
- **Replay Functionality**: Play multiple matches in the same mode and difficulty without restarting.
- **In-Game Instructions**: Access detailed game rules and instructions at any time.
- **User-Friendly GUI**: Enjoy a clean and intuitive graphical interface powered by SFML.

## Technical Architecture
### Technology Stack
- **Programming Language**: C++
- **GUI Framework**: SFML (Simple and Fast Multimedia Library)
- **Development Tools**: Visual Studio

### Architecture Overview
1. **Game Engine**:
   - Core logic for handling the game state, player turns, and win/draw detection.
   - Implementation of difficulty levels and grid size handling.
2. **GUI Layer**:
   - Rendering of game board, buttons, and other visual elements.
   - Handling user interactions such as clicks and keypresses.
3. **Utility Module**:
   - Functions for handling game resets, scorekeeping, and match results.
4. **Instructions Module**:
   - Displays in-game instructions and rules for players.

### Data Flow
- **Input**: Player interactions (mouse clicks or keyboard inputs).
- **Processing**: Game engine updates the board and checks for win/draw conditions.
- **Output**: GUI updates the game board and displays relevant messages or instructions.

## How to Run and Deploy

### Prerequisites
- Install **SFML** (Simple and Fast Multimedia Library).
- Install **Visual Studio** and set it up for C++ development.

### Steps to Run
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd TicTacToe
   ```
2. Set up SFML in Visual Studio:
   - Download and install SFML from [SFML's official website](https://www.sfml-dev.org/).
   - Configure your Visual Studio project to include SFML headers and link SFML libraries.
3. Build the project:
   - Open the project in Visual Studio.
   - Configure the include and library directories for SFML.
   - Build the project using the Visual Studio IDE.
4. Run the executable:
   - Use the "Run" option in Visual Studio to execute the game.

### Deployment
To distribute the game:
1. Package the built executable along with the required SFML libraries.
2. Create a zip file containing:
   - The game executable.
   - Required SFML `.dll` or `.so` files.
   - A README file with running instructions.
3. Share the zip file with users.

## Game Instructions
- The objective of the game is to align your symbols (X or O) in a row, column, or diagonal.
- Players take turns placing their symbols on the grid.
- The match ends when one player wins or the grid is full (resulting in a draw).
- Depending on the difficulty level, the grid size changes:
  - Easy: 3x3.
  - Medium: 4x4.
  - Hard: 6x6.
- After each game, players can choose to replay or exit.
---

Enjoy the game and happy playing!
