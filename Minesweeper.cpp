#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>

// ok so I have submitted this project twice. The first iteration did the exact same thing as the second, however I asked TA thomas to check if everything runs well before the day was over
// he was having issues I was not having, and wasn't able to get the headers and cpp files to work together. The project was 100% working on my machine, however neither of us could figure
// out why it wasn't working on his end. Eventually, he was able to get the code to run on his end by putting all of the code into one cpp file, which is what I have here.
// originally I had additional headers and cpp files, but just for minimizing the risk of the code not working on a grader's machine for some unknown reason, I have combined them for ease into one file
// this is also why there may be randon std::couts or whatever despite me having namespace std at the start. It should all still work though :)
// thank you!

using namespace std;

//original cell.h

struct CellState {
    // we want a bunch of bools that will track what state each cell is in (this also helps so we can have MULTIPLE cell states at the same time (ie revealed and number)
    bool unrevealed = true;
    bool revealed = false;
    bool mine = false;
    bool flag = false;
    bool number = false;
};

class Cell {
private:
    sf::Sprite sprite;
    static sf::Texture* hiddenTexture;
    static sf::Texture* revealedTexture;
    static sf::Texture* flagTexture;
    static sf::Texture* numberTextures[8]; // for number 1-8
    static sf::Texture* mineTexture;

    CellState state; // will track the state of the cell 
    int surroundingMines; // how many mines are adjacent to this cell?


public:
    // Constructor
    Cell(float x, float y, float tileSize);

    static int revealedCells; // static because this will be true of all cells

    void draw(sf::RenderWindow& window);

    void reveal();

    void revealNeighbors(vector<vector<Cell>>& board, int x, int y, int width, int height);

    void placeFlag();

    void removeFlag();

    void setSurroundingMines(int count);

    void setState(const CellState& newState);

    static void cleanUp();

    static void loadTextures();

    const CellState& getState() const;

    int getSurroundingMines() const;

    bool isMine() const;
};

sf::Texture* Cell::hiddenTexture = nullptr;
sf::Texture* Cell::revealedTexture = nullptr;
sf::Texture* Cell::flagTexture = nullptr;
sf::Texture* Cell::numberTextures[8] = { nullptr };
sf::Texture* Cell::mineTexture = nullptr;


// original cell.cpp

Cell::Cell(float x, float y, float tileSize)
    : state{ true, false, false, false, false }, surroundingMines(0) {

    if (hiddenTexture == nullptr) {
        loadTextures();
    }

    sprite.setTexture(*hiddenTexture);
    sprite.setPosition(x, y);
    sprite.setScale(tileSize / sprite.getLocalBounds().width,
        tileSize / sprite.getLocalBounds().height);
}


void Cell::draw(sf::RenderWindow& window) {
    // note: Elizabeth asked on slack why the flags were dark, and thomas said we need to draw the flags/mines on TOP of the revealed/unrevealed tiles. My code was doing the same thing, so this is updated
    if (state.revealed) {
        sprite.setTexture(*revealedTexture);
        window.draw(sprite);

        // draw additional textures on top of the revealed texture
        if (state.mine) {
            sprite.setTexture(*mineTexture);
            window.draw(sprite);
        }
        else if (surroundingMines > 0) {
            sprite.setTexture(*numberTextures[surroundingMines - 1]);
            window.draw(sprite);
        }
    }
    else {
        if (state.flag) {
            sprite.setTexture(*hiddenTexture);
            window.draw(sprite);

            // put flag ON TOP of it (thanks elizabeth)
            sprite.setTexture(*flagTexture);
            window.draw(sprite);
        }
        else {
            sprite.setTexture(*hiddenTexture);
        }
        window.draw(sprite);
    }
}

// when the player clicks, run reveal. Hovering will set state, but only clicking will reveal
void Cell::reveal() {
    if (state.unrevealed && !state.flag) {
        state.unrevealed = false;
        state.revealed = true;

        if (state.mine) {
            sprite.setTexture(*mineTexture);
        }
        else if (surroundingMines > 0) {
            // this is happening
            sprite.setTexture(*numberTextures[surroundingMines - 1]);
            revealedCells++;
        }
        else {
            sprite.setTexture(*revealedTexture);
            revealedCells++;
        }
    }
}

void Cell::revealNeighbors(vector<vector<Cell>>& board, int x, int y, int width, int height) {
    if (state.revealed || state.flag) return;

    // run reveal
    reveal();

    if (surroundingMines == 0 && !state.mine) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int k = x + i;
                int l = y + j;
                if (k >= 0 && k < width && l >= 0 && l < height) {
                    board[l][k].revealNeighbors(board, k, l, width, height);
                }
            }
        }
    }
}

// MAKE SURE FLAGS CAN NOT BE PLACED ON REVEALED TILES!!!
void Cell::placeFlag() {
    if (state.unrevealed) {
        state.flag = true;
        // tile can still be "unrevealed" with a flag 
        sprite.setTexture(*flagTexture);
    }
}

// need to fix later
void Cell::removeFlag() {
    if (state.flag) {
        sprite.setTexture(*hiddenTexture);
        state.flag = false;
        state.unrevealed;
    }
}


void Cell::setState(const CellState& newState) {
    state = newState;
}

const CellState& Cell::getState() const {
    return state;
}

void Cell::setSurroundingMines(int count) {
    surroundingMines = count;
}

void Cell::loadTextures() {
    hiddenTexture = new sf::Texture();
    revealedTexture = new sf::Texture();
    flagTexture = new sf::Texture();
    mineTexture = new sf::Texture();

    if (!hiddenTexture->loadFromFile("images/tile_hidden.png") ||
        !revealedTexture->loadFromFile("images/tile_revealed.png") ||
        !flagTexture->loadFromFile("images/flag.png") ||
        !mineTexture->loadFromFile("images/mine.png")) {
        cout << "something other than the numbers failed to load" << endl;
    }

    // this is just a shorter way of loading them shown to me, so I don't have several lines of the same code just loading in number1, number2, number 3 etx
    for (int i = 0; i < 8; ++i) {
        numberTextures[i] = new sf::Texture();
        if (!numberTextures[i]->loadFromFile("images/number_" + to_string(i + 1) + ".png")) {
            cout << "the numbers failed to load" << endl;
        }
    }
}


void drawNumber(sf::RenderWindow& window, sf::Texture& digitsTexture, int number, float x, float y) {

    number = std::max(-999, std::min(999, number));


    std::string numberStr = std::to_string(std::abs(number));

    // ensure 3 digits (I did this before reading on slack that it doesn't matter -_-
    numberStr = std::string(3 - numberStr.length(), '0') + numberStr;

    // - is 11th option
    if (number < 0) {
        numberStr = "-" + numberStr;
    }

    // draw each digit
    float offsetX = 0;
    for (char digit : numberStr) {
        sf::Sprite digitSprite;
        digitSprite.setTexture(digitsTexture);

        int digitIndex = (digit == '-') ? 10 : (digit - '0');
        digitSprite.setTextureRect(sf::IntRect(digitIndex * 21, 0, 21, 32));
        digitSprite.setPosition(x + offsetX, y);

        window.draw(digitSprite);

        offsetX += 21;
    }
}

void Cell::cleanUp() {
    delete hiddenTexture;
    delete revealedTexture;
    delete flagTexture;
    delete mineTexture;
    for (int i = 0; i < 8; ++i) {
        delete numberTextures[i];
    }
}

bool Cell::isMine() const {
    return state.mine;
}

int Cell::revealedCells = 0;

int Cell::getSurroundingMines() const {
    return surroundingMines;
}







// original main.cpp


void AssortBoard(vector<vector<Cell>>& board, int mineCount, int width, int height);

void loadBoardFromFile(const string& filename, vector<vector<Cell>>& board, float tileSize, int& mineCount, int& totalSafeCells);

int main() {
    // opening the config file, will have to modify later possibly with the test games
    ifstream config("boards/config.cfg");
    int width, height, mineCount;

    if (config.is_open()) {
        config >> width >> height >> mineCount;
        config.close();
    }

    else {
        cout << "config file didn't open L" << endl;
    }

    const int tileSize = 32;
    int windowWidth = width * tileSize;
    // pdf asked for height + 100 for the status bar and smileys
    int windowHeight = height * tileSize + 100;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Minesweeper");

    // creating the smiley and buttons
    sf::Texture happyFaceTexture;
    if (!happyFaceTexture.loadFromFile("images/face_happy.png")) {
        cout << "Failed to load face_happy" << endl;
    }

    sf::Texture sadFaceTexture;
    if (!sadFaceTexture.loadFromFile("images/face_lose.png")) {
        cout << "Failed to load face_lose" << endl;
    }

    sf::Texture coolFaceTexture;
    if (!coolFaceTexture.loadFromFile("images/face_win.png")) {
        cout << "Failed to load face_win" << endl;
    }

    sf::Texture debugTexture;
    if (!debugTexture.loadFromFile("images/debug.png")) {
        cout << "Failed to load debug" << endl;
    }

    sf::Texture test1Texture;
    if (!test1Texture.loadFromFile("images/test_1.png")) {
        cout << "Failed to load test1" << endl;
    }

    sf::Texture test2Texture;
    if (!test2Texture.loadFromFile("images/test_2.png")) {
        cout << "Failed to load test2" << endl;
    }

    sf::Texture test3Texture;
    if (!test3Texture.loadFromFile("images/test_3.png")) {
        cout << "Failed to load test3" << endl;
    }

    sf::Texture digitsTexture;
    if (!digitsTexture.loadFromFile("images/digits.png")) {
        cout << "Failed to load digits.png" << endl;
    }

    sf::Sprite happyFaceSprite(happyFaceTexture);
    // thomas said on slack that the positions dont have to be exact, so im just putting it in the middle (divided by 2)
    happyFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite sadFaceSprite(sadFaceTexture);
    sadFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite coolFaceSprite(coolFaceTexture);
    coolFaceSprite.setPosition((windowWidth - 64) / 2, height * tileSize + (100 - 64) / 2);

    sf::Sprite debugSprite(debugTexture);
    // behold, the part where I try different x and y coordinates 42159 times until it looks nice
    debugSprite.setPosition((windowWidth - 320), height * tileSize + (100 - 64) / 2);

    sf::Sprite test1Sprite(test1Texture);
    test1Sprite.setPosition((windowWidth - 250), height * tileSize + (100 - 64) / 2);

    sf::Sprite test2Sprite(test2Texture);
    test2Sprite.setPosition((windowWidth - 180), height * tileSize + (100 - 64) / 2);

    sf::Sprite test3Sprite(test3Texture);
    test3Sprite.setPosition((windowWidth - 110), height * tileSize + (100 - 64) / 2);

    vector<vector<Cell>> board;
    for (int y = 0; y < height; ++y) {
        vector<Cell> row;
        for (int x = 0; x < width; ++x) {
            row.emplace_back(x * tileSize, y * tileSize, tileSize);
        }
        board.push_back(row);
    }

    // assort the board

    AssortBoard(board, mineCount, width, height);

    bool gameWin = false;
    bool gameEnd = false;
    bool gameLose = false;
    static bool debugEnabled = false;

    int totalSafeCells = (width * height) - mineCount;


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int cursorX = static_cast<int>(mousePos.x / tileSize);
            int cursorY = static_cast<int>(mousePos.y / tileSize);

            // same click inputs as before, just check if the game is running. The only clicks that will register are the menus so they can be out the if statement



            if (!gameEnd) {


                // trying to check if the stuff is properly being laid out
                if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                    const CellState& cellState = board[cursorY][cursorX].getState();
                    // cout << "Hovering over (" << cursorX << ", " << cursorY << ")" << endl;
                    // if (cellState.unrevealed) cout << "Unrevealed ";
                    // if (cellState.revealed) cout << "Revealed ";
                    // if (cellState.mine) cout << "Mine ";
                    // if (cellState.flag) cout << "Flag ";
                    // if (cellState.number) cout << "Number ";
                    // these have been SO USEFUL BECAUSE THE MINES KEEP NOT WORKING EVEN THOUGH I AM CLICKING ON THEM AGGHHH
                }

                // click time
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                            // we can just always call reveal neighbors, since it has reveal within it
                            board[cursorY][cursorX].revealNeighbors(board, cursorX, cursorY, width, height);
                            const CellState& cellState = board[cursorY][cursorX].getState();
                            if (cellState.mine) {
                                gameLose = true;
                                gameEnd = true;
                                // need to get state of every tile, then call reveal if it is a mine
                                for (int y = 0; y < height; y++) {
                                    for (int x = 0; x < width; x++) {
                                        const CellState& currentState = board[y][x].getState();
                                        if (currentState.mine) {
                                            board[y][x].reveal();
                                        }
                                    }
                                }
                            }

                        }
                    }

                    // these F'ing flags

                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if (cursorX >= 0 && cursorX < width && cursorY >= 0 && cursorY < height) {
                            const CellState& cellState = board[cursorY][cursorX].getState();
                            if (cellState.unrevealed) {
                                if (!cellState.flag) {
                                    board[cursorY][cursorX].placeFlag();
                                    mineCount = mineCount - 1;
                                }
                                else {
                                    board[cursorY][cursorX].removeFlag();
                                    mineCount++;

                                }
                            }

                        }
                    }
                }

                // very similar functionality as smiley for the debug button FIX SAME PROBLEM AS FLAGS!!! ALSO ITS SAYING DEBUG CLICKED EVERYWHERE???



                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::FloatRect debugBounds = debugSprite.getGlobalBounds();

                    if (debugBounds.contains(mousePos)) {
                        debugEnabled = !debugEnabled;

                       //  cout << "debug mode enabled" << endl;

                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                const CellState& currentState = board[y][x].getState();
                                if (currentState.mine) {
                                    if (debugEnabled) {
                                        board[y][x].reveal();
                                    }
                                    else {
                                        if (currentState.flag) {
                                            board[y][x].setState({ true, false, true, true, false });
                                        }
                                        else {
                                            board[y][x].setState({ true, false, true, false, false });
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }

            // this checks if the smiley was clicked. This can be outside the if statement above, since it can be clicked at any time during the game
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect happyFaceBounds = happyFaceSprite.getGlobalBounds();

                if (happyFaceBounds.contains(mousePos)) {
                    // I was going to make separate if statements for each state of the smiley but I think since winning/losing just overlays it I can just have one

                    // reset board and defaults
                    // I had an issue where assorting the board would not change mines back to unrevealed, since the assort function
                    // doesn't expect there to be mines prior to assorting. This for loop just ensures all the cells don't have mines before assorting them
                    for (int x = 0; x < width; x++) {
                        for (int y = 0; y < height; y++) {
                            board[y][x].setSurroundingMines(0);
                            board[y][x].setState({ true, false, false, false, false });
                        }
                    }


                    Cell::revealedCells = 0;
                    // NEED FIX!!!!!!!!!!!!!! CLICKING THE BUTTON DOES NOT RESET THE DRAW NUMBER COUNT ????
                    ifstream config("boards/config.cfg");
                    if (config.is_open()) {
                        config >> width >> height >> mineCount;
                        config.close();
                    }
                    AssortBoard(board, mineCount, width, height);
                    // fixed :D
                    gameLose = false;
                    gameEnd = false;
                    gameWin = false;
                    debugEnabled = false;
                    totalSafeCells = (width * height) - mineCount;

                }
            }


            // add the test buttons here (outside the if game)

            // bug here, mine count is correct but the win condition becomes a loss?

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test1Bounds = test1Sprite.getGlobalBounds();

                if (test1Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;
                    loadBoardFromFile("boards/testboard1.brd", board, tileSize, mineCount, totalSafeCells);


                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test2Bounds = test2Sprite.getGlobalBounds();

                if (test2Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;
                    loadBoardFromFile("boards/testboard2.brd", board, tileSize, mineCount, totalSafeCells);


                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::FloatRect test3Bounds = test3Sprite.getGlobalBounds();

                if (test3Bounds.contains(mousePos)) {
                    Cell::revealedCells = 0;

                    gameEnd = false;
                    gameWin = false;
                    gameLose = false;
                    debugEnabled = false;

                    loadBoardFromFile("boards/testboard3.brd", board, tileSize, mineCount, totalSafeCells);

                }
            }




            if (totalSafeCells == Cell::revealedCells) {
                gameWin = true;
                gameEnd = true;
                // place a flag on unrevealed tiles once won
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        const CellState& currentState = board[y][x].getState();
                        if (currentState.mine) {
                            // placeflag only works with unrevealed cells, so this just sets all of them to not have mines (which should happen if you won anyway)
                            // this also helps with making the debug button not do anything after winning
                            board[y][x].setState({ true, false, false, false, false });
                            board[y][x].placeFlag();
                        }
                    }
                }
            }
        }


        window.clear();

        for (auto& row : board) {
            for (auto& cell : row) {
                cell.draw(window);
            }
        }

        window.draw(test1Sprite);
        window.draw(test2Sprite);
        window.draw(test3Sprite);
        if (!gameEnd) {
            window.draw(happyFaceSprite);
        }

        else if (gameEnd && gameLose) {
            window.draw(sadFaceSprite);
        }
        else if (gameEnd && gameWin) {
            window.draw(coolFaceSprite);
        }
        window.draw(debugSprite);
        drawNumber(window, digitsTexture, mineCount, 30, height * tileSize + 10);

        window.display();
    }

    Cell::cleanUp();
    return 0;
}

void AssortBoard(vector<vector<Cell>>& board, int mineCount, int width, int height) {

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < mineCount; ++i) {
        int randX = rand() % width;
        int randY = rand() % height;
        if (!board[randY][randX].isMine()) {
            // THIS ONE LINE CAUSED ME HOURS UPON HOURS OF STRIFE (i accidentally set first to false)
            board[randY][randX].setState({ true, false, true, false, false });
        }
        else {
            --i;
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (board[y][x].isMine()) continue;

            int mineCount = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int k = x + j;
                    int l = y + i;
                    if (k >= 0 && k < width && l >= 0 && l < height) {
                        if (board[l][k].isMine()) {
                            ++mineCount;
                        }
                    }
                }
            }
            if (mineCount > 0) {
                board[y][x].setSurroundingMines(mineCount);
            }
        }
    }
}


void loadBoardFromFile(const string& filename, vector<vector<Cell>>& board, float tileSize, int& mineCount, int& totalSafeCells) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }

    // professor fox told me in OH that the test boards will just assume 25x16
    const int rows = 16;
    const int cols = 25;

    board.clear();
    board.resize(rows, vector<Cell>(cols, Cell(0, 0, tileSize)));

    mineCount = 0;
    totalSafeCells = 400;

    string line;
    int row = 0;

    while (getline(file, line) && row < rows) {
        for (int col = 0; col < cols && col < line.length(); ++col) {
            float x = col * tileSize;
            float y = row * tileSize;

            Cell& cell = board[row][col];
            cell = Cell(x, y, tileSize);

            // set state based on the 1s and 0s
            if (line[col] == '1') {
                CellState state;
                state.unrevealed = true;
                state.mine = true;
                cell.setState(state);
                mineCount++;
                totalSafeCells--;
            }
            else if (line[col] == '0') {
                CellState state;
                state.unrevealed = true;
                state.mine = false;
                cell.setState(state);
            }
        }
        ++row;
    }

    file.close();

    // after loading the board, calculate surrounding mines for each cell
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int surroundingMines = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !(dr == 0 && dc == 0)) {
                        if (board[nr][nc].isMine()) {
                            ++surroundingMines;
                        }
                    }
                }
            }
            board[r][c].setSurroundingMines(surroundingMines);
        }
    }
}
