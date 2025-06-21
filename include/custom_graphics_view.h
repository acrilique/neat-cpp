#ifndef CUSTOM_GRAPHICS_VIEW_H
#define CUSTOM_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QSize>
#include <QWheelEvent>

class CustomGraphicsView : public QGraphicsView {
  Q_OBJECT

public:
  explicit CustomGraphicsView(QWidget *parent = nullptr);
  void setInitialZoom();
  void setZoom(qreal zoom);
  qreal getZoom() const { return m_currentZoom; }
  void setOriginalImageSize(const QSize &size);
  QPointF mapToImageCoordinates(const QPointF &viewPoint) const;
  QPointF mapFromImageCoordinates(const QPointF &imagePoint) const;

signals:
  void mouseMoved();

protected:
  void wheelEvent(QWheelEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void setupGraphicsView();
  void zoomView(double factor);
  void updateScaling();

  double m_currentZoom;
  const double m_minZoom = 0.05;
  const double m_maxZoom = 100.0;
  const double m_zoomFactor = 1.15;
  QSize m_originalImageSize;
  QSize m_currentViewSize;
  QTransform m_currentTransform; // New member to store the current transform
};

#endif // CUSTOM_GRAPHICS_VIEW_H
