#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

// Game variables
sf::SoundBuffer moveBuffer, winBuffer, drawBuffer, buttonClickBuffer;  // Buffer to hold sound file
sf::Sound moveSound, winSound, drawSound, buttonClickSound;         // Sound object to play the move sound
sf::RenderWindow window(sf::VideoMode(800, 660), "Tic Tac Toe");
sf::Font font;  // Default font used
sf::Text welcomeText, instructionText, goalText, playText, gameModeText, nextText, selectedModeText;
sf::Text bestOf1Text, bestOf3Text, bestOf5Text, difficultyText, easyText, mediumText, hardText;
sf::Text difficultyButtonText, player1Text, player2Text;
sf::RectangleShape playButton, bestOf1Button, bestOf3Button, bestOf5Button, nextButton, easyButton,
mediumButton, hardButton, difficultyButton;
sf::RectangleShape scoreBoard[2]; // Player 1 and Player 2 score
sf::Text player1ScoreText, player2ScoreText;
sf::RectangleShape player1Score, player2Score;
sf::Text winnerText, rematchText, exitText, gameTrackText;
sf::RectangleShape rematchButton, exitButton;
sf::Clock delayClock;
bool waitingForDelay = false;

// Game state variables
bool gameOver = false, fullScreenMode = false;
char winner = ' ';  // 'O' or 'X', to store the winner

int selectedGameMode = 0;  // 0 -> None, 1 -> Best of 1, 2 -> Best of 3, 3 -> Best of 5
int selectedDifficulty = 0;  // 0 -> None, 1 -> Easy, 2 -> Medium, 3 -> Hard
bool inGameModeFrame = false;  // Tracks if we are in the game mode selection frame
bool inDifficultyFrame = false;  // Tracks if we are in the game mode selection frame
bool inGameFrame = false;

int gridSize = 0;  // 3 for Easy, 4 for Medium, 6 for Hard
int smallCenter, center;
int OScore = 0, XScore = 0, totalGames = 0;
int gameTrackScore = 0;

const unsigned int frameWidth = 800;
const unsigned int frameHeight = 660;

// 2D array to store the grid state
char grid[6][6];  // Maximum grid size is 6x6 (based on hardest difficulty)

// Player symbols
char currentPlayer = 'O'; // Start with Player O
int player = 0;
bool updateScoreAfterDelay = false;

bool isFullScreen() {
    int desktopHeight = 974;
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode(); // Get screen resolution
    sf::Vector2u windowSize = window.getSize(); // Get window size

    return (windowSize.x == desktopMode.width && windowSize.y == desktopHeight);
}

bool checkWinner(char player) {
    // Check rows
    for (int i = 0; i < gridSize; ++i) {
        bool win = true;
        for (int j = 0; j < gridSize; ++j) {
            if (grid[i][j] != player) {
                win = false;
                break;
            }
        }
        if (win) return true;
    }

    // Check columns
    for (int j = 0; j < gridSize; ++j) {
        bool win = true;
        for (int i = 0; i < gridSize; ++i) {
            if (grid[i][j] != player) {
                win = false;
                break;
            }
        }
        if (win) return true;
    }

    // Check diagonals
    bool win = true;
    for (int i = 0; i < gridSize; ++i) {
        if (grid[i][i] != player) {
            win = false;
            break;
        }
    }
    if (win) return true;

    win = true;
    for (int i = 0; i < gridSize; ++i) {
        if (grid[i][gridSize - 1 - i] != player) {
            win = false;
            break;
        }
    }
    return win;
}
// Function to check if the game is a draw
bool checkDraw() {
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            if (grid[row][col] == ' ') {  // If any cell is empty, it's not a draw
                return false;
            }
        }
    }
    return true;  // Grid is full
}

//provide hover effeect to next/play button 
void hoverEffect(sf::RectangleShape& button, sf::Text& text, sf::Color newButtonColor,
    sf::Color oldButtonColor = sf::Color::Black, sf::Color oldTextColor = sf::Color::White) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        button.setFillColor(newButtonColor);
        text.setFillColor(sf::Color::Black);
    }
    else {
        button.setFillColor(oldButtonColor);
        text.setFillColor(oldTextColor);
    }
}
// Function to update  the selected button
void updateButton(sf::RectangleShape& button, sf::Text& text, bool isSelected, sf::Color newButtonColor,
    sf::Color oldButtonColor = sf::Color::Black, sf::Color oldTextColor = sf::Color::White) {
    if (isSelected) {
        button.setFillColor(newButtonColor);
        text.setFillColor(sf::Color::Black);
    }
    else {
        button.setFillColor(oldButtonColor);
        text.setFillColor(sf::Color::White);
        hoverEffect(button, text, newButtonColor, oldButtonColor, oldTextColor);
    }
}

static void resizeElements(sf::RenderWindow& window, sf::Vector2u initialSize) {
    sf::Vector2u newSize = window.getSize();
    float scaleX = static_cast<float>(newSize.x) / initialSize.x;
    float scaleY = static_cast<float>(newSize.y) / initialSize.y;
    float textScale = std::min(scaleX, scaleY);

    // Helper function for resizing buttons and their text
    auto resizeButton = [&](sf::RectangleShape& button, sf::Text& text, sf::Vector2f buttonSize, sf::Vector2f buttonPos,
        float textXOffset, float textYOffset, unsigned int baseCharacterSize) {
            button.setSize(sf::Vector2f(buttonSize.x * scaleX, buttonSize.y * scaleY));
            button.setPosition(buttonPos.x * scaleX, buttonPos.y * scaleY);
            text.setPosition((buttonPos.x * scaleX + textXOffset * scaleX), buttonPos.y * scaleY + textYOffset * scaleY);
            text.setCharacterSize(static_cast<unsigned int>(baseCharacterSize * textScale));
        };

    // Resize buttons and texts
    resizeButton(playButton, playText, { 300.f, 50.f }, { 255.f, 485.f }, 115.f, 0.f, 45);
    resizeButton(nextButton, nextText, { 300.f, 50.f }, { 255.f, 485.f }, 110.f, 0.f, 40);
    resizeButton(bestOf1Button, bestOf1Text, { 300.f, 50.f }, { 250.f, 200.f }, isFullScreen() ? 101.f : 77.f, 4.f, 40);
    resizeButton(bestOf3Button, bestOf3Text, { 300.f, 50.f }, { 250.f, 275.f }, isFullScreen() ? 101.f : 77.f, 4.f, 40);
    resizeButton(bestOf5Button, bestOf5Text, { 300.f, 50.f }, { 250.f, 350.f }, isFullScreen() ? 101.f : 77.f, 4.f, 40);
    resizeButton(difficultyButton, difficultyButtonText, { 300.f, 50.f }, { 255.f, 485.f }, 115.f, 0.f, 40);
    resizeButton(easyButton, easyText, { 300.f, 50.f }, { 250.f, 200.f }, isFullScreen() ? 111.f : 86.f, 4.f, 35);
    resizeButton(mediumButton, mediumText, { 300.f, 50.f }, { 250.f, 275.f }, isFullScreen() ? 111.f : 86.f, 4.f, 35);
    resizeButton(hardButton, hardText, { 300.f, 50.f }, { 250.f, 350.f }, isFullScreen() ? 111.f : 86.f, 4.f, 35);

    // Resize other texts
    welcomeText.setPosition(230.f * scaleX - 50, 60.f * scaleY);
    welcomeText.setCharacterSize(static_cast<unsigned int>(45 * textScale));

    instructionText.setPosition(40.f * scaleX, 150.f * scaleY);
    instructionText.setCharacterSize(static_cast<unsigned int>(25 * textScale));

    gameModeText.setPosition((isFullScreen() ? 245.f : 201.f) * scaleX - 15, 60.f * scaleY);
    gameModeText.setCharacterSize(static_cast<unsigned int>(52 * textScale));

    difficultyText.setPosition((isFullScreen() ? 248.f : 206.f)* scaleX - 60, 60.f * scaleY);
    difficultyText.setCharacterSize(static_cast<unsigned int>(55 * textScale));

}

// Function to handle input events
void handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::Resized) {
            // Update the view to prevent distortion
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));

            // Call the resizeElements function
            resizeElements(window, sf::Vector2u(800, 660)); // Pass the initial size of the window
        }
        // Handle mouse clicks
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (!inGameModeFrame && !inDifficultyFrame && !inGameFrame) {
                // Check if Play button was clicked on main/instruction screen/frame
                if (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    inGameModeFrame = true;
                }
            }
            else if (gameOver) {
                if (rematchButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    if (winner == ' ') {
                        // Reset the game and start a new match
                        gameOver = false;
                        currentPlayer = (currentPlayer == 'O') ? 'X' : 'O';

                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                grid[i][j] = ' ';  // Empty grid cells
                            }
                        }
                        inGameFrame = true;
                    }
                    else {
                        // Reset the game and start a new match
                        gameOver = false;
                        currentPlayer = 'O';  // Reset the starting player
                        XScore = 0;
                        OScore = 0;
                        totalGames = 0;
                        gameTrackScore = 0;
                        winner = ' ';
                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                grid[i][j] = ' ';  // Empty grid cells
                            }
                        }
                        inGameFrame = true;
                    }
                }

                // Check if Exit button was clicked
                if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    window.close();  // Exit the game
                }
            }
            else if (inGameFrame) {
                // Check if a grid cell was clicked
                float topMargin = 120.f;
                float bottomMargin = window.getSize().y - 140;
                float gridWidth = window.getSize().x - 200;
                float gridHeight = bottomMargin - topMargin;
                float cellWidth = gridWidth / gridSize;
                float cellHeight = gridHeight / gridSize;
                moveSound.play();

                for (int row = 0; row < gridSize; ++row) {
                    for (int col = 0; col < gridSize; ++col) {
                        float cellLeft = 100 + col * cellWidth;
                        float cellTop = topMargin + row * cellHeight;

                        if (sf::Mouse::getPosition(window).x >= cellLeft && sf::Mouse::getPosition(window).x <= cellLeft + cellWidth &&
                            sf::Mouse::getPosition(window).y >= cellTop && sf::Mouse::getPosition(window).y <= cellTop + cellHeight) {

                            if (grid[row][col] == ' ') {  // Only mark the cell if it is empty
                                grid[row][col] = currentPlayer;  // Set current player's mark
                                currentPlayer = (currentPlayer == 'O') ? 'X' : 'O';  // Switch player
                                player = (currentPlayer == 'O') ? 1 : 0;

                                // Check if there's a winner
                                if (checkWinner('X')) {
                                    totalGames++;
                                    winSound.play();  // winning sound
                                    XScore++;
                                    if (selectedGameMode == 1) {
                                        winner = 'X';
                                        gameOver = true;
                                        waitingForDelay = true;
                                        delayClock.restart();
                                    }
                                    else if (selectedGameMode == 2) {
                                        if (totalGames >= 3 && XScore > OScore) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 3 && XScore == OScore) {
                                            winner = 'D';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (XScore >= 2) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }

                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                    else if (selectedGameMode == 3) {
                                        if (totalGames >= 5 && XScore > OScore) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 4 && ((XScore) > (OScore + 1))) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 5 && XScore == OScore) {
                                            winner = 'D';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (XScore >= 3) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                }
                                if (checkWinner('O')) {
                                    totalGames++;
                                    winSound.play();  // winning sound
                                    OScore++;
                                    if (selectedGameMode == 1) {
                                        winner = 'O';
                                        gameOver = true;
                                        waitingForDelay = true;  // Start waiting for the delay
                                        delayClock.restart();
                                    }
                                    else if (selectedGameMode == 2) {
                                        if (totalGames >= 3 && OScore > XScore) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 3 && OScore == XScore) {
                                            winner = 'D';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (OScore >= 2) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                    else if (selectedGameMode == 3) {
                                        if (totalGames >= 5 && OScore > XScore) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 4 && ((OScore) > (XScore + 1))) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 5 && XScore == OScore) {
                                            winner = 'D';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (OScore >= 3) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                }
                                if (checkDraw() && !checkWinner('X') && !checkWinner('O')) {
                                    totalGames++;
                                    drawSound.play();  // game draw sound
                                    if (selectedGameMode == 1) {
                                        gameOver = true;
                                        winner = 'D';
                                        waitingForDelay = true;
                                        delayClock.restart();
                                    }
                                    else if (selectedGameMode == 2) {
                                        if (totalGames >= 3 && XScore > OScore) {
                                            gameOver = true;
                                            winner = 'X';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 3 && XScore < OScore) {
                                            gameOver = true;
                                            winner = 'O';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 3 && XScore == OScore) {
                                            gameOver = true;
                                            winner = 'D';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                    else if (selectedGameMode == 3) {
                                        if (totalGames >= 5 && OScore > XScore) {
                                            gameOver = true;
                                            winner = 'O';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 5 && XScore > OScore) {
                                            gameOver = true;
                                            winner = 'X';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 5 && XScore == OScore) {
                                            gameOver = true;
                                            winner = 'D';
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 4 && ((OScore) > (XScore + 1))) {
                                            winner = 'O';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else if (totalGames >= 4 && ((XScore) > (OScore + 1))) {
                                            winner = 'X';
                                            gameOver = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                        else {
                                            winner = ' ';
                                            gameOver = true;
                                            updateScoreAfterDelay = true;
                                            waitingForDelay = true;
                                            delayClock.restart();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (inDifficultyFrame) {
                // Check if Easy, Medium, or Hard buttons are clicked
                if (easyButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedDifficulty = 1;
                    gridSize = 3;  // Easy(Level 1)  = 3x3 grid
                }
                else if (mediumButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedDifficulty = 2;
                    gridSize = 4;  // Medium(Level 2)  = 4x4 grid
                }
                else if (hardButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedDifficulty = 3;
                    gridSize = 6;  // Hard(Level 3)  = 6x6 grid
                }

                // Check if Difficulty button is clicked after difficulty selection
                if (difficultyButton.getGlobalBounds().contains(mousePos.x, mousePos.y) && selectedDifficulty != 0) {
                    buttonClickSound.play();
                    inGameFrame = true;
                    inDifficultyFrame = false;
                }
            }
            else if (inGameModeFrame) {
                // Check if Best of 1, 3, or 5 buttons are clicked
                if (bestOf1Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedGameMode = 1;
                }
                else if (bestOf3Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedGameMode = 2;
                }
                else if (bestOf5Button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    buttonClickSound.play();
                    selectedGameMode = 3;
                }

                // Check if Next button is clicked after game mode selection
                if (playButton.getGlobalBounds().contains(mousePos.x, mousePos.y) && selectedGameMode != 0) {
                    buttonClickSound.play();
                    inDifficultyFrame = true;
                }
            }
        }
    }
}

// Function to draw the Welcome screen
void drawWelcomeScreen() {
    window.clear(sf::Color::Black);

    // Draw Welcome and Instructions
    window.draw(welcomeText);
    window.draw(instructionText);
    window.draw(nextButton);
    window.draw(nextText);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    updateButton(nextButton, nextText,
        (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)),
        sf::Color(0, 158, 0), sf::Color::Green, sf::Color::Black);

    window.display();
}

// Function to draw the Game Mode selection screen
void drawGameModeScreen() {
    window.clear(sf::Color::Black);

    // Draw Game Mode Text
    window.draw(gameModeText);

    // Draw the buttons (Best of 1, 3, 5)
    window.draw(bestOf1Button);
    window.draw(bestOf3Button);
    window.draw(bestOf5Button);

    window.draw(bestOf1Text);
    window.draw(bestOf3Text);
    window.draw(bestOf5Text);

    // Draw the Next button (if game mode is selected)
    window.draw(playButton);
    window.draw(playText);

    // Update the button 
    updateButton(bestOf1Button, bestOf1Text, selectedGameMode == 1, sf::Color::Green);
    updateButton(bestOf3Button, bestOf3Text, selectedGameMode == 2, sf::Color::Yellow);
    updateButton(bestOf5Button, bestOf5Text, selectedGameMode == 3, sf::Color::Red);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    updateButton(playButton, playText, (playButton.getGlobalBounds().contains(mousePos.x, mousePos.y)),
        sf::Color(0, 158, 0), sf::Color::Green, sf::Color::Black);

    window.display();
}

// Function to draw the Difficulty selection screen
void drawDifficultyScreen() {
    window.clear(sf::Color::Black);

    // Draw Difficulty Text
    window.draw(difficultyText);

    // Draw the difficulty buttons (Easy, Medium, Hard)
    window.draw(easyButton);
    window.draw(mediumButton);
    window.draw(hardButton);

    window.draw(easyText);
    window.draw(mediumText);
    window.draw(hardText);

    // Draw the Next button (if difficulty is selected)
    window.draw(difficultyButton);
    window.draw(difficultyButtonText);

    // Update the button if selected
    updateButton(easyButton, easyText, selectedDifficulty == 1, sf::Color::Green);
    updateButton(mediumButton, mediumText, selectedDifficulty == 2, sf::Color::Yellow);
    updateButton(hardButton, hardText, selectedDifficulty == 3, sf::Color::Red);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    updateButton(difficultyButton, difficultyButtonText, (difficultyButton.getGlobalBounds().contains(mousePos.x, mousePos.y)),
        sf::Color(0, 158, 0), sf::Color::Green, sf::Color::Black);

    window.display();
}

// Function to draw the grid based on the selected difficulty
void drawGrid() {

    float topMargin = 120.f;   // Space at the top for player info
    float bottomMargin = window.getSize().y - 140;  // The bottom margin for the grid

    float gridWidth = window.getSize().x - 200;  // Subtracting 100 from each side
    float gridHeight = bottomMargin - topMargin;  // From y=120 to y=660

    float cellWidth = gridWidth / gridSize;
    float cellHeight = gridHeight / gridSize;

    vector<sf::RectangleShape> gridLines;
    for (int i = 1; i < gridSize; ++i) {
        sf::RectangleShape Horizontalline(sf::Vector2f(gridWidth, 2));
        sf::RectangleShape Verticalline(sf::Vector2f(2, gridHeight));
        Horizontalline.setFillColor(sf::Color::White);
        Horizontalline.setPosition(100, topMargin + i * cellHeight);
        Verticalline.setFillColor(sf::Color::White);
        Verticalline.setPosition(100 + i * cellWidth, topMargin);
        window.draw(Horizontalline);
        window.draw(Verticalline);
    }

    // Draw player marks (X or O) in the  with respect to gridsize
    sf::Text playerMarkText1;
    playerMarkText1.setFont(font);

    if (selectedDifficulty == 1) {
        playerMarkText1.setCharacterSize(window.getSize().x / static_cast<float>(10));
        smallCenter = 28;
        center = 59;
    }
    else if (selectedDifficulty == 2) {
        playerMarkText1.setCharacterSize(window.getSize().x / static_cast<float>(12));
        smallCenter = 22;
        center = 48;
    }
    else {
        playerMarkText1.setCharacterSize(window.getSize().x / static_cast<float>(16));
        smallCenter = 16;
        center = 35;
    }

    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            if (grid[row][col] != ' ') {  // Draw X or O only if the cell is filled

                playerMarkText1.setString(string(1, grid[row][col]));
                if (grid[row][col] == 'O') {
                    playerMarkText1.setFillColor(sf::Color::Green); // Green for O
                }
                else {
                    playerMarkText1.setFillColor(sf::Color::Red); // Red for X
                }
                playerMarkText1.setPosition
                (100 + col * cellWidth + (cellWidth / 2) - playerMarkText1.getLocalBounds().width / 2,
                    (topMargin - ((isFullScreen()) ? center : smallCenter)) + row * cellHeight + (cellHeight / 2) - playerMarkText1.getLocalBounds().height / 2);
                window.draw(playerMarkText1);
            }
        }
    }
}

// Function to draw the Player Info and Scores
void drawPlayerInfo() {

    player1Text.setFont(font);
    player1Text.setString("Player 1 \" O \"");
    player1Text.setCharacterSize(window.getSize().y / static_cast <float>(31));
    player1Text.setPosition(50, 16);  // Left side
    if (currentPlayer == 'O') {
        player1Text.setFillColor(sf::Color::Green);
        player1ScoreText.setFillColor(sf::Color::Green);
    }
    else {
        player1Text.setFillColor(sf::Color::White);
        player1ScoreText.setFillColor(sf::Color::White);
    }
    string o_score = to_string(OScore); // Convert integer to String

    player1ScoreText.setString(o_score);
    player1ScoreText.setFont(font);
    player1ScoreText.setCharacterSize(window.getSize().y / static_cast <float>(30));
    player1ScoreText.setPosition(110, 60);

    string game;

    if (selectedGameMode == 1) {
        game = "Game " + to_string(gameTrackScore + 1) + " of " + to_string(1);
    }
    else if (selectedGameMode == 2) {
        game = "Game " + to_string(gameTrackScore + 1) + " of " + to_string(3);
    }
    else if (selectedGameMode == 3) {
        game = "Game " + to_string(gameTrackScore + 1) + " of " + to_string(5);
    }

    // Game track
    gameTrackText.setFont(font);
    gameTrackText.setString(game);
    gameTrackText.setCharacterSize(window.getSize().y / static_cast<float>(22));
    gameTrackText.setFillColor(sf::Color::White);
    gameTrackText.setPosition
    (window.getSize().x / static_cast<float>(2) - gameTrackText.getLocalBounds().width / static_cast<float>(2), 36);  // Middle

    player2Text.setFont(font);
    player2Text.setString("Player 2 \" X \"");
    player2Text.setCharacterSize(window.getSize().y / static_cast <float>(31));
    player2Text.setPosition(window.getSize().x - 205, 16);  // Right side
    if (currentPlayer == 'X') {
        player2Text.setFillColor(sf::Color::Red);
        player2ScoreText.setFillColor(sf::Color::Red);
    }
    else {
        player2Text.setFillColor(sf::Color::White);
        player2ScoreText.setFillColor(sf::Color::White);
    }

    string x_score = to_string(XScore); // Convert integer to String

    player2ScoreText.setString(x_score);
    player2ScoreText.setFont(font);
    player2ScoreText.setCharacterSize(window.getSize().y / static_cast <float>(30));
    player2ScoreText.setPosition(window.getSize().x - 133, 60);

    window.draw(player1Text);
    window.draw(player1ScoreText);
    window.draw(gameTrackText);
    window.draw(player2Text);
    window.draw(player2ScoreText);
}
//goal of game 
void drawGoal() {
    string goal = "";
    goal = "Goal To Achieve ---> Get " + to_string(gridSize) + " of Your Consecutive Mark(X or O) ";
    goalText.setFont(font);
    goalText.setString(goal);
    goalText.setFillColor(sf::Color::White);
    goalText.setCharacterSize(window.getSize().x / 32);
    goalText.setPosition(10, window.getSize().y - 70);
    window.draw(goalText);
}
// Function to draw the Game screen with the Grid and Player Info
void drawGameScreen() {
    window.clear(sf::Color::Black);

    // Draw Player Info and Scores
    drawPlayerInfo();

    // Draw the grid
    drawGrid();
    drawGoal();
    window.display();
}

void drawGameOverScreen() {
    window.clear(sf::Color::Black);

    string score = to_string(OScore) + " - " + to_string(XScore);

    // Display Winner Text
    if (winner == 'X') {
        winnerText.setString("Player 2 Won!\n\t\t " + score);
    }
    else if (winner == 'O') {
        winnerText.setString("Player 1 Won!\n\t\t " + score);
    }
    else if (winner == ' ') {
        if (!checkDraw() && checkWinner('X'))
            winnerText.setString("Player 2 Won This Round!\n\t\t\t\t  " + score);
        else if (!checkDraw() && checkWinner('O'))
            winnerText.setString("Player 1 Won This Round!\n\t\t\t\t  " + score);
        else if (checkDraw())
            winnerText.setString("ROUND DRAW!\n\t\t" + score);
    }
    else {
        winnerText.setString("Game Draw!\n\t\t" + score);
    }
    winnerText.setFont(font);
    winnerText.setCharacterSize(window.getSize().x / 20);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setPosition(window.getSize().x / 2 - winnerText.getLocalBounds().width / 2, 100);
    window.draw(winnerText);

    // Draw Rematch Button
    rematchButton.setSize(sf::Vector2f(window.getSize().x / 3, window.getSize().y / 16 + 7));
    rematchButton.setFillColor(sf::Color::Green); // Default green
    rematchButton.setOutlineColor(sf::Color::White);
    rematchButton.setOutlineThickness(4);
    rematchButton.setPosition(
        window.getSize().x / 2 - rematchButton.getLocalBounds().width / 2, window.getSize().y / 2 - 60);

    rematchText.setFont(font);
    if (winner == ' ') {
        rematchText.setString("Next Round");
    }
    else {
        rematchText.setString("Play Again");
    }
    rematchText.setCharacterSize((isFullScreen() ? window.getSize().x / 30 : window.getSize().x / 25));
    rematchText.setFillColor(sf::Color::Black);
    rematchText.setPosition(
        window.getSize().x / 2 - rematchText.getLocalBounds().width / 2, window.getSize().y / 2 - (isFullScreen() ? 70 : 55));

    // Draw Exit Button
    exitButton.setSize(sf::Vector2f(window.getSize().x / 3, window.getSize().y / 16 + 7));
    exitButton.setFillColor(sf::Color::Red); // Default red
    exitButton.setOutlineColor(sf::Color::White);
    exitButton.setOutlineThickness(4);
    exitButton.setPosition(
        window.getSize().x / 2 - exitButton.getLocalBounds().width / 2, window.getSize().y / 2 + 30);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize((isFullScreen() ? window.getSize().x / 30 : window.getSize().x / 25));
    exitText.setFillColor(sf::Color::Black);
    exitText.setPosition(
        window.getSize().x / 2 - exitText.getLocalBounds().width / 2, window.getSize().y / 2 + (isFullScreen() ? 21 : 35));

    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Hover effects for Rematch Button (Background color only)
    if (rematchButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        rematchButton.setFillColor(sf::Color(0, 158, 0)); // Darker green for hover
    }
    else {
        rematchButton.setFillColor(sf::Color::Green); // Default green
    }

    // Hover effects for Exit Button (Background color only)
    if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        exitButton.setFillColor(sf::Color(220, 20, 60)); // Light coral for hover
    }
    else {
        exitButton.setFillColor(sf::Color::Red); // Default red
    }

    // Draw buttons and text
    window.draw(rematchButton);
    window.draw(rematchText);
    window.draw(exitButton);
    window.draw(exitText);

    window.display();
}


// Main game loop
void gameLoop() {
    while (window.isOpen()) {
        handleInput();
        if (waitingForDelay) {
            if (delayClock.getElapsedTime().asMilliseconds() >= 1500) {
                waitingForDelay = false;
                // Perform delayed score update
                if (updateScoreAfterDelay) {
                    gameTrackScore++;
                    updateScoreAfterDelay = false; // Reset flag
                }
            }
        }
        if (gameOver && !waitingForDelay) {
            drawGameOverScreen();
        }
        else if (inGameFrame) {
            drawGameScreen();
        }
        else if (inDifficultyFrame) {
            drawDifficultyScreen();
        }
        else if (inGameModeFrame) {
            drawGameModeScreen();
        }
        else {
            drawWelcomeScreen();
        }
    }
}

int main() {

    // Load default font (SFML default font)
    if (!font.loadFromFile("C:\\Users\\Muhammad Bilal\\Documents\\TicTacToeProject\\SFMLLinkTest\\SFMLLinkTest\\TicTacToe\\TicTacToe\\Poppins-Regular.ttf")) {
        cout << "Error loading font!" << endl;
        return -1;
    }
    if (!moveBuffer.loadFromFile("C:\\Users\\Muhammad Bilal\\Documents\\TicTacToeProject\\SFMLLinkTest\\SFMLLinkTest\\TicTacToe\\TicTacToe\\machine-tick.wav")) {
        std::cout << "Error loading Move sound!" << std::endl;
    }
    else {
        moveSound.setBuffer(moveBuffer);
    }

    if (!winBuffer.loadFromFile("C:\\Users\\Muhammad Bilal\\Documents\\TicTacToeProject\\SFMLLinkTest\\SFMLLinkTest\\TicTacToe\\TicTacToe\\winning-alert.wav")) {
        std::cout << "Error loading Win sound!" << std::endl;
    }
    else {
        winSound.setBuffer(winBuffer);
    }

    if (!drawBuffer.loadFromFile("C:\\Users\\Muhammad Bilal\\Documents\\TicTacToeProject\\SFMLLinkTest\\SFMLLinkTest\\TicTacToe\\TicTacToe\\draw-sound.wav")) {
        std::cout << "Error loading Draw sound!" << std::endl;
    }
    else {
        drawSound.setBuffer(drawBuffer);
    }

    if (!buttonClickBuffer.loadFromFile("C:\\Users\\Muhammad Bilal\\Documents\\TicTacToeProject\\SFMLLinkTest\\SFMLLinkTest\\TicTacToe\\TicTacToe\\mouse-click-sound.mp3")) {
        std::cout << "Error loading Button Click sound!" << std::endl;
    }
    else {
        buttonClickSound.setBuffer(buttonClickBuffer);
    }

    // Initialize the grid with empty values
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            grid[i][j] = ' ';  // Empty grid cells
        }
    }
    // Welcome Text
    welcomeText.setFont(font);
    welcomeText.setString("Welcome to Tic Tac Toe!");
    welcomeText.setCharacterSize(45);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setPosition(window.getSize().x / 2 - welcomeText.getLocalBounds().width / 2, 55);

    // Instructions Text
    instructionText.setFont(font);
    instructionText.setString("Instructions:\n - This is a Multiplayer game. Player 1 is O, Player 2 is X.\n - Take turns to place your marks.\n - You can play game in any of three modes :\n\tBest of 1, Best of 3 or Best of 5.\n - You can play game in any of three difficulties :\n\tEasy(Level 1), Medium(Level 2) or Hard(Level 3).\n - Grid Dimensions will vary in three different difficulty level.");
    instructionText.setCharacterSize(23);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(50, 165);

    //  Play Button on main/instruction screen
    nextButton.setSize(sf::Vector2f(300, 50));
    nextButton.setFillColor(sf::Color::Green);
    nextButton.setPosition(250, 500);
    nextButton.setOutlineThickness(5);
    nextButton.setOutlineColor(sf::Color::White);

    nextText.setFont(font);
    nextText.setString("Next");
    nextText.setCharacterSize(30);
    nextText.setFillColor(sf::Color::Black);
    nextText.setPosition(370, 505);

    // Game Mode Text
    gameModeText.setFont(font);
    gameModeText.setString("Select Game Mode");
    gameModeText.setCharacterSize(40);
    gameModeText.setFillColor(sf::Color::White);
    gameModeText.setPosition(210, 50);

    // Best of 1, 3, and 5 Buttons
    bestOf1Button.setSize(sf::Vector2f(300, 50));
    bestOf1Button.setFillColor(sf::Color::Black);
    bestOf1Button.setPosition(250, 150);
    bestOf1Button.setOutlineThickness(4);
    bestOf1Button.setOutlineColor(sf::Color::White);

    bestOf3Button.setSize(sf::Vector2f(300, 50));
    bestOf3Button.setFillColor(sf::Color::Black);
    bestOf3Button.setPosition(250, 220);
    bestOf3Button.setOutlineThickness(4);
    bestOf3Button.setOutlineColor(sf::Color::White);

    bestOf5Button.setSize(sf::Vector2f(300, 50));
    bestOf5Button.setFillColor(sf::Color::Black);
    bestOf5Button.setPosition(250, 290);
    bestOf5Button.setOutlineThickness(4);
    bestOf5Button.setOutlineColor(sf::Color::White);

    // Button Text for Best of 1, 3, and 5
    bestOf1Text.setFont(font);
    bestOf1Text.setString("Best of 1");
    bestOf1Text.setCharacterSize(25);
    bestOf1Text.setFillColor(sf::Color::White);
    bestOf1Text.setPosition(350, 160);

    bestOf3Text.setFont(font);
    bestOf3Text.setString("Best of 3");
    bestOf3Text.setCharacterSize(25);
    bestOf3Text.setFillColor(sf::Color::White);
    bestOf3Text.setPosition(350, 230);

    bestOf5Text.setFont(font);
    bestOf5Text.setString("Best of 5");
    bestOf5Text.setCharacterSize(25);
    bestOf5Text.setFillColor(sf::Color::White);
    bestOf5Text.setPosition(350, 300);

    //   next Button on MODE SELECTION screen
    playButton.setSize(sf::Vector2f(300, 50));
    playButton.setFillColor(sf::Color::Green);
    playButton.setOutlineColor(sf::Color::White);
    playButton.setOutlineThickness(5);
    playButton.setPosition(250, 385);

    playText.setFont(font);
    playText.setString("Next");
    playText.setCharacterSize(35);
    playText.setFillColor(sf::Color::Black);
    playText.setPosition(360, 390);

    // Difficulty Text
    difficultyText.setFont(font);
    difficultyText.setString("Select Difficulty Level");
    difficultyText.setCharacterSize(40);
    difficultyText.setFillColor(sf::Color::White);
    difficultyText.setPosition(190, 50);

    // Easy, Medium, and Hard Buttons
    easyButton.setSize(sf::Vector2f(300, 50));
    easyButton.setFillColor(sf::Color::Black);
    easyButton.setPosition(250, 150);
    easyButton.setOutlineThickness(4);
    easyButton.setOutlineColor(sf::Color::White);

    mediumButton.setSize(sf::Vector2f(300, 50));
    mediumButton.setFillColor(sf::Color::Black);
    mediumButton.setPosition(250, 220);
    mediumButton.setOutlineThickness(4);
    mediumButton.setOutlineColor(sf::Color::White);

    hardButton.setSize(sf::Vector2f(300, 50));
    hardButton.setFillColor(sf::Color::Black);
    hardButton.setPosition(250, 290);
    hardButton.setOutlineThickness(4);
    hardButton.setOutlineColor(sf::Color::White);

    // Button Text for Easy, Medium, and Hard
    easyText.setFont(font);
    easyText.setString("Level 1");
    easyText.setCharacterSize(25);
    easyText.setFillColor(sf::Color::White);
    easyText.setPosition(355, 160);

    mediumText.setFont(font);
    mediumText.setString("Level 2");
    mediumText.setCharacterSize(25);
    mediumText.setFillColor(sf::Color::White);
    mediumText.setPosition(355, 230);

    hardText.setFont(font);
    hardText.setString("Level 3");
    hardText.setCharacterSize(25);
    hardText.setFillColor(sf::Color::White);
    hardText.setPosition(355, 300);

    // Difficulty Button
    difficultyButton.setSize(sf::Vector2f(300, 50));
    difficultyButton.setFillColor(sf::Color::Green);  // Disabled until selected
    difficultyButton.setPosition(250, 400);
    difficultyButton.setOutlineThickness(5);
    difficultyButton.setOutlineColor(sf::Color::White);

    difficultyButtonText.setFont(font);
    difficultyButtonText.setString("Play");
    difficultyButtonText.setCharacterSize(35);
    difficultyButtonText.setFillColor(sf::Color::Black);
    difficultyButtonText.setPosition(370, 405);

    // Run the game loop
    gameLoop();

    return 0;
}