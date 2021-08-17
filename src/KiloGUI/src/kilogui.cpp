#include <QApplication>
#include "kilowindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    /* app.setStyle("windowsvista"); */

    KiloWindow window;
    window.setGeometry(10,10,100,100); //goes on top left corner on unix otherwise
    
    window.raise();
    window.show();

    return app.exec();
}
