#include "image_presenter.h"
#include "utils.h"
#include <QFileDialog>
#include <QImageReader>
#include <QBuffer>
#include <QGraphicsPixmapItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <cmath>

ImagePresenter::ImagePresenter() : QMainWindow()
{
    setupUi();
    setupVariables();
    setupConnections();
    loadLastState();
    updateWindowTitle();
}

void ImagePresenter::setupUi()
{
    setGeometry(100, 100, 800, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    layout = new QVBoxLayout(centralWidget);

    topBar = new QWidget();
    topLayout = new QVBoxLayout(topBar);
    controlsLayout = new QHBoxLayout();
    topLayout->addLayout(controlsLayout);

    loadButton = new QPushButton("Load Image/Presentation", this);
    saveButton = new QPushButton("Save Presentation", this);
    fullscreenButton = new QPushButton("Fullscreen", this); // New fullscreen button
    controlsLayout->addWidget(loadButton);
    controlsLayout->addWidget(saveButton);
    controlsLayout->addWidget(fullscreenButton);

    recentFilesDropdown = new QComboBox(this);
    recentFilesDropdown->setFixedWidth(200);
    recentFilesDropdown->addItem("Recent Files");
    controlsLayout->addWidget(recentFilesDropdown);

    instructionsLabel = new QLabel("S: Set point | Enter/N: Next point | Backspace/P: Previous point | R: Reset view | F: Toggle fullscreen | Mouse wheel: Scroll | Ctrl + Mouse wheel: Zoom | Mouse drag: Pan", this);
    topLayout->addWidget(instructionsLabel, 0, Qt::AlignCenter);

    layout->addWidget(topBar);

    graphicsView = new CustomGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    layout->addWidget(graphicsView);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    setMouseTracking(true);
    graphicsView->setMouseTracking(true);
}

void ImagePresenter::setupVariables()
{
    imageItem = nullptr;
    currentPointIndex = -1;
    lastAccessedFolder = "";
    currentFilePath = "";

    animation = new QPropertyAnimation(this, "");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
}

void ImagePresenter::setupConnections()
{
    connect(loadButton, &QPushButton::clicked, this, &ImagePresenter::loadImage);
    connect(saveButton, &QPushButton::clicked, this, &ImagePresenter::savePresentation);
    connect(fullscreenButton, &QPushButton::clicked, this, &ImagePresenter::toggleFullscreen); // Connect fullscreen button
    connect(recentFilesDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImagePresenter::loadRecentFile);
    connect(graphicsView, &CustomGraphicsView::mouseMoved, this, &ImagePresenter::onMouseMove);
    connect(hideTimer, &QTimer::timeout, this, &ImagePresenter::hideTopBarAndCursor);
}

void ImagePresenter::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        fullscreenButton->setText("Fullscreen");
    } else {
        showFullScreen();
        fullscreenButton->setText("Exit Fullscreen");
    }
}

void ImagePresenter::loadLastState()
{
    auto [lastOpenedFile, lastFolder, recentFilesList] = utils::load_state();
    lastAccessedFolder = QString::fromStdString(lastFolder);
    recentFiles = utils::stdVectorToQStringList(recentFilesList);
    updateRecentFilesDropdown();

    if (!lastOpenedFile.empty() && QFile::exists(QString::fromStdString(lastOpenedFile))) {
        loadFile(QString::fromStdString(lastOpenedFile));
        utils::log_session("Loaded last opened file: " + lastOpenedFile);
    }
}

void ImagePresenter::updateWindowTitle()
{
    if (!currentFilePath.isEmpty()) {
        setWindowTitle(currentFilePath + " - Neat");
    } else {
        setWindowTitle("Neat");
    }
}

void ImagePresenter::updateRecentFilesDropdown()
{
    recentFilesDropdown->clear();
    recentFilesDropdown->addItem("Recent Files");
    for (const auto& file : recentFiles) {
        recentFilesDropdown->addItem(QFileInfo(file).fileName(), file);
    }
}

void ImagePresenter::loadRecentFile(int index)
{
    if (index > 0) {
        QString filePath = recentFilesDropdown->itemData(index).toString();
        loadFile(filePath);
        recentFilesDropdown->setCurrentIndex(0);
    }
}

void ImagePresenter::addToRecentFiles(const QString& filePath)
{
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 7) {
        recentFiles.removeLast();
    }
    updateRecentFilesDropdown();
}

void ImagePresenter::startHideTimer()
{
    hideTimer->start(5000);
}

void ImagePresenter::hideTopBarAndCursor()
{
    topBar->hide();
    QApplication::setOverrideCursor(Qt::BlankCursor);
    graphicsView->viewport()->setCursor(Qt::BlankCursor);
}

void ImagePresenter::showTopBarAndCursor()
{
    topBar->show();
    QApplication::restoreOverrideCursor();
    graphicsView->viewport()->unsetCursor();
}

void ImagePresenter::onMouseMove()
{
    showTopBarAndCursor();
    startHideTimer();
}

void ImagePresenter::toggleHiding(bool enable)
{
    if (enable) {
        connect(hideTimer, &QTimer::timeout, this, &ImagePresenter::hideTopBarAndCursor);
        connect(graphicsView, &CustomGraphicsView::mouseMoved, this, &ImagePresenter::onMouseMove);
    } else {
        disconnect(hideTimer, &QTimer::timeout, this, &ImagePresenter::hideTopBarAndCursor);
        disconnect(graphicsView, &CustomGraphicsView::mouseMoved, this, &ImagePresenter::onMouseMove);
        showTopBarAndCursor();
    }
}

void ImagePresenter::loadImage()
{
    toggleHiding(false);
    QString initialDir = lastAccessedFolder.isEmpty() ? QDir::homePath() : lastAccessedFolder;
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image or Presentation", initialDir,
        "All Supported Files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.neatp);;"
        "Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;"
        "Neat Presentation (*.neatp)");
    toggleHiding(true);

    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void ImagePresenter::loadFile(const QString& filePath)
{
    lastAccessedFolder = QFileInfo(filePath).path();
    scene->clear();
    try {
        if (filePath.toLower().endsWith(".neatp")) {
            loadPresentation(filePath);
        } else {
            loadImageFile(filePath);
        }

        graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        graphicsView->setInitialZoom();
        updateStatusBar();
        qInfo() << "Image/Presentation loaded:" << filePath;
        addToRecentFiles(filePath);
        utils::save_state(filePath.toStdString(), lastAccessedFolder.toStdString(), utils::QStringListToStdVector(recentFiles));
        utils::log_session("Loaded file: " + filePath.toStdString());
        currentFilePath = filePath;
        updateWindowTitle();
    } catch (const std::exception& e) {
        qCritical() << "Error loading image/presentation:" << e.what();
        utils::log_session("Error loading file: " + filePath.toStdString() + ", Error: " + e.what());
    }
}

void ImagePresenter::loadImageFile(const QString& filePath)
{
    QImageReader reader(filePath);
    QImage image = reader.read();
    if (image.isNull()) {
        throw std::runtime_error("Failed to load image: " + filePath.toStdString());
    }
    QPixmap pixmap = QPixmap::fromImage(image);
    imageItem = scene->addPixmap(pixmap);
    scene->setSceneRect(pixmap.rect());
    imageFormat = reader.format();
    presentationPoints.clear();
    currentPointIndex = -1;
    graphicsView->setOriginalImageSize(pixmap.size());
    graphicsView->setInitialZoom();
}

void ImagePresenter::loadPresentation(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open presentation file: " + filePath.toStdString());
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject data = doc.object();

    QByteArray imageData = QByteArray::fromBase64(data["image_data"].toString().toUtf8());
    imageFormat = data["image_format"].toString();

    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData, imageFormat.toUtf8().constData())) {
        throw std::runtime_error("Failed to load image data from presentation file");
    }
    imageItem = scene->addPixmap(pixmap);
    scene->setSceneRect(pixmap.rect());
    graphicsView->setOriginalImageSize(pixmap.size());

    presentationPoints.clear();
    QJsonArray points = data["presentation_points"].toArray();
    for (const auto& pointJson : points) {
        QJsonObject pointObj = pointJson.toObject();
        QPointF point(pointObj["x"].toDouble(), pointObj["y"].toDouble());
        qreal zoom = pointObj["zoom"].toDouble();
        presentationPoints.emplace_back(point, zoom);
    }
    currentPointIndex = -1;
    graphicsView->setInitialZoom();
}

void ImagePresenter::savePresentation()
{
    if (!imageItem) {
        qWarning() << "No image loaded to save";
        return;
    }

    toggleHiding(false);
    QString initialDir = lastAccessedFolder.isEmpty() ? QDir::homePath() : lastAccessedFolder;
    QString filePath = QFileDialog::getSaveFileName(this, "Save Presentation", initialDir, "Neat Presentation (*.neatp)");
    toggleHiding(true);

    if (filePath.isEmpty()) {
        return;
    }

    lastAccessedFolder = QFileInfo(filePath).path();
    if (!filePath.toLower().endsWith(".neatp")) {
        filePath += ".neatp";
    }

    try {
        QByteArray imageData = encodeImageData();
        QJsonObject data;
        data["image_format"] = imageFormat;
        data["image_data"] = QString::fromUtf8(imageData.toBase64());

        QJsonArray pointsArray;
        for (const auto& [point, zoom] : presentationPoints) {
            QJsonObject pointObj;
            pointObj["x"] = point.x();
            pointObj["y"] = point.y();
            pointObj["zoom"] = zoom;
            pointsArray.append(pointObj);
        }
        data["presentation_points"] = pointsArray;

        QJsonDocument doc(data);
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            throw std::runtime_error("Failed to open file for writing: " + filePath.toStdString());
        }
        file.write(doc.toJson());

        qInfo() << "Presentation saved:" << filePath;
        addToRecentFiles(filePath);
        utils::save_state(filePath.toStdString(), lastAccessedFolder.toStdString(), utils::QStringListToStdVector(recentFiles));
        utils::log_session("Saved presentation: " + filePath.toStdString());
        currentFilePath = filePath;
        updateWindowTitle();
    } catch (const std::exception& e) {
        qCritical() << "Error saving presentation:" << e.what();
        utils::log_session("Error saving presentation: " + filePath.toStdString() + ", Error: " + e.what());
    }
}

QByteArray ImagePresenter::encodeImageData()
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    imageItem->pixmap().save(&buffer, imageFormat.toUtf8().constData());
    return imageData;
}

void ImagePresenter::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_S:
            setPresenterPoint();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_N:
            nextPoint();
            break;
        case Qt::Key_Backspace:
        case Qt::Key_P:
            previousPoint();
            break;
        case Qt::Key_R:
            resetView();
            break;
        case Qt::Key_F:
            toggleFullscreen();
            break;
        case Qt::Key_Escape:
            if (isFullScreen()) {
                showNormal();
                fullscreenButton->setText("Fullscreen");
            }
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}

void ImagePresenter::mouseMoveEvent(QMouseEvent* event)
{
    QMainWindow::mouseMoveEvent(event);
    onMouseMove();
}

void ImagePresenter::setPresenterPoint()
{
    if (imageItem) {
        QPointF center = graphicsView->mapToScene(graphicsView->viewport()->rect().center());
        QPointF imageCoords = graphicsView->mapToImageCoordinates(center);
        qreal zoom = graphicsView->getZoom();
        presentationPoints.emplace_back(imageCoords, zoom);
        currentPointIndex = static_cast<int>(presentationPoints.size()) - 1;
        updateStatusBar();
        qInfo() << "Presentation point set:" << imageCoords << "zoom:" << zoom;
        utils::log_session("Set presentation point: " + std::to_string(presentationPoints.size()));
    }
}

void ImagePresenter::nextPoint()
{
    navigateToNextPoint(1);
}

void ImagePresenter::previousPoint()
{
    navigateToNextPoint(-1);
}

void ImagePresenter::resetView()
{
    if (imageItem) {
        graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        updateStatusBar();
        qInfo() << "View reset";
        utils::log_session("View reset");
    }
}

void ImagePresenter::updateStatusBar()
{
    if (imageItem) {
        QString status = QString("Points: %1 | Current: %2")
            .arg(presentationPoints.size())
            .arg(currentPointIndex >= 0 ? currentPointIndex + 1 : 0);
        statusBar->showMessage(status);
    }
}

void ImagePresenter::navigateToPoint(const std::tuple<QPointF, qreal>& point)
{
    const auto& [imageCoords, zoom] = point;

    // Check if the imageCoords is a valid point
    if (imageCoords.isNull()) {
        return;
    }

    QPointF startCenter = graphicsView->mapToScene(graphicsView->viewport()->rect().center());
    QPointF endCenter = graphicsView->mapFromImageCoordinates(imageCoords);
    qreal startZoom = graphicsView->getZoom();
    smoothNavigateToPoint(startCenter, endCenter, startZoom, zoom);
    updateStatusBar();
}

void ImagePresenter::smoothNavigateToPoint(const QPointF& startCenter, const QPointF& endCenter, qreal startZoom, qreal endZoom)
{
    delete animation;
    animation = new QPropertyAnimation(this, "");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(animation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
        qreal t = value.toReal();
        QPointF currentCenter(
            (1 - t) * startCenter.x() + t * endCenter.x(),
            (1 - t) * startCenter.y() + t * endCenter.y()
        );
        qreal currentZoom = (1 - t) * startZoom + t * endZoom;
        
        graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
        graphicsView->setZoom(currentZoom);
        graphicsView->centerOn(currentCenter);
    });

    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void ImagePresenter::navigateToNextPoint(int direction)
{
    if (!presentationPoints.empty()) {
        currentPointIndex = (currentPointIndex + direction + static_cast<int>(presentationPoints.size())) % static_cast<int>(presentationPoints.size());
        navigateToPoint(presentationPoints[currentPointIndex]);
        utils::log_session("Navigated to point: " + std::to_string(currentPointIndex + 1));
    }
}