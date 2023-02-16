pragma Singleton
import QtQuick 2.12

QtObject {
    readonly property variant colors: {
        0 : "#00000000",
        1 : "#ff0000",
        2 : "#00ff00",
        3 : "#0000ff",
        4 : "#00ffff"
    }
    readonly property int no_ball_in_hand : -1
    readonly property color in_hand_color : "#50000000"
    readonly property color on_board_color : "#00000000"
}
