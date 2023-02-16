import QtQuick 2.12

Text {
    id: _score
    text : "0"
    property int intText: 0
    property int targetInt: 0
    property int changeStep: 1
    function set_score(new_score){
        intText = new_score;
        text = intText;
    }
    function change_score(new_score){
        if (new_score === intText)
            return;

        targetInt = new_score;
        if (targetInt > intText)
            changeStep = 1;
        else
            changeStep = -1;
        _anim_timer.start();
    }
    function anim_step(){
        set_score(intText+changeStep);
        if (intText == targetInt){
            _anim_timer.stop();
        }
    }
    Timer {
        id: _anim_timer
        interval: 50
        repeat: true
        onTriggered: _score.anim_step()
    }
}
