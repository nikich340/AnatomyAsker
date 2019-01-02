#include <QtWidgets>
#include "AnatomyAsker.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    AnatomyAsker asker;
    QBrush brush;
    QPalette pallete = asker.palette();

    brush.setTexture(QPixmap(":/background.jpg"));
    pallete.setBrush(QPalette::Background, brush);
    asker.setPalette(pallete);

    asker.showMaximized();
    return app.exec();
}
