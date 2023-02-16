#include "gamecore.h"

GameCore::GameCore(int board_size, size_t ball_types_num) :
    m_board_size{board_size},
    m_board_cells_num {m_board_size*m_board_size}
{
    m_board_cells.assign(m_board_cells_num, BALL_TYPE_NO);
    m_free_cell_indexs.reserve(m_board_cells_num);
    ball_types.reserve(ball_types_num);
    for (size_t i = 0; i < ball_types_num; ++i)
    {
        ball_types.emplace_back(static_cast<int>(i+1));
    }
    m_db_handler = new DBHandler(m_board_cells_num, BALL_TYPE_NO);
}

void GameCore::showBoard() const
{
    int row_counter {0};
    for (const int& ball : m_board_cells)
    {
        std::cout << ball << " ";
        row_counter++;
        if (row_counter >= m_board_size)
        {
            std::cout << std::endl;
            row_counter = 0;
        }
    }
    std::cout << "-------------" << m_score << std::endl;

}

int GameCore::boardSize() const
{
    return m_board_size;
}

int GameCore::boardCellsNum() const
{
    return static_cast<int>(m_board_cells_num);
}

void GameCore::newGame()
{
    m_board_cells.assign(m_board_cells_num, BALL_TYPE_NO);
    m_db_handler->clearBoard();
    setScore(0);
    addRandomBall();
    emit gameStarted();
}

void GameCore::loadGame()
{
    bool need_new_game = m_db_handler->newDB();
    if (need_new_game){
        addRandomBall();
    }
    else{
        std::vector<int> loaded_cells = m_db_handler->selectAllCells();
        for (size_t i=0; i< loaded_cells.size(); ++i){
            int ball_type = loaded_cells.at(i);
            if (ball_type != BALL_TYPE_NO){
                setBall(static_cast<int>(i), ball_type, false);
            }
        }
    }
    setScore(m_db_handler->selectScore());

    emit gameStarted();
}

int GameCore::getCellBall(const int index)
{
    if (!isValidIndex(index)) return NO_CELL;

    return m_board_cells.at(index);
}

bool GameCore::setBall(const int index, const int ball_type, const bool use_db)
{
    if (!isValidIndex(index)) return false;

    int* cur_cell = &m_board_cells.at(index);
    if (*cur_cell != BALL_TYPE_NO)
    {
        std::cout << "!!! Ball collision !!!" << std::endl;
        return false;
    }

    *cur_cell = ball_type;
    if (use_db){
       m_db_handler->updateCell(index, ball_type);
    }
    emit ballAdded(index);
    return true;
}

bool GameCore::moveBall(const int index_from, const int index_to)
{
    if (!isValidIndex(index_from) || !isValidIndex(index_to)) return false;
    int* cell_from = &m_board_cells.at(index_from);
    if (*cell_from == BALL_TYPE_NO)
    {
        std::cout << "!!! No ball to move !!!" << std::endl;
        return false;
    }
    int* cell_to = &m_board_cells.at(index_to);
    if (*cell_to != BALL_TYPE_NO)
    {
        std::cout << "!!! Ball collision !!!" << std::endl;
        return false;
    }

    *cell_to = *cell_from;
    *cell_from = BALL_TYPE_NO;
    m_db_handler->updateCell(index_from, BALL_TYPE_NO);
    m_db_handler->updateCell(index_to, *cell_to);
    emit ballMoved(index_from, index_to, *cell_to);

    if (!checkLineFrom(index_to))
        addRandomBall();

    return true;
}

int GameCore::noBall() const
{
    return BALL_TYPE_NO;
}

int GameCore::score() const
{
    return m_score;
}

void GameCore::addRandomBall()
{
    //собираем список свободных ячеек
    m_free_cell_indexs.clear();
    for (size_t i=0; i<m_board_cells.size(); ++i)
    {
        if(m_board_cells.at(i) == BALL_TYPE_NO)
        {
            m_free_cell_indexs.push_back(i);
        }
    }
    //уточняем количество вставляемых шаров
    const size_t ball_num = std::min(m_random_ball_num, m_free_cell_indexs.size());
    if (ball_num <= 0)
    {
        emit gameOver();
        return;
    }

    //случайным образом выбираем одно из свободных место и цвет нового шара,
    //ставим шар, и попутно запоминаем, куда поставили
    std::random_device rd;
    std::default_random_engine random_engine(rd());
    std::uniform_int_distribution<int> color_distrib (0, ball_types.size()-1);
    std::vector<size_t> insert_cells;
    insert_cells.reserve(ball_num);

    for (size_t i=0; i<ball_num; ++i)
    {
        std::uniform_int_distribution<size_t> index_distrib (0, m_free_cell_indexs.size()-1);
        size_t* free_cell_index = &m_free_cell_indexs.at(index_distrib(random_engine));
        setBall(static_cast<int>(*free_cell_index),
                 ball_types.at(color_distrib(random_engine))
                 );
        insert_cells.push_back(*free_cell_index);
        *free_cell_index = m_free_cell_indexs.back();
        m_free_cell_indexs.pop_back();
    }

    //только после того, как выставили все шары, проверяем их на случайное составление линий
    bool has_lines{false};
    for (const size_t& cell_index : insert_cells)
    {
        has_lines = checkLineFrom(cell_index) || has_lines;
    }
    //если двигать шары больше некуда, и при этом линии не составились, то gameover
    if (!has_lines && m_free_cell_indexs.empty())
    {
        emit gameOver();
    }
}

bool GameCore::checkLineFrom(const int index)
{
    if (!isValidIndex(index)) return false;
    const int line_type = m_board_cells.at(index);
    if (line_type == BALL_TYPE_NO) return false;

    //сначала проверим, есть ли хотя бы один сосед того же цвета
    bool has_neighbour {false};
    std::vector<int> neighbours{
        getNeighbourIndexUp(index),
        getNeighbourIndexDown(index),
        getNeighbourIndexRight(index),
        getNeighbourIndexLeft(index)
    };
    for (const int& neighbour_index : neighbours)
    {
        has_neighbour = (neighbour_index != NO_CELL && m_board_cells.at(neighbour_index) == line_type);
        if (has_neighbour) break;
    }
    if (!has_neighbour) return false;

    //сосед есть, значит есть смысл искать линию
    std::vector<int> line_vector;
    line_vector.reserve(m_board_size);
    int cur_index = index;
    line_vector.push_back(cur_index);

    //вверх
    for (int i = 0; i<m_board_size; ++i)
    {
        cur_index = getNeighbourIndexUp(cur_index);
        if (cur_index == NO_CELL || !addNeighbourToLine(cur_index, line_type, &line_vector))
        {
            cur_index = index;
            break;
        }
    }
    //вниз
    for (int i = 0; i<m_board_size; ++i)
    {
        cur_index = getNeighbourIndexDown(cur_index);
        if (cur_index == NO_CELL || !addNeighbourToLine(cur_index, line_type, &line_vector))
        {
            cur_index = index;
            break;
        }
    }

    if (line_vector.size() < m_line_ball_num)
    {
        line_vector.erase(line_vector.begin()+1, line_vector.end());
        //вправо
        for (int i = 0; i<m_board_size; ++i)
        {
            cur_index = getNeighbourIndexRight(cur_index);
            if (cur_index == NO_CELL || !addNeighbourToLine(cur_index, line_type, &line_vector))
            {
                cur_index = index;
                break;
            }
        }
        //влево
        for (int i = 0; i<m_board_size; ++i)
        {
            cur_index = getNeighbourIndexLeft(cur_index);
            if (cur_index == NO_CELL || !addNeighbourToLine(cur_index, line_type, &line_vector))
            {
                cur_index = index;
                break;
            }
        }
    }

    if (line_vector.size() < m_line_ball_num) return false;

    //нашли линию, считаем очки
    setScore(m_score + m_score_line + m_score_extra_ball * static_cast<int>(line_vector.size() - m_line_ball_num));

    //убираем линию
    for (const int& ball_index : line_vector)
    {
        m_board_cells.at(ball_index) = BALL_TYPE_NO;
        m_db_handler->updateCell(ball_index, BALL_TYPE_NO);
        emit ballRemoved(ball_index);
    }
    return true;
}

void GameCore::setScore(const int new_score)
{
    m_score = new_score;
    m_db_handler->updateScore(new_score);
    emit scoreChanged();
}

bool GameCore::isValidIndex(const int index)
{
    return !(index < 0 || static_cast<size_t>(index) >= m_board_cells_num);
}

int GameCore::getNeighbourIndexUp(const int index)
{
    int neighbour_index = index - m_board_size;
    if (neighbour_index < 0) return NO_CELL;
    return neighbour_index;
}

int GameCore::getNeighbourIndexDown(const int index)
{
    int neighbour_index = index + m_board_size;
    if (neighbour_index >= static_cast<int>(m_board_cells_num)) return NO_CELL;
    return neighbour_index;
}

int GameCore::getNeighbourIndexRight(const int index)
{
    int neighbour_index = index+1;
    if (neighbour_index % m_board_size == 0) return NO_CELL;
    return neighbour_index;
}

int GameCore::getNeighbourIndexLeft(const int index)
{
    if (index % m_board_size == 0) return NO_CELL;
    return index-1;
}

bool GameCore::addNeighbourToLine(const int index, const int ball_type, std::vector<int> *line)
{
    if (m_board_cells.at(index) == ball_type)
    {
        line->push_back(index);
        return true;
    }
    return false;
}

