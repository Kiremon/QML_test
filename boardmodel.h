#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <QAbstractListModel>
#include <vector>
#include <queue>
#include <iostream>
#include "gamecore.h"

class BoardModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(GameCore* core READ getCore WRITE setCore)

public:
    BoardModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    GameCore* getCore() const;
    void setCore(GameCore* new_core);

public slots:
    void putBall(const int cell_from, const int cell_to);
    int boardSize() const;
    void updateView();
    bool emitNextAnimSignal();

signals:
    void ballMoved(const int cell_from, const int sell_to, const int ball_type);
    void ballAdded(const int cell);
    void ballRemoved(const int cell);
    void gameOver();
    void scoreChanged();
    void gameStarted();

private slots:
    void regAnimBallMoved(const int index_from, const int index_to, const int ball_type);
    void regAnimBallAdded(const int index);
    void regAnimBallRemoved(const int index);
    void newGame();

private:
    enum {
      ANIM_MOVE,
      ANIM_ADD,
      ANIM_REMOVE
    };
    struct AnimAction {
        int anim_type{};
        int index_from{};
        int index_to{};
        int ball_type{};
    };
    std::queue<AnimAction> m_anim_queue;

    GameCore* m_core;
};

#endif // BOARDMODEL_H
