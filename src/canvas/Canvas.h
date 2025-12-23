#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QPoint>
#include <QStack>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    enum ShapeMode { Freehand, Line, Rectangle, Ellipse,Fill,Text };

    explicit Canvas(QWidget *parent = nullptr);

    void clear();
    void setColor(const QColor &color);
    void setBrushSize(int size);
    void setEraser(bool enabled);
    void setShapeMode(ShapeMode mode);
    void setFillShape(bool fill);

    void undo();
    void redo();
    void setText(const QString &text);
    void floodFill(const QPoint &pt, const QColor &newColor);

    bool loadImage(const QString &path);
    QImage getImage() const;
    QString textToDraw;
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    void saveState();

    QImage image;
    QImage tempImage;

    QPoint startPoint;
    QPoint lastPoint;

    bool drawing;
    bool eraserMode;
    bool fillShape;

    QColor penColor;
    int penWidth;
    ShapeMode shapeMode;

    QStack<QImage> undoStack;
    QStack<QImage> redoStack;
    QCursor penCursor;
    QCursor eraserCursor;
};

#endif
