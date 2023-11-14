#include "mainwindow.h"

#include <QApplication>

///
/// \brief main run the sprite editor app
/// \param argc argument count, used by Qt
/// \param argv arguments, used by Qt
/// \return status code (0 for success)
///
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::shared_ptr<Model> model(new Model());
    MainWindow w(model);
    w.show();
    return a.exec();
}
