#include <QtWidgets>
#include "AnatomyAsker.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    AnatomyAsker asker;

    //asker.setMaximumWidth(QApplication::desktop()->width());
    //asker.setMaximumHeight(QApplication::desktop()->height());
    asker.showMaximized();
    return app.exec();
}
