import QtQuick 2.12

GridView {
    id: _view
    interactive : false
    model: game_core.boardCellsNum()
    delegate: Rectangle {
        border.color: "blue"
        border.width: 1
        color : "gray"
        width: _view.cellHeight
        height: width
    }
}
