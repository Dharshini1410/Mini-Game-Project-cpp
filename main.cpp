/*
 * ============================================================
 *   Mini Game Project — Tic Tac Toe & Snake Game
 *   C++ | Loops | Arrays | Conditional Logic
 *   Developed for Thiranex Internship - Task 4
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
  #define CLEAR "cls"
  #define SLEEP(ms) Sleep(ms)
  char getKey() { return _getch(); }
#else
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
  #define CLEAR "clear"
  #define SLEEP(ms) usleep((ms)*1000)

  // Non-blocking single-char read for Linux/Mac
  char getKey() {
      struct termios oldt, newt;
      char ch = 0;
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~(ICANON | ECHO);
      newt.c_cc[VMIN]  = 0;
      newt.c_cc[VTIME] = 0;
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      read(STDIN_FILENO, &ch, 1);
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return ch;
  }

  // Blocking single-char read (for menus)
  char getKeyBlocking() {
      struct termios oldt, newt;
      char ch = 0;
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~(ICANON | ECHO);
      newt.c_cc[VMIN]  = 1;
      newt.c_cc[VTIME] = 0;
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      read(STDIN_FILENO, &ch, 1);
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return ch;
  }
#endif

using namespace std;

// ═══════════════════════════════════════════════════════════
//
//   SECTION 1 — COMMON UTILITIES
//
// ═══════════════════════════════════════════════════════════

void clearScreen() { system(CLEAR); }

void printLine(char c = '=', int len = 60) {
    cout << string(len, c) << "\n";
}

void waitEnter(const string& msg = "  Press Enter to continue...") {
    cout << msg;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getInt(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= lo && v <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        cout << "  [!] Enter a number between " << lo << " and " << hi << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void animateText(const string& text, int delayMs = 30) {
    for (char c : text) {
        cout << c << flush;
        SLEEP(delayMs);
    }
    cout << "\n";
}

// ═══════════════════════════════════════════════════════════
//
//   SECTION 2 — TIC TAC TOE
//
// ═══════════════════════════════════════════════════════════

namespace TicTacToe {

// ── Score tracking ──
struct Scores {
    int p1 = 0, p2 = 0, draws = 0;
};

// ── Board: 3x3 array ──
char board[3][3];

void resetBoard() {
    int n = 1;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = '0' + n++;   // '1'..'9'
}

// ── Draw board ──
void drawBoard(const string& p1Name, const string& p2Name,
               const Scores& scores, int currentPlayer) {
    clearScreen();
    printLine('=');
    cout << "   TIC TAC TOE\n";
    printLine('=');
    cout << "  " << p1Name << " [X]: " << scores.p1
         << "   Draws: " << scores.draws
         << "   " << p2Name << " [O]: " << scores.p2 << "\n";
    printLine('-', 60);
    cout << "\n";

    for (int r = 0; r < 3; r++) {
        cout << "      ";
        for (int c = 0; c < 3; c++) {
            char cell = board[r][c];
            if (cell == 'X')      cout << " \033[1;34mX\033[0m ";
            else if (cell == 'O') cout << " \033[1;31mO\033[0m ";
            else                  cout << " \033[90m" << cell << "\033[0m ";
            if (c < 2) cout << "|";
        }
        cout << "\n";
        if (r < 2) cout << "      ---+---+---\n";
    }

    cout << "\n";
    printLine('-', 60);
    cout << "  Turn: " << (currentPlayer == 1
        ? "\033[1;34m" + p1Name + " (X)\033[0m"
        : "\033[1;31m" + p2Name + " (O)\033[0m") << "\n";
    printLine('-', 60);
}

// ── Place marker ──
bool placeMarker(int cell, char marker) {
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[r][c] == ('0' + cell)) {
                board[r][c] = marker;
                return true;
            }
    return false;
}

// ── Check winner ──
char checkWinner() {
    // Rows & cols
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
    }
    // Diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[0][2];
    return ' ';
}

// ── Check draw ──
bool isDraw() {
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[r][c] != 'X' && board[r][c] != 'O') return false;
    return true;
}

// ── AI: Easy (random) ──
int aiEasyMove() {
    vector<int> available;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[r][c] != 'X' && board[r][c] != 'O')
                available.push_back((r * 3) + c + 1);
    if (available.empty()) return -1;
    return available[rand() % available.size()];
}

// ── AI: Hard (minimax) ──
int minimax(bool isMaximizing) {
    char winner = checkWinner();
    if (winner == 'X') return -10;
    if (winner == 'O') return  10;
    if (isDraw())      return   0;

    if (isMaximizing) {
        int best = -100;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (board[r][c] != 'X' && board[r][c] != 'O') {
                    char tmp = board[r][c];
                    board[r][c] = 'O';
                    best = max(best, minimax(false));
                    board[r][c] = tmp;
                }
        return best;
    } else {
        int best = 100;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (board[r][c] != 'X' && board[r][c] != 'O') {
                    char tmp = board[r][c];
                    board[r][c] = 'X';
                    best = min(best, minimax(true));
                    board[r][c] = tmp;
                }
        return best;
    }
}

int aiHardMove() {
    int bestScore = -100, bestCell = -1;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[r][c] != 'X' && board[r][c] != 'O') {
                char tmp = board[r][c];
                int cell = r * 3 + c + 1;
                board[r][c] = 'O';
                int score = minimax(false);
                board[r][c] = tmp;
                if (score > bestScore) { bestScore = score; bestCell = cell; }
            }
    return bestCell;
}

// ── Single round ──
// returns: 1=P1 wins, 2=P2 wins, 0=draw
int playRound(const string& p1, const string& p2,
              Scores& scores, bool vsAI, int aiDiff) {
    resetBoard();
    int currentPlayer = 1;

    while (true) {
        drawBoard(p1, p2, scores, currentPlayer);

        int cell;
        char marker = (currentPlayer == 1) ? 'X' : 'O';

        if (currentPlayer == 2 && vsAI) {
            cout << "  AI is thinking...\n";
            SLEEP(600);
            cell = (aiDiff == 1) ? aiEasyMove() : aiHardMove();
            cout << "  AI chose cell " << cell << "\n";
            SLEEP(400);
        } else {
            while (true) {
                cell = getInt("  Enter cell (1-9): ", 1, 9);
                bool ok = false;
                for (int r = 0; r < 3; r++)
                    for (int c = 0; c < 3; c++)
                        if (board[r][c] == ('0' + cell)) ok = true;
                if (ok) break;
                cout << "  [!] Cell already taken. Try again.\n";
            }
        }

        placeMarker(cell, marker);

        char winner = checkWinner();
        if (winner != ' ') {
            drawBoard(p1, p2, scores, currentPlayer);
            if (winner == 'X') {
                scores.p1++;
                cout << "\n  \033[1;32m*** " << p1 << " (X) WINS! ***\033[0m\n\n";
                return 1;
            } else {
                scores.p2++;
                cout << "\n  \033[1;32m*** " << p2 << " (O) WINS! ***\033[0m\n\n";
                return 2;
            }
        }

        if (isDraw()) {
            drawBoard(p1, p2, scores, currentPlayer);
            scores.draws++;
            cout << "\n  \033[1;33m*** IT'S A DRAW! ***\033[0m\n\n";
            return 0;
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
}

// ── Main TTT entry ──
void run() {
    clearScreen();
    printLine('=');
    animateText("   TIC TAC TOE", 40);
    printLine('=');
    cout << "\n";

    cout << "  Game Mode:\n";
    cout << "  1. Player vs Player\n";
    cout << "  2. Player vs AI (Easy)\n";
    cout << "  3. Player vs AI (Hard)\n\n";
    int mode = getInt("  Choose mode: ", 1, 3);

    cout << "\n  Enter Player 1 name (X): ";
    string p1;
    getline(cin, p1);
    if (p1.empty()) p1 = "Player1";

    string p2;
    bool vsAI  = (mode > 1);
    int aiDiff = mode - 1;   // 1=easy, 2=hard

    if (vsAI) {
        p2 = (aiDiff == 1) ? "AI [Easy]" : "AI [Hard]";
    } else {
        cout << "  Enter Player 2 name (O): ";
        getline(cin, p2);
        if (p2.empty()) p2 = "Player2";
    }

    Scores scores;
    char replay = 'y';

    while (tolower(replay) == 'y') {
        playRound(p1, p2, scores, vsAI, aiDiff);

        printLine('-', 60);
        cout << "  SCOREBOARD: "
             << p1 << " " << scores.p1
             << "  |  Draws " << scores.draws
             << "  |  " << p2 << " " << scores.p2 << "\n";
        printLine('-', 60);
        cout << "  Play again? (y/n): ";
        cin >> replay;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    clearScreen();
    printLine('=');
    cout << "  FINAL SCORES\n";
    printLine('-', 60);
    cout << "  " << p1 << "  : " << scores.p1 << " win(s)\n";
    cout << "  " << p2 << "  : " << scores.p2 << " win(s)\n";
    cout << "  Draws      : " << scores.draws  << "\n";
    printLine('=');
    cout << "\n  Thanks for playing Tic Tac Toe!\n\n";
    waitEnter();
}

} // namespace TicTacToe

// ═══════════════════════════════════════════════════════════
//
//   SECTION 3 — SNAKE GAME
//
// ═══════════════════════════════════════════════════════════

namespace Snake {

// ── Board dimensions ──
const int WIDTH  = 30;
const int HEIGHT = 18;

// ── Direction enum ──
enum Direction { UP, DOWN, LEFT, RIGHT };

// ── Point struct ──
struct Point { int x, y; };

// ── Game state ──
struct GameState {
    vector<Point> snake;
    Point         food;
    Direction     dir;
    int           score;
    int           level;
    bool          gameOver;
};

// ── Place food at random empty cell ──
void placeFood(GameState& gs) {
    while (true) {
        int fx = rand() % (WIDTH  - 2) + 1;
        int fy = rand() % (HEIGHT - 2) + 1;
        bool onSnake = false;
        for (const auto& p : gs.snake)
            if (p.x == fx && p.y == fy) { onSnake = true; break; }
        if (!onSnake) { gs.food = {fx, fy}; return; }
    }
}

// ── Initialize game ──
GameState init(int level) {
    GameState gs;
    gs.score    = 0;
    gs.level    = level;
    gs.gameOver = false;
    gs.dir      = RIGHT;

    // Snake starts in middle, 3 segments
    int cx = WIDTH / 2, cy = HEIGHT / 2;
    gs.snake.push_back({cx,   cy});
    gs.snake.push_back({cx-1, cy});
    gs.snake.push_back({cx-2, cy});

    placeFood(gs);
    return gs;
}

// ── Draw frame ──
void draw(const GameState& gs, int highScore) {
    // Build 2D char grid
    char grid[HEIGHT][WIDTH];
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            grid[y][x] = ' ';

    // Borders
    for (int x = 0; x < WIDTH;  x++) { grid[0][x] = '#'; grid[HEIGHT-1][x] = '#'; }
    for (int y = 0; y < HEIGHT; y++) { grid[y][0] = '#'; grid[y][WIDTH-1]   = '#'; }

    // Food
    grid[gs.food.y][gs.food.x] = '$';

    // Snake body
    for (int i = 1; i < (int)gs.snake.size(); i++)
        grid[gs.snake[i].y][gs.snake[i].x] = 'o';

    // Snake head
    grid[gs.snake[0].y][gs.snake[0].x] = '@';

    // Print
    clearScreen();
    printLine('=');
    cout << "  SNAKE GAME   Score: " << setw(4) << gs.score
         << "   Level: " << gs.level
         << "   High Score: " << highScore << "\n";
    printLine('=');
    for (int y = 0; y < HEIGHT; y++) {
        cout << "  ";
        for (int x = 0; x < WIDTH; x++) {
            char c = grid[y][x];
            if (c == '#')      cout << "\033[90m#\033[0m";
            else if (c == '@') cout << "\033[1;32m@\033[0m";
            else if (c == 'o') cout << "\033[32mo\033[0m";
            else if (c == '$') cout << "\033[1;33m$\033[0m";
            else               cout << ' ';
        }
        cout << "\n";
    }
    printLine('-', 60);
    cout << "  Controls: W=Up  S=Down  A=Left  D=Right  Q=Quit\n";
    printLine('-', 60);
}

// ── Move snake ──
void move(GameState& gs) {
    Point head = gs.snake[0];

    switch (gs.dir) {
        case UP:    head.y--; break;
        case DOWN:  head.y++; break;
        case LEFT:  head.x--; break;
        case RIGHT: head.x++; break;
    }

    // Wall collision
    if (head.x <= 0 || head.x >= WIDTH-1 || head.y <= 0 || head.y >= HEIGHT-1) {
        gs.gameOver = true; return;
    }

    // Self collision
    for (const auto& p : gs.snake)
        if (p.x == head.x && p.y == head.y) { gs.gameOver = true; return; }

    gs.snake.insert(gs.snake.begin(), head);

    // Eat food
    if (head.x == gs.food.x && head.y == gs.food.y) {
        gs.score += 10 * gs.level;
        placeFood(gs);
    } else {
        gs.snake.pop_back();
    }
}

// ── Speed based on level ──
int getDelay(int level) {
    // ms per frame
    switch (level) {
        case 1: return 160;
        case 2: return 110;
        case 3: return  70;
        default: return 160;
    }
}

// ── Main Snake loop ──
void run() {
    clearScreen();
    printLine('=');
    animateText("   SNAKE GAME", 40);
    printLine('=');
    cout << "\n";
    cout << "  Select Level:\n";
    cout << "  1. Easy   (slow)\n";
    cout << "  2. Medium (normal)\n";
    cout << "  3. Hard   (fast)\n\n";
    int level = getInt("  Level: ", 1, 3);

    int highScore = 0;
    char replay   = 'y';

    while (tolower(replay) == 'y') {
        GameState gs = init(level);
        int delay    = getDelay(level);

        while (!gs.gameOver) {
            draw(gs, highScore);

            // Non-blocking key poll over the delay window
            auto start = chrono::steady_clock::now();
            while (true) {
                auto now     = chrono::steady_clock::now();
                auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start).count();
                if (elapsed >= delay) break;

                char key = getKey();
                if (key != 0) {
                    char k = tolower(key);
                    if      (k == 'w' && gs.dir != DOWN)  gs.dir = UP;
                    else if (k == 's' && gs.dir != UP)    gs.dir = DOWN;
                    else if (k == 'a' && gs.dir != RIGHT) gs.dir = LEFT;
                    else if (k == 'd' && gs.dir != LEFT)  gs.dir = RIGHT;
                    else if (k == 'q')                    gs.gameOver = true;
                }
                SLEEP(10);
            }

            if (!gs.gameOver) move(gs);

            // Level up every 50 pts
            if (gs.score > 0 && gs.score % 50 == 0 && level < 3) {
                level++;
                delay = getDelay(level);
            }
        }

        if (gs.score > highScore) highScore = gs.score;

        draw(gs, highScore);
        cout << "\n";
        if (gs.score == highScore && gs.score > 0)
            cout << "  \033[1;33m*** NEW HIGH SCORE: " << highScore << "! ***\033[0m\n\n";
        else
            cout << "  \033[1;31m GAME OVER! \033[0m  Final Score: " << gs.score << "\n\n";

        cout << "  Play again? (y/n): ";
        cin >> replay;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        level = getInt("  Level (1-3): ", 1, 3);
        delay = getDelay(level);
    }

    clearScreen();
    printLine('=');
    cout << "  SNAKE — SESSION STATS\n";
    printLine('-', 60);
    cout << "  High Score: " << highScore << "\n";
    printLine('=');
    cout << "\n  Thanks for playing Snake!\n\n";
    waitEnter();
}

} // namespace Snake

// ═══════════════════════════════════════════════════════════
//
//   SECTION 4 — MAIN LAUNCHER
//
// ═══════════════════════════════════════════════════════════

void showLauncher() {
    clearScreen();
    printLine('=');
    cout << "\n";
    animateText("     MINI GAME PROJECT", 35);
    animateText("     Thiranex Internship - Task 4", 20);
    cout << "\n";
    printLine('=');
    cout << "\n";
    cout << "   Select a Game:\n\n";
    cout << "   1.  Tic Tac Toe\n";
    cout << "       2-player or vs AI (Easy / Hard / Minimax)\n\n";
    cout << "   2.  Snake Game\n";
    cout << "       Classic snake — 3 difficulty levels\n\n";
    cout << "   0.  Exit\n\n";
    printLine('-', 60);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    int choice;
    do {
        showLauncher();
        choice = getInt("   Your choice: ", 0, 2);

        switch (choice) {
            case 1:
                TicTacToe::run();
                break;
            case 2:
                Snake::run();
                break;
            case 0:
                clearScreen();
                printLine('=');
                animateText("  Thanks for playing! — Thiranex Internship", 25);
                printLine('=');
                cout << "\n";
                break;
            default:
                break;
        }
    } while (choice != 0);

    return 0;
}