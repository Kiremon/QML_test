#ifndef GAMECORE_H
#define GAMECORE_H

#include <QObject>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include "dbhandler.h"

class GameCore : public QObject
{
    Q_OBJECT

public: 
    GameCore(int board_size = 9, size_t ball_types_num = 4);

    static constexpr int BALL_TYPE_NO{0};
    static const int NO_CELL{-1};
    std::vector<int> ball_types;

    int getCellBall(const int index);
    bool setBall(const int index, const int ball_type, const bool use_db = true);
    bool moveBall(const int index_from, const int index_to);

public slots:
    int noBall() const;
    int score() const;
    int boardSize() const;
    int boardCellsNum() const;
    void newGame();
    void loadGame();

signals:
    void scoreChanged();
    void gameStarted();
    void ballMoved(const int index_from, const int index_to, const int ball_type);
    void ballAdded(const int index);
    void ballRemoved(const int index);
    void gameOver();

private:
    const int m_board_size;
    const size_t m_board_cells_num;

    const size_t m_random_ball_num {3};
    const size_t m_line_ball_num {5};
    const int m_score_line {10};
    const int m_score_extra_ball {5};

    int m_score {0};

    void showBoard() const;
    void addRandomBall();
    bool checkLineFrom(const int index);
    void setScore(const int new_score);
    bool isValidIndex(const int index);
    int getNeighbourIndexUp(const int index);
    int getNeighbourIndexDown(const int index);
    int getNeighbourIndexRight(const int index);
    int getNeighbourIndexLeft(const int index);
    bool addNeighbourToLine(const int index, const int ball_type, std::vector<int>* line);

    std::vector<int> m_board_cells;
    std::vector<size_t> m_free_cell_indexs;

    DBHandler* m_db_handler;
};

#endif // GAMECORE_H
