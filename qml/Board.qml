import QtQuick 2.12
import Settings 1.0
import GameModel 1.0

GridView {
    id: _view
    interactive : false
    property Item score

    property int ball_in_hand: Settings.no_ball_in_hand
    model: BoardModel{
        core : game_core
        onBallMoved : (index_from, index_to, ball_type)=> {
            _ball_mover.move(index_from, index_to, ball_type);
        }
        onBallAdded : (cell)=> {
            _view.itemAtIndex(cell).fadeIn();
        }
        onBallRemoved : (cell)=> {
            _view.itemAtIndex(cell).fadeOut();
        }
        onScoreChanged:{
            score.change_score(game_core.score());
        }
        onGameOver:{
            score.text = qsTr("КОНЕЦ! Итого: ") + game_core.score();
        }
        onGameStarted:{
            for (var i = 0; i < _view.count; i++)
                _view.itemAtIndex(i).opacity = 0;
        }
    }

    property int activ_anim_num: 0
    function animation_started(){
        activ_anim_num += 1;
    }
    function animation_finished()
    {
        activ_anim_num -= 1;
        if (activ_anim_num <= 0){
            activ_anim_num = 0
            if (!model.emitNextAnimSignal())
                model.updateView();
        }
    }

    delegate: BallStatic{
        id: _ball
        ball_type: display
        cell_side: _view.cellHeight
        view: _view
    }
    BallMover{
        id: _ball_mover
        cell_side: _view.cellHeight
        view: _view
    }

    Component.onCompleted: game_core.loadGame()
}
