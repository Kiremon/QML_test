import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0

Window {
    id: root
    width: 500
    height: 540
    visible: true
    title: qsTr("Color Lines")
    property real cell_side: width / game_core.boardSize()


    Rectangle {
        color : "gray"
        width: root.width
        height: root.height

        Score {
            id: _score
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            font.pointSize: root.width * 0.04
        }

        BackGround {
            anchors.top: _score.bottom
            width: root.width
            height: root.height
            cellHeight : root.cell_side
            cellWidth: cellHeight
        }

        Button {
            anchors.top: parent.top
            anchors.bottom: _score.bottom
            anchors.right: parent.right
            width: root.width * 0.2
            text: qsTr("Новая игра")
            onClicked: game_core.newGame();
        }

        Board {
            score: _score
            cellHeight : root.cell_side
            cellWidth: cellHeight
            anchors.top: _score.bottom
            width: root.width
            height: root.height
        }
    }
}
