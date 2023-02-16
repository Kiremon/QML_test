import QtQuick 2.12
import Settings 1.0

Ball {
    id: _ball
    property Item view
    opacity: 0
    MouseArea {
        anchors.fill: parent
        onClicked: {
                if (_ball.ball_type !== game_core.noBall())
                {
                    if (view.ball_in_hand === index)
                    {
                        view.ball_in_hand = Settings.no_ball_in_hand;
                        _ball.put_me_back();
                    }
                    else
                    {
                        _ball.take_me();
                        if (view.ball_in_hand !== Settings.no_ball_in_hand)
                            view.itemAtIndex(view.ball_in_hand).put_me_back();
                        view.ball_in_hand = index;
                    }
                }
                else if (view.ball_in_hand !== Settings.no_ball_in_hand)
                {
                    view.itemAtIndex(view.ball_in_hand).put_me_back();
                    view.model.putBall(view.ball_in_hand, index);
                    view.ball_in_hand = Settings.no_ball_in_hand;
                }
        }
    }
    function fadeIn (){
        _fade_in.start();
    }
    function fadeOut (){
        _fade_out.start();
    }
    function take_me (){
        _ball.color = Settings.in_hand_color;
    }
    function put_me_back (){
        _ball.color = Settings.on_board_color;
    }

    NumberAnimation on opacity {
        id: _fade_in
        from: 0
        to: 1
        duration: 500
        running: false
        onStarted: {
            view.animation_started();
        }
        onFinished: {
            view.animation_finished();
        }
    }
    NumberAnimation on opacity {
        id: _fade_out
        from: 1
        to: 0
        duration: 500
        running: false
        onStarted: {
            view.animation_started();
        }
        onFinished: {
            view.animation_finished();
        }
    }
}
