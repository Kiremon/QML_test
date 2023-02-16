#include "dbhandler.h"

DBHandler::DBHandler(const size_t board_cells_num, const int no_ball_const, const QString db_name) :
    m_board_cells_num{board_cells_num},
    BALL_TYPE_NO{no_ball_const}
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_name);
    if(!db.open()) {
        std::cout << "Что-то пошло не так! Играем без базы! " << db.lastError().text().toStdString() << std::endl;
        m_db_active = false;
    }
}

DBHandler::~DBHandler()
{
    //Нет уверенности, что необходимо закрывать БД в диструкторе,
    //везде пишут, что при работе с одной локальной базой её вообще можно не закрывать явным образом.
    if (m_db_active){
        QSqlDatabase::database().close();
    }
}

bool DBHandler::newDB()
{
    if (!m_db_active) return true;

    QSqlQuery query{};
    query.exec("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='board'");
    query.next();
    bool new_db{query.value(0).toInt() < 1};

    //Вообще, тут нужно ещё выбирать количество записей в board,
    //и, если их не m_board_cells_num, то табличку нужно перезаполнять.
    if (new_db){
        query.exec("CREATE TABLE board("
                        "id INTEGER NOT NULL, "
                        "ball_type INTEGER NOT NULL, "
                        "PRIMARY KEY (id)"
                        ")"
                      );
        for (size_t i=0; i<m_board_cells_num; ++i)
        {
            query.prepare("INSERT INTO board(id, ball_type) "
                            "VALUES(:index, :ball_type)"
                          );
            query.bindValue(":index", static_cast<int>(i));
            query.bindValue(":ball_type", BALL_TYPE_NO);

            query.exec();
        }
        query.exec("CREATE TABLE score("
                       "id INTEGER NOT NULL, "
                       "value INTEGER NOT NULL, "
                       "PRIMARY KEY (id)"
                       ")"
                      );
        query.exec("INSERT INTO score(id, value) "
                        "VALUES(0, 0)"
                   );
    }
    return new_db;
}

bool DBHandler::updateCell(const int index, const int ball_type)
{
    if (!m_db_active) return false;

    QSqlQuery query{};
    query.prepare("UPDATE board SET ball_type = :ball_type WHERE id = :id");
    query.bindValue(":ball_type", ball_type);
    query.bindValue(":id", index);
    return query.exec();
}

std::vector<int> DBHandler::selectAllCells()
{
    if (!m_db_active) std::vector<int> {};

    QSqlQuery query{};
    query.exec("SELECT id, ball_type FROM board");
    std::vector<int> result(m_board_cells_num, 0);
    while(query.next()) {
        result.at(query.value(0).toInt()) = query.value(1).toInt();
    }
    return result;
}

bool DBHandler::updateScore(const int score)
{
    if (!m_db_active) return false;

    QSqlQuery query{};
    query.prepare("UPDATE score "
                    "SET value = :score "
                    "WHERE id = 0"
                   );
    query.bindValue(":score", QVariant(score));
    return query.exec();
}

int DBHandler::selectScore()
{
    if (!m_db_active) return 0;

    QSqlQuery query{};
    query.exec("SELECT value FROM score");
    query.next();
    return query.value(0).toInt();
}

void DBHandler::clearBoard()
{
    for (size_t i=0; i<m_board_cells_num; ++i){
        updateCell(static_cast<int>(i), BALL_TYPE_NO);
    }
}
