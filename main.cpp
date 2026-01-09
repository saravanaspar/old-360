#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Enable High DPI scaling (Optional, but good for modern screens)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // CRITICAL: Synthesize mouse events from touch if needed
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
