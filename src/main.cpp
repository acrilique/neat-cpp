#include "image_presenter.h"
#include "utils.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
  try {
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Neat");
    app.setApplicationName("Neat");
    app.setApplicationVersion("0.0.1");

    ImagePresenter window;
    window.show();

    utils::log_session("Application started");

    return app.exec();
  } catch (const std::exception &e) {
    qCritical() << "Fatal error:" << e.what();
    return 1;
  } catch (...) {
    qCritical() << "Unknown fatal error occurred";
    return 1;
  }
}