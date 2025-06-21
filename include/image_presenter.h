#ifndef IMAGE_PRESENTER_H
#define IMAGE_PRESENTER_H

#include "custom_graphics_view.h"
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <tuple>
#include <vector>

class ImagePresenter : public QMainWindow {
  Q_OBJECT

public:
  ImagePresenter();

private slots:
  void loadImage();
  void savePresentation();
  void loadRecentFile(int index);
  void setPresenterPoint();
  void nextPoint();
  void previousPoint();
  void resetView();
  void toggleFullscreen(); // New slot for fullscreen toggle

private:
  void setupUi();
  void setupVariables();
  void setupConnections();
  void loadLastState();
  void updateWindowTitle();
  void updateRecentFilesDropdown();
  void addToRecentFiles(const QString &filePath);
  void startHideTimer();
  void hideTopBarAndCursor();
  void showTopBarAndCursor();
  void onMouseMove();
  void toggleHiding(bool enable);
  void loadFile(const QString &filePath);
  void loadImageFile(const QString &filePath);
  void loadSvgFile(const QString &filePath);
  void loadPresentation(const QString &filePath);
  QByteArray encodeImageData();
  void updateStatusBar();
  void navigateToPoint(const std::tuple<QPointF, qreal> &point);
  void smoothNavigateToPoint(const QPointF &startCenter,
                             const QPointF &endCenter, qreal startZoom,
                             qreal endZoom);
  void navigateToNextPoint(int direction);
  QString transformNestedSvg(const QString &svgContent);

  QWidget *centralWidget;
  QVBoxLayout *layout;
  QWidget *topBar;
  QVBoxLayout *topLayout;
  QHBoxLayout *controlsLayout;
  QPushButton *loadButton;
  QPushButton *saveButton;
  QPushButton *fullscreenButton; // New button for fullscreen toggle
  QComboBox *recentFilesDropdown;
  QLabel *instructionsLabel;
  CustomGraphicsView *graphicsView;
  QGraphicsScene *scene;
  QStatusBar *statusBar;

  QGraphicsPixmapItem *imageItem;
  QGraphicsSvgItem *svgItem;
  QString svgContent; // Added to store the original SVG content

  std::vector<std::tuple<QPointF, qreal>> presentationPoints;
  int currentPointIndex;
  QString imageFormat;
  QString lastAccessedFolder;
  QStringList recentFiles;
  QString currentFilePath;

  QPropertyAnimation *animation;
  QTimer *hideTimer;

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // IMAGE_PRESENTER_H
