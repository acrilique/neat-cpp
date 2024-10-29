#include "custom_graphics_view.h"
#include <QApplication>
#include <QPainter>
#include <QScrollBar>
#include <QGraphicsScene>
#include <cmath>

CustomGraphicsView::CustomGraphicsView(QWidget* parent)
    : QGraphicsView(parent), m_currentZoom(1.0), m_originalImageSize(0, 0), m_currentViewSize(0, 0), m_currentTransform()
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
        m_currentTransform = QTransform();
        setTransform(m_currentTransform);
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

        m_currentTransform = QTransform::fromScale(scale, scale);
        setTransform(m_currentTransform);

        m_currentZoom = scale;
    }
}

void CustomGraphicsView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    m_currentViewSize = event->size();
    
    // Preserve the current center point
    QPointF centerPoint = mapToScene(viewport()->rect().center());
    
    // Update scaling while maintaining the current zoom level
    qreal scaleX = static_cast<qreal>(m_currentViewSize.width()) / m_originalImageSize.width();
    qreal scaleY = static_cast<qreal>(m_currentViewSize.height()) / m_originalImageSize.height();
    qreal scale = qMin(scaleX, scaleY);
    
    // Apply the current zoom to the new scale
    scale *= m_currentZoom;
    
    // Update the transform
    m_currentTransform = QTransform::fromScale(scale, scale);
    setTransform(m_currentTransform);
    
    // Center on the previous center point
    centerOn(centerPoint);
}

QPointF CustomGraphicsView::mapToImageCoordinates(const QPointF& viewPoint) const
{
    // Map the view point to scene coordinates
    QPointF scenePoint = mapToScene(viewPoint.toPoint());
    
    // Convert scene coordinates to image coordinates by applying the inverse of the current transform
    return scenePoint;
}

QPointF CustomGraphicsView::mapFromImageCoordinates(const QPointF& imagePoint) const
{
    // Convert image coordinates to scene coordinates
    QPointF scenePoint = imagePoint;
    
    // Map scene coordinates back to view coordinates
    return mapFromScene(scenePoint);
}

void CustomGraphicsView::setZoom(qreal zoom)
{
    zoom = qBound(m_minZoom, zoom, m_maxZoom);
    qreal factor = zoom / m_currentZoom;
    m_currentTransform.scale(factor, factor);
    setTransform(m_currentTransform);
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
    
    m_currentTransform.scale(actualFactor, actualFactor);
    setTransform(m_currentTransform);
    m_currentZoom = newZoom;
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    emit mouseMoved();
}
