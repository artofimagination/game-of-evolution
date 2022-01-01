import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import QtQuick.Window 2.3

import backendGuiInterface 1.0

Rectangle {
    id: mainPage
    width: Screen.width
    height: Screen.height
    color: "darkgrey"
    property var cTitleAreaWidth: 40

    property var backendEngine

    //! Update canvas from backend data. The update is triggered by the backend at the moment.
    function updateCanvasSize(){
        var scaledPeepRadius = simulatorCanvas.peepRadius * backendInterface.UiScale()
        var frameSize = backendInterface.GetFrameSize()
        canvasRectangle.width = frameSize.width * backendInterface.UiScale() + scaledPeepRadius * 4
        canvasRectangle.height = frameSize.height * backendInterface.UiScale() + scaledPeepRadius * 4
        simulatorCanvas.scaling = backendInterface.UiScale()
    }

    //! Sets challenge elements. Triggered by challenge selection and by the canvas refresh timer.
    //! The latter is needed, because the challenge details can change from one sim step to the other.
    function setChallengeItems(challengeId)
    {
        switch(challengeId)
        {
            case Challenge.Altruism:
                var setup = backendInterface.GetAltruismSetup()
                simulatorCanvas.createCircleChallengeItem(setup.altruismCenter, setup.altruismColor, setup.altruismRadius)
                simulatorCanvas.createCircleChallengeItem(setup.sacrificeCenter, setup.sacrificeColor, setup.sacrificeRadius)
                break
            case Challenge.AltruismSacrifice:
                var setup = backendInterface.GetAltruismSacrificeSetup()
                simulatorCanvas.createCircleChallengeItem(
                    setup.altruismSacrificeCenter,
                    setup.altruismSacrificeColor,
                    setup.altruismSacrificeRadius)
                break
            case Challenge.Circle:
                var setup = backendInterface.GetCircleSetup()
                simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
                break
            case Challenge.RightHalf:
                var setup = backendInterface.GetRightHalfSetup()
                simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
                break
            case Challenge.RightQuarter:
                var setup = backendInterface.GetRightQuarterSetup()
                simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
                break
            case Challenge.LeftEighth:
                var setup = backendInterface.GetLeftEighthSetup()
                simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
                break
            case Challenge.NeighborCount:
                var setup = backendInterface.GetNeighborCountSetup()
                simulatorCanvas.createBorderChallengeItems(setup.neighborCountBorders, 10, setup.neighborCountColor)
                break
            case Challenge.CenterWeighted:
                var setup = backendInterface.GetCenterWeightedSetup()
                simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
                break
            case Challenge.CenterUnweighted:
                var setup = backendInterface.GetCenterUnweightedSetup()
                simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
                break
            case Challenge.CenterSparsed:
                var setup = backendInterface.GetCenterSparsedSetup()
                simulatorCanvas.createCircleChallengeItem(
                    setup.centerSparsedCenter, 
                    setup.centerSparsedColor, 
                    setup.centerSparsedRadius)
                break
            case Challenge.Corner:
                var setup = backendInterface.GetCornerSetup()
                for(var i = 0; i < setup.cornerCenters.length; i++){
                    simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
                }
                break
            case Challenge.CornerWeighted:
                var setup = backendInterface.GetCornerSetup()
                for(var i = 0; i < setup.cornerCenters.length; i++){
                    simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
                }
                break
            case Challenge.RadioActiveWalls:
                var setup = backendInterface.GetRadioactiveWallSetup()
                simulatorCanvas.setCanvasGradient(setup.border, setup.radioactiveColor, setup.distance)
                break
            case Challenge.TouchAnyWall:
                var setup = backendInterface.GetTouchAnyWallSetup()
                simulatorCanvas.createBorderChallengeItems(setup.anyWallBorders, 10, setup.anyWallColor)
                break
            case Challenge.AgainstAnyWall:
                var setup = backendInterface.GetAgainstAnyWallSetup()
                simulatorCanvas.createBorderChallengeItems(setup.anyWallBorders, 10, setup.anyWallColor)
                break
            case Challenge.EastWestEighths:
                var setup = backendInterface.GetEastWestEighthsSetup()
                simulatorCanvas.createRectChallengeItem(setup.rectLeft, setup.doubleRectColor)
                simulatorCanvas.createRectChallengeItem(setup.rectRight, setup.doubleRectColor)
                break
            case Challenge.Pairs:
                var setup = backendInterface.GetPairsSetup()
                simulatorCanvas.createBorderChallengeItems(setup.pairsBorders, 10, setup.pairsColor)
                break
            case Challenge.CircularSequence:
                var setup = backendInterface.GetCircularSequenceSetup()
                for(var i = 0; i < setup.cornerCenters.length; i++){
                    simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
                }
                break
            default:
                break
        }
    }

    //! Sets the new challenge in the backend and restarts the analytics charts.
    function setChallenge(challengeId) {
        setChallengeItems(challengeId)
        analyticsTab.removeAllSeries()
    }

    //! Sets the new analytics chart series.
    function setAnalyticsType(analyticsType)
    {
        backendInterface.ClearAnalyticsProcessedCount();
        analyticsTab.removeAllSeries()
        switch (analyticsType ) {
            case AnalyticsTypes.Survivors:
                analyticsTab.addInput("red", "Survivors", 0)
                break
            case AnalyticsTypes.SurvivorToNextGen:
                analyticsTab.addInput("red", "Survivors to Next gen", 0)
                break
            case AnalyticsTypes.GeneticDiversity:
                analyticsTab.addInput("red", "GeneticDiversity", 0)
                break
            case AnalyticsTypes.AvgAge:
                analyticsTab.addInput("red", "Average Ages", 0)
                break
            case AnalyticsTypes.CompletedTasks:
                analyticsTab.addInput("red", "Task 1", 0)
                analyticsTab.addInput("green", "Task 2", 0)
                analyticsTab.addInput("blue", "Task 3", 0)
                analyticsTab.addInput("grey", "Task 4", 0)
                analyticsTab.addInput("yellow", "Task 5", 0)
                analyticsTab.addInput("purple", "Task 6", 0)
                analyticsTab.addInput("cyan", "Task 7", 0)
                analyticsTab.addInput("darkgreen", "Task 8", 0)
                break
            default:
                break
        }
    }

    //! Gets new analytics data from the backend. Will trigger a signal with the new data towards the chart.
    function updateAnalyticsData(analyticsType) {
        switch (analyticsType ) {
            case AnalyticsTypes.Survivors:
                var data = backendInterface.GetSurvivors()
                break
            case AnalyticsTypes.SurvivorToNextGen:
                var data = backendInterface.GetSurvivorsToNextGen()
                break
            case AnalyticsTypes.GeneticDiversity:
                var data = backendInterface.GetGeneticDiversity()
                break
            case AnalyticsTypes.AvgAge:
                var data = backendInterface.GetAvgAges()
                break
            case AnalyticsTypes.CompletedTasks:
                var data = backendInterface.GetCompletedChallengeTaskCounts()
                break
            default:
                break
        }
        analyticsTab.updateChartData(data)
    }

    //! Creates barrier UI elements.
    function setBarriers(barrierType)
    {
        if (barrierType == Barrier.ThreeFloatingIslands ||
            barrierType == Barrier.SpotsSpecified) {
            var setup = backendInterface.GetCircleBarriers()
            for(var i = 0; i < setup.circleBarriers.length; i++){
                simulatorCanvas.createCircleBarrierItem(setup.circleBarriers[i], setup.circleBarrierColor, setup.circleBarrierRadius)
            }
        } else if (barrierType == Barrier.VerticalBarConstantLoc ||
                   barrierType == Barrier.VerticalBarRandomLoc ||
                   barrierType == Barrier.FiveBlocksStaggered ||
                   barrierType == Barrier.HorizontalBarConstantLoc) {
            var setup = backendInterface.GetRectBarriers()
            for(var i = 0; i < setup.rectBarriers.length; i++){
                simulatorCanvas.createRectBarrierItem(setup.rectBarriers[i], setup.rectBarrierColor)
            }
        }
    }

    //! Sets the initial details and analytics data
    function setUI()
    {
        detailsTab.setData()
        analyticsTab.setData()
    }

    //! Updates dynamic UI elements when the canvas update is triggered.
    function updateUiData() {
        simulatorCanvas.clear();
        var challenge = backendInterface.GetChallengeId()
        var barrierType = backendInterface.GetBarrierType()
        var imageData = backendInterface.GetWorldData()
        simulatorCanvas.createPeeps(imageData.peepsPositions, imageData.peepsColors)
        setChallengeItems(challenge)
        setBarriers(barrierType)

        detailsTab.challengeIndex = challenge
        detailsTab.generationText = "Generation " + imageData.generation
        detailsTab.simStepText = "Sim step " + imageData.simStep
        detailsTab.maxPopulationText = "Max population " + imageData.maxPopulation
    }

    //! C++ QML/backend interface 
    QMLInterface{
        id: backendInterface

        Component.onCompleted: {
            mainPage.backendEngine = StartBackend()
            mainPage.backendEngine.ParametersUpdated.connect(mainPage.updateCanvasSize)
            detailsTab.selectChallenge.connect(mainPage.setChallenge)
            analyticsTab.selectAnalytics.connect(mainPage.setAnalyticsType)
            analyticsTab.requestUpdateData.connect(mainPage.updateAnalyticsData)
            simulatorCanvas.requestUiData.connect(mainPage.updateUiData)
        }
    }

    Text {
        id: titleText
        text: "Game of Evolution"
        y: 30
        anchors.horizontalCenter: mainPage.horizontalCenter
        font.pointSize: 24; font.bold: true
    }

    //! Main content window area
    Rectangle {
        id: simulatorWindow
        y: titleText.y + titleText.font.pointSize + cTitleAreaWidth
        width: mainPage.width; height: mainPage.height - y
        color: "lightgray"

        //! World canvas
        Frame {
            id: canvasRectangle
            anchors.left: parent.left

            Canvas2DType{
                id: simulatorCanvas
            }
        }

        //! Simulation start/stop/reset
        Frame {
            anchors.top: canvasRectangle.bottom
            Row {
                padding: 5.0
                spacing: 20
                Button {
                    id: resetButton
                    text: "Reset"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        analyticsTab.removeAllSeries()
                        backendInterface.ResetSim()
                    }
                }

                Button {
                    id: startButton
                    text: "Start"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        backendInterface.StartSim()
                    }
                }

                Button {
                    id: stopButton
                    text: "Stop"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        backendInterface.StopSim()
                    }
                }
            }
        }

        //! Details/Config/Analytics/NeuralNetwork tabs
        TabBar {
            id: detailsTabBar
            anchors.left: canvasRectangle.right
            anchors.top: simulatorWindow.top
            anchors.leftMargin: 20
            anchors.topMargin: 10
            width: mainPage.width - canvasRectangle.width

            TabButton {
                text: "Details"
            }
            TabButton {
                text: "Analytics"
            }

            TabButton {
                text: "Neural network"
            }
        }

        StackLayout {
            anchors.left: detailsTabBar.left
            anchors.top: detailsTabBar.bottom
            anchors.leftMargin: 20
            anchors.topMargin: 10
            id: detailsLayout

            currentIndex: detailsTabBar.currentIndex

            Details {
                id: detailsTab
            }

            Analytics {
                id: analyticsTab
            }

            Item {
                Text {
                  text: "TBD"
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex == 1) {
                    analyticsTab.width = mainPage.width - canvasRectangle.width - 50
                    analyticsTab.height = canvasRectangle.height
                }
            }
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

    Component.onCompleted: {
        mainPage.setUI()
    }
}