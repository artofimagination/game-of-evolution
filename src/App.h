#pragma once

#include "Backend.h"

#include <QApplication>
#include <QtQuick/QQuickView>

/*! \class App
    \brief Main application for the game of evolution

    Responsible for creating a GUI thread.
*/
class App : public QApplication
{
    Q_OBJECT
public:
    /*! \fn EvoGameApp
        \brief Constructor
        \param[in] argc command line argument count
        \param[in] argv command line arguments
    */
    App(int argc, char *argv[]);

    /*! \fn Exec
        \brief Main entry point for the app.
        \return The exit state
    */
    int Exec();
    /*! \fn Quit
        \brief Terminates the application.
    */
    void Quit();

private:
    QQuickView  m_MainView; //!< Main UI window
};
