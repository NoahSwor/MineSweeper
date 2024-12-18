#include "Header1.h"

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
