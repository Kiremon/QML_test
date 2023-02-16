import QtQuick 2.12
import Settings 1.0

Rectangle {
    property int ball_type
    property real cell_side
    color: Settings.on_board_color
    width: cell_side
    height: width
    Rectangle {
        anchors.centerIn: parent
        color : Settings.colors[parent.ball_type]
        width: parent.width * 0.9
        height: width
        radius: width
    }
}
