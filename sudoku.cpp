#include <vector>

class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        // set up matrix of possibilities
        vector<vector<vector<char>>> board3D(9, vector<vector<char>>(9, vector<char>(9, '.')));
        fillEmptyBoxes(board, board3D);

        // solve the sudoku
        guessingLoop(board, board3D);
    }

    // first places numbers that can logically only be placed in a certain box until no more placements can be determined
    // if the sudoku is not solved and there are no contradictions, it makes a guess for the box with the least possibilities and recurs itself
    // if there is a contradiction, it returns to the previous recursion, which reverts the guess, makes a new guess, and recurs itself
    int guessingLoop(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D) {
        // logical placement of numbers
        int changesMade = 1;
        while (changesMade > 0) {
            changesMade = 0;

            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (board[i][j] == '.' && placeIfPossible(board, board3D, i, j) ) {
                        ++changesMade;
                    }
                }
            }
        }

        // checks board state and decides whether to revert and guess again, keep going and make a new guess for another box, or return success
        int state = boardState(board, board3D);
        if (state == 2) {
            return 1;
        } else if (state == 1) {
            // finds empty box with least possibilities
            int bestX = -1, bestY = -1, minCount = 10;
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (board[i][j] == '.') {
                        int count = 0;
                        for (int k = 0; k < 9; ++k) {
                            if (board3D[i][j][k] != '.') {
                                ++count;
                            }
                        }
                        if (count < minCount) {
                            minCount = count;
                            bestX = i; bestY = j;
                        }
                    }
                }
            }

            // guesses the chosen box until it gets it right
            for (int k = 0; k < 9; ++k) {
                if (board3D[bestX][bestY][k] != '.') {
                    char charNum = '1' + k;
                    vector<vector<char>> savedBoard = board;
                    vector<vector<vector<char>>> saved3D = board3D;

                    // make educated guess
                    board[bestX][bestY] = charNum;
                    removeUpdateImpossibilities(board, board3D, bestX, bestY);

                    // recursive call
                    if (guessingLoop(board, board3D)) {
                        return 1;
                    }

                    // revert back
                    board = savedBoard;
                    board3D = saved3D;
                }
            }
        }
        
        return 0;
    }
    // checks if solved, unsolved valid, or unsolved contradicting
    int boardState(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D) {
        int state = 2;

        // check for boxes with no possibilities
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] == '.') {
                    state = 0;
                    for (int k = 0; state == 0 && k < 9; ++k) {
                        if (board3D[i][j][k] != '.') {
                            state = 1;
                        }
                    }
                    if (state == 0) {
                        return 0;
                    }
                }
            }
        }

        // check for rows, columns, or bigBoxes that can't have a certain number in any box
        for (int k = 0; k < 9; ++k) {
            char charNum = '1' + k;

            //columns
            for (int i = 0; i < 9; ++i) {
                int found = 0;
                for (int j = 0; found == 0 && j < 9; ++j) {
                    if (board[i][j] == charNum || board3D[i][j][k] == charNum) {
                        found = 1;
                    }
                }
                if (found == 0) {
                    return 0;
                }
            }

            //rows
            for (int j = 0; j < 9; ++j) {
                int found = 0;
                for (int i = 0; found == 0 && i < 9; ++i) {
                    if (board[i][j] == charNum || board3D[i][j][k] == charNum) {
                        found = 1;
                    }
                }
                if (found == 0) {
                    return 0;
                }
            }

            //bigBoxes
            for (int b = 0; b < 9; ++b) {
                int bigBoxX = b % 3;
                int bigBoxY = b / 3;
                int found = 0;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; found == 0 && j < 3; ++j) {
                        if (board[bigBoxX*3 + i][bigBoxY*3 + j] == charNum || board3D[bigBoxX*3 + i][bigBoxY*3 + j][k] == charNum) {
                            found = 1;
                        }
                    }
                }
                if (found == 0) {
                    return 0;
                }
            }
        }

        return state;
    }
    // if this box is the only box in its row, column, or bigBox with a certain possible number, place that number into board
    // if there is only one possible number left for this box, place that number into board
    int placeIfPossible(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D, int x, int y) {
        int numPeriods = 0;
        char possibility = 'a';

        for (int k = 0; k < 9; ++k) {
            if (board3D[x][y][k] == '.') {
                ++numPeriods;
            } else if (isUniquePossibility(board, board3D, x, y, k)) {
                board[x][y] = board3D[x][y][k];
                removeUpdateImpossibilities(board, board3D, x, y);
                return 1;
            } else {
                possibility = board3D[x][y][k];
            }
        }
        if (numPeriods == 8) {
            board[x][y] = possibility;
            removeUpdateImpossibilities(board, board3D, x, y);
            return 1;
        }

        return 0;
    }  
    // checks if this box is the only box in its row, column, or bigBox with a certain possible number
    int isUniquePossibility(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D, int x, int y, int z) {
        int numPeriods = 0;

        // check this column to see if only one box has this number as a possibility
        for (int j = 0; j < 9; ++j) {
            if (board3D[x][j][z] == '.') {
                ++numPeriods;
            }
        }
        if (numPeriods == 8) {
            return 1;
        }

        // check this row to see if only one box has this number as a possibility
        numPeriods = 0;
        for (int i = 0; i < 9; ++i) {
            if (board3D[i][y][z] == '.') {
                ++numPeriods;
            }
        }
        if (numPeriods == 8) {
            return 1;
        }

        // check all bigboxes to see if only one box has this number as a possibility
        numPeriods = 0;
        int bigBoxX = x / 3;
        int bigBoxY = y / 3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board3D[bigBoxX*3 + i][bigBoxY*3 + j][z] == '.') {
                    ++numPeriods;
                }
            }
        }
        if (numPeriods == 8) {
            return 1;
        }

        return 0;
    }
    // after updating a box in board, remove that possibility from every affected box in board3D
    void removeUpdateImpossibilities(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D, int x, int y) {
        int updateNum = board[x][y] - '0' - 1;

        // remove updateNum from every box in board3D in same column
        for (int j = 0; j < 9; ++j) {
            board3D[x][j][updateNum] = '.';
        }

        // remove updateNum from every box in board3D in same row
        for (int i = 0; i < 9; ++i) {
            board3D[i][y][updateNum] = '.';
        }

        // remove updateNum from every box in board3D in same bigBox
        int bigBoxX = x / 3;
        int bigBoxY = y / 3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board3D[bigBoxX*3 + i][bigBoxY*3 + j][updateNum] = '.';
            }
        }

        // remove all possibilities from updated box
        for (int k = 0; k < 9; ++k) {
            board3D[x][y][k] = '.';
        }
    }
    // fill vectors for empty boxes with numbers 1-9 and then remove impossibilities
    void fillEmptyBoxes(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D) {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (board[i][j] == '.') {
                    for (int k = 0; k < 9; ++k) {
                        board3D[i][j][k] = '1' + k;
                    }

                    removeInitialImpossibilities(board, board3D, i, j);
                }
            }
        }
    }
    // remove all impossibilities from an empty box during initialization
    void removeInitialImpossibilities(vector<vector<char>>& board, vector<vector<vector<char>>>& board3D, int x, int y) {
        // check all boxes in this column to find numbers it cant be and remove them
        for (int j = 0; j < 9; ++j) {
            if (board[x][j] != '.') {
                int z = board[x][j] - '0' - 1;
                board3D[x][y][z] = '.';
            }
        }

        // check all boxes in this row to find numbers it cant be and remove them
        for (int i = 0; i < 9; ++i) {
            if (board[i][y] != '.') {
                int z = board[i][y] - '0' - 1;
                board3D[x][y][z] = '.';
            }
        }

        // check all boxes in this big box to find numbers it cant be and remove them
        int bigBoxX = x / 3;
        int bigBoxY = y / 3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[bigBoxX*3 + i][bigBoxY*3 + j] != '.') {
                    int z = board[bigBoxX*3 + i][bigBoxY*3 + j] - '0' - 1;
                    board3D[x][y][z] = '.';
                }
            }
        }
    }
};