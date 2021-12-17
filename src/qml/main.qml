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
    property var cTitleAreaWidth: 40

    property var backendEngine

    function updateCanvasSize(){
        var scaledPeepRadius = simulatorCanvas.peepRadius * backendInterface.UiScale()
        canvasRectangle.width = backendEngine.GetFrameSize().width * backendInterface.UiScale() + scaledPeepRadius
        canvasRectangle.height = backendEngine.GetFrameSize().height * backendInterface.UiScale() + scaledPeepRadius
        simulatorCanvas.scaling = backendInterface.UiScale()
    }

    function setChallengeItems(challengeId)
    {
        if (challengeId == Challenge.Altruism) {
            var setup = backendInterface.GetAltruismSetup()
            simulatorCanvas.createCircleChallengeItem(setup.altruismCenter, setup.altruismColor, setup.altruismRadius)
            simulatorCanvas.createCircleChallengeItem(setup.sacrificeCenter, setup.sacrificeColor, setup.sacrificeRadius)
        } else if (challengeId == Challenge.AltruismSacrifice) {
            var setup = backendInterface.GetAltruismSacrificeSetup()
            simulatorCanvas.createCircleChallengeItem(setup.altruismSacrificeCenter, setup.altruismSacrificeColor, setup.altruismSacrificeRadius)
        } else if (challengeId == Challenge.Circle) {
            var setup = backendInterface.GetCircleSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.RightHalf) {
            var setup = backendInterface.GetRightHalfSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.RightQuarter) {
            var setup = backendInterface.GetRightQuarterSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.LeftEighth) {
            var setup = backendInterface.GetLeftEighthSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.NeighborCount) {
            var setup = backendInterface.GetNeighborCountSetup()
            simulatorCanvas.createBorders(setup.neighborCountBorders, 10, setup.neighborCountColor)
        } else if (challengeId == Challenge.CenterWeighted) {
            var setup = backendInterface.GetCenterWeightedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.CenterUnweighted) {
            var setup = backendInterface.GetCenterUnweightedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.CenterSparsed) {
            var setup = backendInterface.GetCenterSparsedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.centerSparsedCenter, setup.centerSparsedColor, setup.centerSparsedRadius)
        } else if (challengeId == Challenge.Corner) {
            var setup = backendInterface.GetCornerSetup()
            for(var i = 0; i < setup.cornerCenters.length; i++){
                simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
            }
        } else if (challengeId == Challenge.CornerWeighted) {
            var setup = backendInterface.GetCornerSetup()
            for(var i = 0; i < setup.cornerCenters.length; i++){
                simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
            }
        } else if (challengeId == Challenge.RadioActiveWalls) {
            var setup = backendInterface.GetRadioactiveWallSetup()
            simulatorCanvas.setCanvasGradient(setup.border, setup.radioactiveColor, setup.distance)
        }
    }

    function updateUiData() {
        simulatorCanvas.clear();
        var challenge = backendInterface.GetChallengeId()
        var imageData = backendInterface.GetImageFrameData()
        simulatorCanvas.createPeeps(imageData.peepsPositions, imageData.peepsColors)
        setChallengeItems(challenge)

        generation.text = "Generation " + imageData.generation
        simulationStep.text = "Sim step " + imageData.simStep
        maxPopulation.text = "Max population " + imageData.maxPopulation 
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
        y: titleText.y + titleText.font.pointSize + cTitleAreaWidth
        width: mainPage.width; height: mainPage.height - y
        color: "lightgray"

        Frame {
            id: canvasRectangle
            anchors.left: parent.left

            Canvas2DType{
                id: simulatorCanvas
            }
        }

        Frame {
            id: detailsRectangle
            anchors.left: canvasRectangle.right
            anchors.top: simulatorWindow.top
            anchors.leftMargin: 50
            anchors.topMargin: 20
            Column {
                Text {
                    id: details
                    text: "Details:"
                    font.bold: true
                    font.pointSize: 14
                }
                Text {
                    id: generation
                    text: "Generation"
                    font.pointSize: 12
                }
                Text {
                    id: simulationStep
                    text: "Sim step "
                    font.pointSize: 12
                }
                Text {
                    id: maxPopulation
                    text: "Max population "
                    font.pointSize: 12
                }
            }
        }

        // GridLayout {
        //     id: mainAreaGrid
        //     anchors.fill: parent
        //     columns: 2
        //     rows: 1



        //     Rectangle {
        //         id: detailsElement
        //         Layout.fillHeight: true
        //         Layout.fillWidth: true
        //         Layout.column: 50
        //         Layout.columnSpan: 1
        //         Layout.row: 1
        //         Layout.rowSpan: 1
        //         color: simulatorWindow.color
        //         Rectangle {
        //             Text {
        //                 id: generation
        //                 text: "Generation"
        //                 font.bold: true
        //                 font.pointSize: 12
        //             }
        //         }

        //         Rectangle {
        //             Layout.fillHeight: true
        //             Layout.fillWidth: true
        //             Layout.column: 50
        //             Layout.columnSpan: 1
        //             Layout.row: 2
        //             Layout.rowSpan: 1
        //             color: simulatorWindow.color

        //             Text {
        //                 id: simulationStep
        //                 text: "Sim step"
        //                 font.bold: true
        //                 font.pointSize: 12
        //             }
        //         }
        //     }
        //}
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