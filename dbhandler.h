#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

#include <vector>
#include <iostream>

class DBHandler : public QObject
{
    Q_OBJECT
public:
    DBHandler(const size_t board_cells_num, const int no_ball_const, const QString db_name = QString("db_board"));
    ~DBHandler();

    bool newDB();
    bool updateCell(const int index, const int ball_type);
    std::vector<int> selectAllCells();
    bool updateScore(const int score);
    int selectScore();
    void clearBoard();

private:
    bool m_db_active{true};
    const size_t m_board_cells_num;
    const int BALL_TYPE_NO;
};

#endif // DBHANDLER_H
