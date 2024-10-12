#include "custom_graphics_view.h"
#include <QApplication>
#include <QPainter>
#include <QScrollBar>
#include <QGraphicsScene>
#include <cmath>

CustomGraphicsView::CustomGraphicsView(QWidget* parent)
    : QGraphicsView(parent), m_currentZoom(1.0), m_originalImageSize(0, 0), m_currentViewSize(0, 0)
{
    setupGraphicsView();
}

void CustomGraphicsView::setupGraphicsView()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
}

void CustomGraphicsView::setOriginalImageSize(const QSize& size)
{
    m_originalImageSize = size;
    m_currentViewSize = viewport()->size();
    updateScaling();
}

void CustomGraphicsView::setInitialZoom()
{
    if (scene() && !scene()->items().isEmpty()) {
        setTransform(QTransform());
        updateScaling();
        centerOn(scene()->sceneRect().center());
    }
}

void CustomGraphicsView::updateScaling()
{
    if (m_originalImageSize.isValid() && !m_currentViewSize.isEmpty()) {
        qreal scaleX = static_cast<qreal>(m_currentViewSize.width()) / m_originalImageSize.width();
        qreal scaleY = static_cast<qreal>(m_currentViewSize.height()) / m_originalImageSize.height();
        qreal scale = qMin(scaleX, scaleY);

        scale = qBound(m_minZoom, scale, m_maxZoom);

        QTransform transform;
        transform.scale(scale, scale);
        setTransform(transform);

        m_currentZoom = scale;
    }
}

void CustomGraphicsView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    m_currentViewSize = event->size();
    updateScaling();
}

QPointF CustomGraphicsView::mapToImageCoordinates(const QPointF& viewPoint) const
{
    QPointF scenePoint = mapToScene(viewPoint.toPoint());
    qreal scaleX = static_cast<qreal>(m_originalImageSize.width()) / sceneRect().width();
    qreal scaleY = static_cast<qreal>(m_originalImageSize.height()) / sceneRect().height();
    return QPointF(scenePoint.x() * scaleX, scenePoint.y() * scaleY);
}

QPointF CustomGraphicsView::mapFromImageCoordinates(const QPointF& imagePoint) const
{
    qreal scaleX = static_cast<qreal>(m_originalImageSize.width()) / sceneRect().width();
    qreal scaleY = static_cast<qreal>(m_originalImageSize.height()) / sceneRect().height();
    QPointF scenePoint(imagePoint.x() / scaleX, imagePoint.y() / scaleY);
    return mapFromScene(scenePoint);
}

void CustomGraphicsView::setZoom(qreal zoom)
{
    zoom = qBound(m_minZoom, zoom, m_maxZoom);
    qreal factor = zoom / m_currentZoom;
    scale(factor, factor);
    m_currentZoom = zoom;
}

void CustomGraphicsView::wheelEvent(QWheelEvent* event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        double delta = event->angleDelta().y() / 120.0;
        double factor = std::pow(m_zoomFactor, delta);
        zoomView(factor);
        event->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void CustomGraphicsView::zoomView(double factor)
{
    double newZoom = m_currentZoom * factor;
    newZoom = qBound(m_minZoom, newZoom, m_maxZoom);
    double actualFactor = newZoom / m_currentZoom;
    
    scale(actualFactor, actualFactor);
    m_currentZoom = newZoom;
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    emit mouseMoved();
}