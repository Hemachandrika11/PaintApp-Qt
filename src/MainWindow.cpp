#include "MainWindow.h"
#include "canvas/Canvas.h"

#include <QMenuBar>
#include <QAction>
#include <QColorDialog>
#include <QSlider>
#include <QLabel>
#include <QToolBar>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PaintApp");
    resize(900, 600);

    // ===== Canvas =====
    canvas = new Canvas(this);
    setCentralWidget(canvas);

    // =========================
    // MENU BAR
    // =========================
    QMenu *fileMenu = menuBar()->addMenu("File");

    QAction *newAction = fileMenu->addAction("New");
    newAction->setShortcut(QKeySequence::New); // Ctrl+N
    connect(newAction, &QAction::triggered, canvas, &Canvas::clear);

    QAction *openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, [=]() {
        QString file = QFileDialog::getOpenFileName(
            this, "Open Image", QString(),
            "Images (*.png *.jpg *.jpeg *.bmp)"
        );
        if (!file.isEmpty())
            canvas->loadImage(file);
    });

    QAction *saveAction = fileMenu->addAction("Save");
    saveAction->setShortcut(QKeySequence::Save); // Ctrl+S
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveImage);

    // =========================
    // UNDO / REDO
    // =========================
    QAction *undoAct = new QAction("Undo", this);
    undoAct->setShortcut(QKeySequence::Undo); // Ctrl+Z
    connect(undoAct, &QAction::triggered, canvas, &Canvas::undo);

    QAction *redoAct = new QAction("Redo", this);
    redoAct->setShortcut(QKeySequence::Redo); // Ctrl+Y
    connect(redoAct, &QAction::triggered, canvas, &Canvas::redo);

    menuBar()->addAction(undoAct);
    menuBar()->addAction(redoAct);

    // =========================
    // TOOL BAR
    // =========================
    QToolBar *toolBar = addToolBar("Tools");

    // ---- Pen / Eraser ----
    QAction *penAction = toolBar->addAction("Pen");
    QAction *eraserAction = toolBar->addAction("Eraser");

    penAction->setCheckable(true);
    eraserAction->setCheckable(true);
    penAction->setChecked(true);

    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(penAction);
    toolGroup->addAction(eraserAction);

    connect(penAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Freehand);
    });

    connect(eraserAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(true);
        canvas->setShapeMode(Canvas::Freehand);
    });

    toolBar->addSeparator();

    // ---- SHAPES & TOOLS ----
    QAction *freeAction = toolBar->addAction("Free");
    QAction *lineAction = toolBar->addAction("Line");
    QAction *rectAction = toolBar->addAction("Rect");
    QAction *ellipseAction = toolBar->addAction("Ellipse");
    QAction *fillAction = toolBar->addAction("Fill");
    QAction *textAction = toolBar->addAction("Text");

    QList<QAction*> shapeActions = {
        freeAction, lineAction, rectAction, ellipseAction, fillAction, textAction
    };

    QActionGroup *shapeGroup = new QActionGroup(this);
    for (QAction *a : shapeActions) {
        a->setCheckable(true);
        shapeGroup->addAction(a);
    }
    freeAction->setChecked(true);

    connect(freeAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Freehand);
    });

    connect(lineAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Line);
    });

    connect(rectAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Rectangle);
    });

    connect(ellipseAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Ellipse);
    });

    // ✅ FIXED FILL TOOL
    connect(fillAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);
        canvas->setShapeMode(Canvas::Fill);
    });

    // ✅ TEXT TOOL
    connect(textAction, &QAction::triggered, this, [=]() {
        canvas->setEraser(false);

        bool ok;
        QString text = QInputDialog::getText(
            this, "Text Tool", "Enter text:",
            QLineEdit::Normal, "", &ok
        );

        if (ok && !text.isEmpty()) {
            canvas->setText(text);
            canvas->setShapeMode(Canvas::Text);
        }
    });

    toolBar->addSeparator();

    // ---- COLOR ----
    QAction *colorAction = toolBar->addAction("Color");
    connect(colorAction, &QAction::triggered, this, &MainWindow::pickColor);

    toolBar->addSeparator();

    // ---- BRUSH SIZE ----
    QLabel *label = new QLabel("Size:");
    toolBar->addWidget(label);

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(1, 30);
    slider->setValue(4);
    slider->setFixedWidth(120);
    toolBar->addWidget(slider);

    connect(slider, &QSlider::valueChanged,
            canvas, &Canvas::setBrushSize);
}

// =========================
// SLOTS
// =========================

void MainWindow::clearCanvas()
{
    canvas->clear();
}

void MainWindow::pickColor()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Select Color");
    if (color.isValid())
        canvas->setColor(color);
}

void MainWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Image", QString(),
        "PNG Image (*.png);;JPEG Image (*.jpg)"
    );

    if (fileName.isEmpty())
        return;

    if (!canvas->getImage().save(fileName)) {
        QMessageBox::warning(this, "Save Failed", "Could not save image.");
    }
}
