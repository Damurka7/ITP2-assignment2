// Lecture 13 - Type casting
#include <iostream>
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;
using namespace chrono_literals;

const int TIMEOUT = 400; // maximum number of milliseconds that a player is allowed to take

//ITEM 3.d: UNIQUE FEATURE (I really hope that no one has done this), system of penalty - in my game it may happens that player did stupid step(didn't change anything), therefore this player deserve a time-penalty
const int PENALTY = 5; //penalty time if player hasn't done his action

class Symbols;

vector<vector<Symbols>> map; //our map
bool gameOver = false;

//ITEM 3.b: Smart Pointer container, but instead of using std smart pointers, I have created my own, because I need to change all the features of standard pointers - it is so ineffective
//for example: deleting element do not require its real deletion, I only mark its position as null-Symbol
template<class T>
class SmartPointer {
public:
    //simple constructor
    SmartPointer() {}

    //again constructor
    SmartPointer(T &obj) : obj(obj) {}

    //remove constructor, it doesn't delete the symbol, but marks it as null-symbol
    ~SmartPointer() {
        obj->isNull = true; //not deleting the element, but making it null
    }

    //operators overloading
    T *operator->() {
        return obj;
    }

    T operator*() {
        return *obj;
    }

    T &operator=(T n) {
        this->obj = &n;
        return *this->obj;
    }

private:
    T *obj;
};

class Position {
public:
    tuple<int, int> pos;

    bool operator==(const Position &p) {
        int oldr = get<0>(this->pos);
        int oldc = get<1>(this->pos);
        int pr = get<0>(p.pos);
        int pc = get<1>(p.pos);
        return (oldr == pr) && (oldc == pc);
    }

    bool operator!=(const Position &p) {
        int oldr = get<0>(this->pos);
        int oldc = get<1>(this->pos);
        int pr = get<0>(p.pos);
        int pc = get<1>(p.pos);
        return (oldr != pr) || (oldc != pc);
    }

};

//ITEM 1.a: movable symbols; Capital and Small;
class Symbols : public Position {
public:
    Symbols() : isNull(true), letter('-') {}

    Symbols(int row, int col) : isNull(true), letter('-') {
        tuple<int, int> p(row, col);
        this->pos = p;
    }

    char getLetter() const {
        return letter;
    }

    bool operator==(const Symbols &s) const {
        return this->symb == s.symb;
    }

    bool operator!=(const Symbols &s) const {
        return this->symb != s.symb;
    }

//TODO symbols can be null or not null. null symbols are empty spaces
    bool isNull;
    bool isCapital;
    string symb = "none";
protected:
    char letter;
};

class rSymb : public Symbols {
public:
    rSymb(int row, int column) {
        this->letter = 'r';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "r";
        this->isCapital = false;
    }


};

class RSymb : public Symbols {
public:
    RSymb(int row, int column) {
        this->letter = 'R';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "r";
        this->isCapital = true;

    }


};

class sSymb : public Symbols {
public:
    sSymb(int row, int column) {
        this->letter = 's';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "s";
        this->isCapital = false;
    }


};

class SSymb : public Symbols {
public:
    SSymb(int row, int column) {
        this->letter = 'S';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "s";
        this->isCapital = true;

    }


};

class pSymb : public Symbols {
public:
    pSymb(int row, int column) {
        this->letter = 'p';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "p";
        this->isCapital = false;

    }


};

class PSymb : public Symbols {
public:
    PSymb(int row, int column) {
        this->letter = 'P';
        this->isNull = false;
        this->pos = tuple<int, int>(row, column);
        this->symb = "p";
        this->isCapital = true;
    }


};

//ITEM 1.b: fixed Flags (fixed because int move() methods check the letter of symbol, they do not move flags)
class FlagCapital : public Symbols {
public:
    FlagCapital() {
        this->letter = 'F';
        this->isNull = false;
        this->pos = tuple<int, int>(14, 14);
        this->symb = "f";
        this->isCapital = true;
    }
};

class FlagSmall : public Symbols {
public:
    FlagSmall() {
        this->letter = 'f';
        this->isNull = false;
        this->pos = tuple<int, int>(0, 0);
        this->symb = "f";
    }
};

//ITEM 2.b: Mountains - are not movable or mortal
class Mountains : public Symbols {
public:
    Mountains(int row, int column) {
        this->letter = 'M';
        this->isNull = false;
        this->pos = tuple(row, column);
    }
};

class World {
public:
    /**
     * constructor for world
     */
    World() {
        for (int i = 0; i < 15; ++i) {
            vector<Symbols> row;
            for (int j = 0; j < 15; ++j) {
                Symbols s(i, j);
                row.push_back(s);
            }
            map.push_back(row);
        }
    }

    //ITEM 2.a: initial setup, filling the world
    /**
     * fills the world as in setup
     */
    void fillWorld() {
        map[0][0] = FlagSmall();
        map[14][14] = FlagCapital();
        for (int i = 1; i < 6; ++i) {
            //ITEM 3.b: usage of my smart pointers
            SmartPointer<rSymb> s;
            s = rSymb(0, i);
            map[0][i] = *s;
        }
        for (int i = 1; i < 6; ++i) {
            SmartPointer<pSymb> s;
            s = pSymb(0, i);
            map[1][i] = *s;
        }
        for (int i = 1; i < 6; ++i) {
            SmartPointer<sSymb> s;
            s = sSymb(2, i);
            map[2][i] = *s;
        }
        for (int i = 1; i < 6; ++i) {
            SmartPointer<rSymb> s;
            s = rSymb(3, i);
            map[3][i] = *s;
        }
        for (int i = 1; i < 6; ++i) {
            SmartPointer<pSymb> s;
            s = pSymb(4, i);
            map[4][i] = *s;
        }
        for (int i = 1; i < 6; ++i) {
            SmartPointer<sSymb> s;
            s = sSymb(5, i);
            map[5][i] = *s;
        }

        for (int i = 9; i < 14; ++i) {
            SmartPointer<RSymb> s;
            s = RSymb(11, i);
            map[11][i] = *s;
        }
        for (int i = 9; i < 14; ++i) {
            SmartPointer<PSymb> s;
            s = PSymb(10, i);
            map[10][i] = *s;
        }
        for (int i = 9; i < 14; ++i) {
            SmartPointer<SSymb> s;
            s = SSymb(9, i);
            map[9][i] = *s;
        }
        for (int i = 9; i < 14; ++i) {
            SmartPointer<RSymb> s;
            s = RSymb(14, i);
            map[14][i] = *s;
        }
        for (int i = 9; i < 14; ++i) {
            SmartPointer<PSymb> s;
            s = PSymb(13, i);
            map[13][i] = *s;
        }
        for (int i = 9; i < 14; ++i) {
            SmartPointer<SSymb> s;
            s = SSymb(12, i);
            map[12][i] = *s;
        }

        SmartPointer<Mountains> s;
        s = Mountains(2, 10);
        map[2][10] = *s;
        s = Mountains(2, 12);
        map[2][12] = *s;

        s = Mountains(4, 9);
        map[4][9] = *s;
        s = Mountains(5, 10);
        map[5][10] = *s;
        s = Mountains(5, 11);
        map[5][11] = *s;
        s = Mountains(5, 12);
        map[5][12] = *s;
        s = Mountains(4, 13);
        map[4][13] = *s;

        s = Mountains(7, 6);
        map[7][6] = *s;
        s = Mountains(7, 7);
        map[7][7] = *s;
        s = Mountains(7, 8);
        map[7][8] = *s;

        s = Mountains(10, 2);
        map[10][2] = *s;
        s = Mountains(11, 2);
        map[11][2] = *s;
        s = Mountains(12, 2);
        map[12][2] = *s;

        s = Mountains(10, 4);
        map[10][4] = *s;
        s = Mountains(11, 4);
        map[11][4] = *s;
        s = Mountains(12, 4);
        map[12][4] = *s;
        s = Mountains(12, 5);
        map[12][5] = *s;
        s = Mountains(10, 6);
        map[10][6] = *s;
        s = Mountains(11, 6);
        map[11][6] = *s;
        s = Mountains(12, 6);
        map[12][6] = *s;


    }

    /**
     * prints the whole world
     */
    void printWorld() {
        for (int i = 0; i < 15; ++i) {
            for (int j = 0; j < 15; ++j) {
                cout << map[i][j].getLetter() << " ";
            }
            cout << endl;
        }
    }

    /**
     * changes position of given symbol
     * if new position is equal to flag's position, game is over
     * @param symbol - given symbol
     * @param newPos - new position of given symbol
     */
    void changePos(Symbols &symbol, tuple<int, int> &newPos) {
        int oldRow = get<0>(symbol.pos);
        int oldColumn = get<1>(symbol.pos);
        Symbols s;
        get<0>(s.pos) = oldRow;
        get<1>(s.pos) = oldColumn;

        int newRow = get<0>(newPos);
        int newColumn = get<1>(newPos);

        if (map[newRow][newColumn] != symbol) {

            if (map[newRow][newColumn].symb != "f") {
                if (symbol.symb == map[newRow][newColumn].symb ||
                    (map[newRow][newColumn].symb == "p" && symbol.symb == "s") || map[newRow][newColumn].isNull) {
                    map[newRow][newColumn] = symbol;
                    symbol.pos = newPos;
                    map[oldRow][oldColumn] = s;
                }

            } else {
                cout << "Game is over" << endl;
                if (map[newRow][newColumn].isCapital) {
                    cout << "Player 0 has won" << endl;
                } else {
                    cout << "Player 1 has won" << endl;
                }
                gameOver =true;
            }
        }
    }
};

class Action : public World {
public:

    //ITEM 2.d: Only one unit can occupy one position in the maze at a time.
    //ITEM 4.a-g: all movement rules are described here
    /**
     * moving given symbol one position to the up
     * if there is a mountain, move symbol down
     * @param symbol
     */
    void moveSymbolUp(Symbols &symbol) {
        this->from.pos = symbol.pos;
        tuple<int, int> temp = symbol.pos;
        int row = get<0>(temp);
        int column = get<1>(temp);

        tuple<int, int> newPos(row - 1, column);
        if (map[get<0>(newPos)][get<1>(newPos)].getLetter() != 'M') {
            changePos(symbol, newPos);
            symbol.pos = newPos;
            map[row - 1][column].pos = newPos;
            this->to.pos = newPos;
        } else {
            moveSymbolDown(symbol);
            this->to.pos = symbol.pos;
        }

    }

    /**
     * moving symbol one position to the down
     * if there is a mountain, move symbol to the left
     * @param symbol
     */
    void moveSymbolDown(Symbols &symbol) {
        this->from.pos = symbol.pos;
        tuple<int, int> temp = symbol.pos;
        int row = get<0>(temp);
        int column = get<1>(temp);

        tuple<int, int> newPos(row + 1, column);
        if (map[get<0>(newPos)][get<1>(newPos)].getLetter() != 'M') {
            changePos(symbol, newPos);
            map[row + 1][column].pos = newPos;
            this->to.pos = newPos;
        } else {
            moveSymbolLeft(symbol);
            this->to.pos = symbol.pos;
        }
    }

    /**
     * moving given symbol one position to the left
     * if there is a mountain, move symbol right
     * @param symbol
     */
    void moveSymbolLeft(Symbols &symbol) {
        this->from.pos = symbol.pos;
        tuple<int, int> temp = symbol.pos;
        int row = get<0>(temp);
        int column = get<1>(temp);

        tuple<int, int> newPos(row, column - 1);
        if (map[get<0>(newPos)][get<1>(newPos)].getLetter() != 'M') {
            changePos(symbol, newPos);
            map[row][column - 1].pos = newPos;
            this->to.pos = newPos;
        } else {
            moveSymbolRight(symbol);
            this->to.pos = symbol.pos;
        }
    }

    /**
     * moving given symbol  one position up to the right
     * if there is a mountain, move symbol up
     * @param symbol
     */
    void moveSymbolRight(Symbols &symbol) {
        this->from.pos = symbol.pos;
        tuple<int, int> temp = symbol.pos;
        int row = get<0>(temp);
        int column = get<1>(temp);

        tuple<int, int> newPos(row, column + 1);
        if (map[get<0>(newPos)][get<1>(newPos)].getLetter() != 'M') {
            changePos(symbol, newPos);
            map[row][column + 1].pos = newPos;
            this->to.pos = newPos;
        } else {
            moveSymbolUp(symbol);
            this->to.pos = symbol.pos;
        }
    }

    /**
     * gives information about left lower corer - 4 cells
     * @return: 0 - if corner has empty spaces at map[13-14][1]
     *          1 - if map[13-14][0] are not null
     *          2 - if corner is full of non-null symbols
     */
    int infoLLC() {
        if ((!map[14][0].isNull && !map[13][0].isNull) && (map[14][1].isNull || map[13][1].isNull)) {
            return 1;
        } else if (!map[14][0].isNull && !map[14][1].isNull && !map[13][0].isNull && !map[13][1].isNull) {
            return 2;
        } else {
            return 0;
        }
    }

    /**
     * special method for player 1
     * moves 4 symbols to the lower left corner
     */
    void moveArmyLeft() {
        if (infoLLC() == 0) {
            int randRow = rand() % (14 - 13 + 1) + 13;
            int i = 0;
            while (map[randRow][i].isNull) {
                i++;
            }
            if (i != 0) {
                if(map[randRow][i].isCapital)
                    moveSymbolLeft(map[randRow][i]);
            } else {
                if (randRow == 14) {
                    int j = 0;
                    while (map[13][j].isNull) {
                        j++;
                    }
                    if(map[13][j].isCapital)
                        moveSymbolLeft(map[13][j]);
                } else {
                    int j = 0;
                    while (map[14][j].isNull) {
                        j++;
                    }
                    if(map[14][j].isCapital)
                        moveSymbolLeft(map[14][j]);
                }
            }
        } else if (infoLLC() == 1) {
            int randRow = rand() % (14 - 13 + 1) + 13;
            int i = 1;
            while (map[randRow][i].isNull) {
                i++;
            }
            if (i != 1) {
                if(map[randRow][i].isCapital)
                    moveSymbolLeft(map[randRow][i]);
            } else {
                if (randRow == 14) {
                    int j = 1;
                    while (map[13][j].isNull) {
                        j++;
                    }
                    if(map[13][j].isCapital)
                        moveSymbolLeft(map[13][j]);
                } else {
                    int j = 1;
                    while (map[14][j].isNull) {
                        j++;
                    }
                    if(map[14][j].isCapital)
                        moveSymbolLeft(map[14][j]);
                }
            }
        }
    }

    /**
     * calculates the distance between army at LLC
     * @return distance
     */
    int dist() {
        int sum = 0;
        int i = 14;
        while (map[i][0].isNull) {
            i--;
        }
        int k = i - 1;
        while (map[k][0].isNull) {
            k--;
        }
        sum += (i - k);

        int j = 14;
        while (map[j][1].isNull) {
            j--;
        }
        int h = j - 1;
        while (map[h][1].isNull) {
            h--;
        }
        sum += (j - h);
        return sum - 2;
    }

    /**
     * special method for player 1
     * moves 4 symbols to the enemy's flag
     */
    void moveArmyUp() {
        if (dist() < 2) {
            int randNum = rand() % (1 - 0 + 1) + 0;
            int i = 14;
            while (map[i][randNum].isNull) {
                i--;
            }
            i--;
            while (map[i][randNum].isNull) {
                i--;
            }
            moveSymbolUp(map[i][randNum]);
        } else {
            int randNum = rand() % (1 - 0 + 1) + 0;
            int i = 14;
            while (map[i][randNum].isNull) {
                i--;
            }
            if (map[i - 1][randNum].isNull) {
                moveSymbolUp(map[i][randNum]);
            } else {
                if (randNum == 0) {
                    moveSymbolUp(map[i - 1][1]);
                } else {
                    moveSymbolUp(map[i - 1][0]);
                }
            }

        }
    }

    /**
     * randomly chooses a symbol and moves its randomly to enemy's flag
     */
    void moveSymbol() {
        int ran = rand() % (2 - (-2) + 1) + (-2);//-2,-1,0,1,2

        if (ran > 0) {
            int i = 14, j = 14;
            while (!map[i][j].isNull) {
                i--;
                j--;
            }
            i++, j++;
            i -= ran;
            if (map[i - 1][j].isNull) {
                moveSymbolUp(map[i][j]);
            } else {
                moveSymbolLeft(map[i][j]);
            }
        } else {
            int i = 14, j = 14;
            while (!map[i][j].isNull) {
                i--;
                j--;
            }
            i++, j++;
            j += ran;
            if (map[i - 1][j].isNull) {
                if (map[i - 1][j].getLetter() == 'M' || !map[i - 1][j].isNull) {
                    moveSymbolLeft(map[i][j]);
                } else {
                    moveSymbolUp(map[i][j]);
                }
            } else {
                if (map[i][j - 1].getLetter() == 'M' || !map[i][j - 1].isNull) {
                    moveSymbolUp(map[i][j]);
                } else {
                    moveSymbolLeft(map[i][j]);
                }
            }
        }

    }

private:
    Position from; // current row, column of the unit to be moved
    Position to; // position to where the unit must be moved

};

/**
 * first player should build a defence wall for his flag
 * this method shows whether defence is built ot not
 * @return
 */
bool defenceIsBuilt() {
    for (int i = 12; i < 15; ++i) {
        for (int j = 11; j < 15; ++j) {
            if (map[i][j].isNull) {
                return false;
            }
        }
    }
    return true;
}

//shows whether 4 symbols were in lower left corner
bool LLCwasFilled = false;

bool mapHasChanged(vector<vector<Symbols>> nMap){
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if(map[i][j]!=nMap[i][j]){
                return true;
            }
        }
    }
    return false;
}

//ITEM 2.c: controls only capital letters
//ITEM 3.b: strategy for player one:
//ITEM 3.c:
/**
 * player 1 firstly builds defence wall around his flag
 * then this player has dodgy strategy:
 * he moves 4 symbols through the corner to enemy's flag - he is trying to avoid small letters
 * @param world - given world
 * @return - action
 */
Action actionPlayerOne(const World world) {
    vector<vector<Symbols>> temp = map;
    Action act;
    //making a layer around flag

    map.erase(map.cbegin() + 15, map.end());


    if (!defenceIsBuilt()) {
        if (map[13][14].isNull) {
            if (map[12][14].isNull) {
                if (map[11][14].isNull) {
                    if(map[11][13].isCapital)
                        act.moveSymbolRight(map[11][13]);
                } else {
                    if(map[11][14].isCapital)
                        act.moveSymbolDown(map[11][14]);
                }
            } else {
                if(map[12][14].isCapital)
                    act.moveSymbolDown(map[12][14]);
            }
        } else {

            if (map[12][14].isNull) {
                if (map[11][14].isNull) {
                    if (map[10][14].isNull) {
                        if(map[10][13].isCapital)
                            act.moveSymbolRight(map[10][13]);
                    } else {
                        if(map[10][14].isCapital)
                            act.moveSymbolDown(map[10][14]);
                    }
                } else {
                    if(map[11][14].isCapital)
                        act.moveSymbolDown(map[11][14]);
                }
            } else {
                if(map[12][14].isCapital)
                    act.moveSymbolDown(map[12][14]);
            }
        }
    } else {//start the attack
        if (act.infoLLC() != 2 && !LLCwasFilled) {
            act.moveArmyLeft();
            if (act.infoLLC() == 2) {
                LLCwasFilled = true;
            }
        } else {
            int randNum =
                    rand() % (2 - 1 + 1) + 1;//if 1 - move symbols at LLC up, if 2 - move other symbols on diagonal.
            if (randNum == 1) {
                act.moveArmyUp();
            } else {
                act.moveSymbol();
            }
        }
    }

    if(!mapHasChanged(temp)){
        this_thread::sleep_for(chrono::milliseconds (PENALTY));
    }

    cout << "actionPlayerOne()" << endl;
    this_thread::sleep_for(385ms); // 0.4 seconds
    return act;
}

//ITEM 2.c: controls only small letters
//ITEM 3.b: strategy for player zero:
//ITEM 3.c:
/**
 * player 0 moves his small letters by diagonal to enemy's flag
 * it is like a stupid dude who only wants to attack
 * @param world
 * @return
 */
Action actionPlayerZero(const World world) {
    vector<vector<Symbols>> temp = map;
    Action act;
    map.erase(map.cbegin() + 15, map.end());

    int ran = rand() % (5 - (-5) + 1) + (-5);//-5,-4,-3,-2,-1,0,1,2,3,4,5

    if (ran > 0) {
        int i = 14, j = 14;
        while (map[i][j].isCapital || map[i][j].isNull || map[i][j].getLetter() == 'M') {
            i--;
            j--;
        }
        i -= ran;
        if(i<1){
            i+=2*ran;
        }
        if (map[i + 1][j].isNull) {
            act.moveSymbolDown(map[i][j]);
        } else {
            act.moveSymbolRight(map[i][j]);
        }
    } else {
        int i = 14, j = 14;
        while (map[i][j].isCapital || map[i][j].isNull || map[i][j].getLetter() == 'M') {
            i--;
            j--;
        }
        j += ran;
        if(j<1){
            j+=2*ran;
        }
        if (map[i + 1][j].isNull && map[i + 1][j].getLetter() != 'M') {
            act.moveSymbolDown(map[i][j]);
        } else {
            if (map[i + 1][j].getLetter() == 'M' || !map[i + 1][j].isNull) {
                act.moveSymbolRight(map[i][j]);
            } else {
                act.moveSymbolDown(map[i][j]);
            }
        }
    }

    if(!mapHasChanged(temp)){
        this_thread::sleep_for(chrono::milliseconds (PENALTY));
    }

    cout << "actionPlayerZero()" << endl;
    this_thread::sleep_for(385ms); // 0.4 seconds
    return act;
}

/**
 * The return is a pair: action and a boolean whether a timeout happened
 */
tuple<Action, bool> waitPlayer(Action (*f)(World), World &world) {
    auto start = chrono::high_resolution_clock::now();
    Action action = f(world);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;
    cout << "Waited " << elapsed.count() << " ms\n";

    //ITEM 3.a: player looses if time is over
    if (elapsed.count() > TIMEOUT) // if time > 0.4 s
        return {action, true}; // player failed to answer in less than 400 ms
    else return {action, false};
}

int main() {
    World world;
    world.fillWorld();
    world.printWorld();

    bool endGame = false;
    while (!endGame) {
        world.printWorld();
        auto[action0, timeout0] = waitPlayer(actionPlayerZero, world);
        auto[action1, timeout1] = waitPlayer(actionPlayerOne, world);
        if (timeout0 || timeout1) {
            endGame = true;
            if(timeout0){
                cout<<"Player 0 caused the timeout"<<endl;
                cout << "Player 1 has won" << endl;

            }else if(timeout1){
                cout<<"Player 1 caused the timeout"<<endl;
                cout << "Player 0 has won" << endl;
            }

        } else {
            if(gameOver){
                endGame=true;
            }
        }
    }

    return 0;
}