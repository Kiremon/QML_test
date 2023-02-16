#include "boardmodel.h"

BoardModel::BoardModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int BoardModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_core->boardCellsNum();
}

QVariant BoardModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_core->getCellBall(index.row()));
}

GameCore *BoardModel::getCore() const
{
    return m_core;
}

void BoardModel::setCore(GameCore *new_core)
{
    beginResetModel();

    m_core = new_core;
    connect(m_core, SIGNAL(gameStarted()), this, SLOT(newGame()));
    connect(m_core, SIGNAL(ballMoved(int, int, int)), this, SLOT(regAnimBallMoved(int, int, int)));
    connect(m_core, SIGNAL(ballAdded(int)), this, SLOT(regAnimBallAdded(int)));
    connect(m_core, SIGNAL(ballRemoved(int)), this, SLOT(regAnimBallRemoved(int)));
    connect(m_core, SIGNAL(gameOver()), this, SIGNAL(gameOver()));
    connect(m_core, SIGNAL(scoreChanged()), this, SIGNAL(scoreChanged()));

    endResetModel();
}

void BoardModel::regAnimBallMoved(const int index_from, const int index_to, const int ball_type)
{
    AnimAction new_action;
    new_action.anim_type = ANIM_MOVE;
    new_action.index_from = index_from;
    new_action.index_to = index_to;
    new_action.ball_type = ball_type;
    m_anim_queue.push(new_action);

    const QModelIndex q_index_to_update = createIndex(index_to, 0);
    emit dataChanged(q_index_to_update, q_index_to_update);
}

void BoardModel::regAnimBallAdded(const int index)
{
    AnimAction new_action;
    new_action.anim_type = ANIM_ADD;
    new_action.index_to = index;
    m_anim_queue.push(new_action);

    const QModelIndex q_index_to_update = createIndex(index, 0);
    emit dataChanged(q_index_to_update, q_index_to_update);
}

void BoardModel::regAnimBallRemoved(const int index)
{
    AnimAction new_action;
    new_action.anim_type = ANIM_REMOVE;
    new_action.index_from = index;
    m_anim_queue.push(new_action);
}

void BoardModel::newGame()
{
    emit gameStarted();
    updateView();
    emitNextAnimSignal();
}

void BoardModel::putBall(const int cell_from, const int cell_to)
{
    m_core->moveBall(cell_from, cell_to);
    emitNextAnimSignal();
}

int BoardModel::boardSize() const
{
    return m_core->boardSize();
}

void BoardModel::updateView()
{
    emit dataChanged(createIndex(0, 0), createIndex(m_core->boardCellsNum(), 0));
}

bool BoardModel::emitNextAnimSignal()
{
    if (m_anim_queue.empty()) return false;

    AnimAction next = m_anim_queue.front();
    if (next.anim_type == ANIM_MOVE)
    {
        emit ballMoved(next.index_from, next.index_to, next.ball_type);
        m_anim_queue.pop();
    }
    else if (next.anim_type == ANIM_ADD)
    {
        while (next.anim_type == ANIM_ADD)
        {
            emit ballAdded(next.index_to);
            m_anim_queue.pop();
            if (m_anim_queue.empty()) break;
            next = m_anim_queue.front();

        }
    }
    else if (next.anim_type == ANIM_REMOVE)
    {
        while (next.anim_type == ANIM_REMOVE)
        {
            emit ballRemoved(next.index_from);
            m_anim_queue.pop();
            if (m_anim_queue.empty()) break;
            next = m_anim_queue.front();
        }
    }

    return true;
}
