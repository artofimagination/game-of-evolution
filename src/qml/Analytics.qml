import QtQuick 2.0
import QtCharts 2.0
import QtQuick.Controls 2.12
import backendGuiInterface 1.0

//! This component is responsible for handling analytics
//! The user has a chartview to see the analytics and an analytics selector.
Item {
    id: chartRoot
    width: parent.width; height: parent.height
    property var name: ""
    property var analyticsIndex: 0
    
    //! Signals the new data retrieval in the chart widget
    signal updateChartData(var data)
    //! Signals a new data request
    signal requestUpdateData(var id)
    //! Signals new analytics type selection happened.
    signal selectAnalytics(var id)

    //! Sets the initial chart data. For example the selectable analytics types.
    function setData()
    {
        var names = backendInterface.GetAnalyticsNames()
        for(var i = 0; i < names.length; i++){
            analyticsTypes.model.append({name:names[i]})
        }
    }

    //! Clears the chart
    function removeAllSeries()
    {
        refreshTimer.running = false
        chartConnector.RemoveAllSeries()
        chartView.removeAllSeries()
    }

    //! Updates the margin in the chart
    function updateMargin(max, min)
    {
        chartConnector.ChangeMargin(max, min)
    }

    //! Adds a new chart series to the widget.
    function addInput(color, text, yAxis)
    {
        var newSeries = chartView.createSeries(ChartView.SeriesTypeLine, text)
        chartView.axisX(newSeries)
        newSeries.axisX = chartView.axes[0]
        if(yAxis === 0){
            newSeries.axisY = y1Axis
        }else{
            newSeries.axisYRight = y2Axis
        }
        newSeries.useOpenGL = true
        newSeries.color = color
        
        chartConnector.AddInput(newSeries)
        refreshTimer.running = true
    }

    Row {
        //! Charting view to display analytics data.
        ChartView {
            id: chartView
            width: 2 * chartRoot.width / 3; height: chartRoot.height
            title: chartRoot.name
            antialiasing: true

            axes: [
                ValueAxis {
                    id: xAxis
                    min: chartConnector.minX
                    max: chartConnector.maxX
                    tickCount: 10
                },
                ValueAxis {
                    id: y1Axis
                    min: 0
                    max: 0
                }
            ]

            //! C++ connector to manipulate series data in an effective way.
            ChartsConnector{
              id: chartConnector
            }

            //! Refresh timer to control the polling of new data. Frequency depends on the analytics type.
            Timer {
                id: refreshTimer
                interval: 1 / 30 * 1000 // 30 Hz
                running: false
                repeat: true
                onTriggered: {
                    chartConnector.UpdateSeries()
                    requestUpdateData(analyticsTypes.currentIndex)
                }
            }

            Component.onCompleted:{
              chartRoot.updateChartData.connect(chartConnector.UpdateHistory)
            }

            Component.onDestruction:{
              chartView.removeAllSeries()
            }
        }

        //! Analytics type selector
        Frame {
          Column{
              Text {
                  text: "Analytics"
                  font.bold: true
                  font.pointSize: 16
              }
              ComboBox {
                  id: analyticsTypes
                  model: ListModel {}
                  currentIndex: analyticsIndex
                  onActivated: {
                      // Change the refresh frequency based on the analytics.
                      switch(analyticsTypes.currentIndex)
                      {
                          case AnalyticsTypes.Survivors:
                              refreshTimer.interval = 2 * 1000 // 2 Hz
                              break;
                          default:
                              break;
                      }
                      selectAnalytics(analyticsTypes.currentIndex)
                  }
              }
          }
        }
    }
}
