import QtQuick 2.0

//! Component responsible for storing and drawing rectangle data.
Component{
    id: rectComponent
    Item{
        id: rectItem
        property var rect : Qt.rect(30, 30, 30, 30)
        property var fillColor : "red"
        property var lineColor : "black"
        property var lineWidth : 3

        function draw(context){
            context.beginPath();
            context.fillStyle = rectItem.fillColor
            context.rect(rect.x, rect.y, rect.width, rect.height)
            context.fill();
            context.stroke();
        }
    }
}