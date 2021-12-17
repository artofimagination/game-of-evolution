import QtQuick 2.0

Component{
    id: circleComponent
    Item{
        id: circleItem
        property var radius : 1
        property var position : Qt.point(30, 30)
        property var fillColor : "red"
        property var lineColor : "black"
        property var lineWidth : 3

        function draw(context){
            context.beginPath();
            context.fillStyle = circleItem.fillColor
            context.arc(circleItem.position.x, circleItem.position.y, circleItem.radius, 0.0, 2*Math.PI, true)
            context.fill();
            context.stroke();
        }
    }
}