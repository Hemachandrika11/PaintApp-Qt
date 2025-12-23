#include "Canvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <QStack>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent),
      drawing(false),
      eraserMode(false),
      fillShape(false),
      penColor(Qt::black),
      penWidth(4),
      shapeMode(Freehand)
{
    setMinimumSize(900, 600);
    setMouseTracking(true);

    image = QImage(900, 600, QImage::Format_RGB32);
    image.fill(Qt::white);

    penCursor = QCursor(Qt::CrossCursor);
    eraserCursor = QCursor(Qt::OpenHandCursor);
    setCursor(penCursor);
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

void Canvas::saveState()
{
    undoStack.push(image);
    redoStack.clear();
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    saveState();

    // 🔵 FILL TOOL
    if (shapeMode == Fill && !eraserMode) {
        floodFill(event->pos(), penColor);
        update();
        return;
    }

    // 🔤 TEXT TOOL
    if (shapeMode == Text && !eraserMode && !textToDraw.isEmpty()) {
        QPainter painter(&image);
        painter.setPen(QPen(penColor));
        painter.setFont(QFont("Arial", penWidth * 3));
        painter.drawText(event->pos(), textToDraw);
        update();
        return;
    }

    // ✏️ DRAWING START
    drawing = true;
    startPoint = lastPoint = event->pos();
    tempImage = image;
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!drawing || !(event->buttons() & Qt::LeftButton))
        return;

    QColor color = eraserMode ? Qt::white : penColor;
    QPoint endPoint = event->pos();

    // ================= FREEHAND / ERASER =================
    if (shapeMode == Freehand) {
        QPainter painter(&image);
        painter.setPen(QPen(color, penWidth,
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
        painter.drawLine(lastPoint, endPoint);
        lastPoint = endPoint;
        update();
        return;
    }

    // 🚫 Shapes disabled while erasing
    if (eraserMode)
        return;

    // ================= SHIFT CONSTRAINT =================
    if (event->modifiers() & Qt::ShiftModifier) {
        int dx = endPoint.x() - startPoint.x();
        int dy = endPoint.y() - startPoint.y();
        int size = qMax(qAbs(dx), qAbs(dy));
        endPoint = QPoint(
            startPoint.x() + (dx < 0 ? -size : size),
            startPoint.y() + (dy < 0 ? -size : size)
        );
    }

    // ================= SHAPE PREVIEW =================
    image = tempImage;
    QPainter painter(&image);

    painter.setPen(QPen(color, penWidth,
                        Qt::SolidLine,
                        Qt::RoundCap,
                        Qt::RoundJoin));

    painter.setBrush(fillShape ? color : Qt::NoBrush);

    QRect rect(startPoint, endPoint);

    switch (shapeMode) {
    case Line:
        painter.drawLine(startPoint, endPoint);
        break;
    case Rectangle:
        painter.drawRect(rect);
        break;
    case Ellipse:
        painter.drawEllipse(rect);
        break;
    default:
        break;
    }

    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent *)
{
    drawing = false;
}

void Canvas::undo()
{
    if (!undoStack.isEmpty()) {
        redoStack.push(image);
        image = undoStack.pop();
        update();
    }
}

void Canvas::redo()
{
    if (!redoStack.isEmpty()) {
        undoStack.push(image);
        image = redoStack.pop();
        update();
    }
}

void Canvas::clear()
{
    saveState();
    image.fill(Qt::white);
    update();
}

void Canvas::setColor(const QColor &color)
{
    penColor = color;
}

void Canvas::setBrushSize(int size)
{
    penWidth = size;
}

void Canvas::setEraser(bool enabled)
{
    eraserMode = enabled;

    if (eraserMode) {
        shapeMode = Freehand;      // force freehand
        setCursor(eraserCursor);
    } else {
        setCursor(penCursor);
    }
}


void Canvas::setShapeMode(ShapeMode mode)
{
    if (eraserMode)
        return;   // 🚫 eraser blocks other tools

    shapeMode = mode;
}


void Canvas::setFillShape(bool fill)
{
    fillShape = fill;
}

bool Canvas::loadImage(const QString &path)
{
    QImage loaded;
    if (!loaded.load(path))
        return false;

    saveState();
    image = loaded.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
    return true;
}

QImage Canvas::getImage() const
{
    return image;
}

// ================= FILL ALGORITHM =================
void Canvas::floodFill(const QPoint &pt, const QColor &newColor)
{
    if (!image.rect().contains(pt))
        return;

    QColor oldColor = image.pixelColor(pt);
    if (oldColor == newColor)
        return;

    QStack<QPoint> stack;
    stack.push(pt);

    while (!stack.isEmpty()) {
        QPoint p = stack.pop();
        if (!image.rect().contains(p)) continue;
        if (image.pixelColor(p) != oldColor) continue;

        image.setPixelColor(p, newColor);

        stack.push(QPoint(p.x() + 1, p.y()));
        stack.push(QPoint(p.x() - 1, p.y()));
        stack.push(QPoint(p.x(), p.y() + 1));
        stack.push(QPoint(p.x(), p.y() - 1));
    }
}

void Canvas::setText(const QString &text)
{
    textToDraw = text;
}
