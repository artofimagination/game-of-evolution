import QtQuick 2.1
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import QtQuick.Window 2.3

import backendGuiInterface 1.0

Rectangle {
    id: mainPage
    width: Screen.width
    height: Screen.height
    color: "darkgrey"

    property var backendEngine

    function updateCanvasSize(){
        simulatorCanvas.width = backendEngine.GetFrameSize().width * backendInterface.UiScale()
        simulatorCanvas.height = backendEngine.GetFrameSize().height * backendInterface.UiScale()
        simulatorCanvas.scaling = backendInterface.UiScale()
    }

    function updateUiData() {
        var imageData = backendEngine.GetImageFrameData()
        simulatorCanvas.peepsPositions = imageData.peepsPositions
        simulatorCanvas.peepsColors = imageData.peepsColors
        // simulatorCanvas.maxPopulation = imageData.maxPopulation
        
        generation.text = "Generation " + imageData.generation
        simulationStep.text = "Sim step " + imageData.simStep
    }

    QMLInterface{
        id: backendInterface

        Component.onCompleted: {
            mainPage.backendEngine = StartBackend()
            mainPage.backendEngine.ParametersUpdated.connect(mainPage.updateCanvasSize)
            simulatorCanvas.getUiData.connect(mainPage.updateUiData)
        }
    }

    Text {
        id: titleText
        text: "Game of Evolution"
        y: 30
        anchors.horizontalCenter: mainPage.horizontalCenter
        font.pointSize: 24; font.bold: true
    }

    Rectangle {
        id: simulatorWindow
        y: titleText.y + titleText.font.pointSize + 40
        width: mainPage.width; height: mainPage.height - y
        color: "lightgray"

        Text {
          id: generation
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.topMargin: 10
          text: "Generation"
          font.bold: true
          font.pointSize: 12
        }

        Text {
          id: simulationStep
          anchors.top: generation.bottom
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.topMargin: 10
          text: "Sim step"
          font.bold: true
          font.pointSize: 12
        }

        Canvas2DType{
            id: simulatorCanvas
            width: parent.width; height: parent.height      
        }
    }

    Button {
        id: quitButton
        text: "Quit"

        anchors.right: mainPage.right
        anchors.top: mainPage.top

        onClicked: {
            backendInterface.Quit()
            Qt.quit()
        }
    }
}