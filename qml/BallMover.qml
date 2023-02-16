import QtQuick 2.12
import Settings 1.0

Ball {
    id: _ball
    property Item view
    property int finish_index
    opacity: 0
    function move (index_from, index_to, ball_type){
        var ball_from = view.itemAtIndex(index_from);
        var ball_to = view.itemAtIndex(index_to);
        opacity = 1;
        finish_index = index_to;
        _ball.ball_type = ball_type
        ball_from.opacity = 0;
        _anim_move_x.from = ball_from.x;
        _anim_move_y.from = ball_from.y;
        _anim_move_x.to = ball_to.x;
        _anim_move_y.to = ball_to.y;
        _anim_move.start();
    }
    ParallelAnimation {
        id: _anim_move
        NumberAnimation {
            id: _anim_move_x
            target: _ball
            property: "x"
            duration: 500
        }
        NumberAnimation {
            id: _anim_move_y
            target: _ball
            property: "y"
            duration: 500
        }
        onStarted: {
            view.animation_started();
        }
        onFinished: {
            _ball.opacity = 0;
            var ball_to = view.itemAtIndex(_ball.finish_index);
            ball_to.opacity = 1;
            view.animation_finished();
        }
    }
}
