/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 19.5.2011
*/

#include "mainwindow.h"
#include "warpwindow.h"
#include "imginterpolator.h"

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    warpWidget = new WarpWidget(QSize(5, 4), QSize(320, 240), this);
    createOptions();

    createActions();
    createMenus();

    centralLayout = new QGridLayout;
    centralLayout->addWidget(gridOptGroupBox, 0, 0);
    centralLayout->addWidget(optionsGroupBox, 0, 1);
    centralLayout->addWidget(showGroupBox, 0, 2);

    centralLayout->addLayout(warpHolderLayout, 1, 0, 1, 3);

    centralWidget->setLayout(centralLayout);

    setWindowTitle(tr("B-Spline Image Warping"));
}

void MainWindow::setGridSize()
{
    QSize newGridSize(columnsSpinB->value(), rowsSpinB->value());
    newGridSize = warpWidget->wWindow->setGridSize(newGridSize);
    rowsSpinB->setValue(newGridSize.height());
    columnsSpinB->setValue(newGridSize.width());
    warpWidget->clearScene();
    warpWidget->initScene();

    int maxPD = qMin(newGridSize.width(), newGridSize.height());
    bsPolyDegSpinB->setMaximum(maxPD + 1);
}

void MainWindow::resetGridSize()
{
    QSize gridSize = warpWidget->wWindow->getGridSize();
    warpWidget->wWindow->setGridSize(gridSize);
    rowsSpinB->setValue(gridSize.width());
    columnsSpinB->setValue(gridSize.height());
    warpWidget->clearScene();
    warpWidget->initScene();
}

void MainWindow::setGridPolyDegree(){
    warpWidget->wWindow->setBSPolynomDegree(bsPolyDegSpinB->value());

    rowsSpinB->setMinimum(bsPolyDegSpinB->value() - 1);
    columnsSpinB->setMinimum(bsPolyDegSpinB->value() - 1);
}

void MainWindow::setGridApproxPrecision(){
    warpWidget->wWindow->setBSApproxPrecision(bsPrecisionSpinB->value());
}

void MainWindow::setScaleComBoxText(int newScale){
    QString value;
    value.append(value.number(newScale));
    value.append('%');
    comBoxWarpScale->setItemText(0, value);
    comBoxWarpScale->setCurrentIndex(0);
}

void MainWindow::setWarpScaleFromComBox(int index){
    switch (index)
    {
    case 0:
        // hodnota sa nenastavuje, lebo 0 znamena ze sa mal
        // nastavit combobox na novu hodnotu
        break;
    case 1:
        if(warpScaleSpinB->value() != 25)
            warpScaleSpinB->setValue(25);
        break;
    case 2:
        if(warpScaleSpinB->value() != 50)
            warpScaleSpinB->setValue(50);
        break;
    case 3:
        if(warpScaleSpinB->value() != 75)
            warpScaleSpinB->setValue(75);
        break;
    case 4:
        if(warpScaleSpinB->value() != 100)
            warpScaleSpinB->setValue(100);
        break;
    case 5:
        if(warpScaleSpinB->value() != 150)
            warpScaleSpinB->setValue(150);
        break;
    case 6:
        if(warpScaleSpinB->value() != 200)
            warpScaleSpinB->setValue(200);
        break;
    case 7:
        if(warpScaleSpinB->value() != 300)
            warpScaleSpinB->setValue(300);
        break;
    default:
        if(warpScaleSpinB->value() != 100)
            warpScaleSpinB->setValue(100);
        break;
    }
}

void MainWindow::createOptions()
{
    // ----------------------------------------------------
    // Grid size Box

    btnLblRows = new QLabel(tr("Rows:"));
    btnLblColumns = new QLabel(tr("Columns:"));
    btnGridReset = new QPushButton(tr("Reset"));
    btnGridApply = new QPushButton(tr("Apply"));
    rowsSpinB = new QSpinBox();
    columnsSpinB = new QSpinBox();

    rowsSpinB->setMinimum(warpWidget->wWindow->getGridPolyDegree()-1);
    rowsSpinB->setValue(warpWidget->wWindow->getGridSize().height());

    columnsSpinB->setMinimum(warpWidget->wWindow->getGridPolyDegree()-1);
    columnsSpinB->setValue(warpWidget->wWindow->getGridSize().width());

    connect(btnGridReset, SIGNAL(clicked()), this, SLOT(resetGridSize()));
    connect(btnGridApply, SIGNAL(clicked()), this, SLOT(setGridSize()));

    gridOptLayout = new QGridLayout();
    gridOptLayout->addWidget(btnLblRows, 0, 0);
    gridOptLayout->addWidget(rowsSpinB, 0, 1);
    gridOptLayout->addWidget(btnLblColumns, 1, 0);
    gridOptLayout->addWidget(columnsSpinB, 1, 1);
    gridOptLayout->addWidget(btnGridReset, 2, 0);
    gridOptLayout->addWidget(btnGridApply, 2, 1);

    gridOptGroupBox = new QGroupBox(tr("Grid size"));
    gridOptGroupBox->setMaximumWidth(170);
    gridOptGroupBox->setLayout(gridOptLayout);

    // ----------------------------------------------------
    // Options Box

    btnLblInterpolate = new QLabel(tr("Interpolation:"));
    btnLblBSPrecision = new QLabel(tr("B-Spline Precision: [1-100]"));
    btnLblBSPolDeg = new QLabel(tr("B-S. polynom degree:"));

    comBoxInterpolate = new QComboBox();
    comBoxInterpolate->addItem(tr("Nearest"), ImgInterpolator::NEAREST);
    comBoxInterpolate->addItem(tr("Bilinear"), ImgInterpolator::BILINEAR);
    comBoxInterpolate->addItem(tr("Bicubic"), ImgInterpolator::BICUBIC);

    connect(comBoxInterpolate, SIGNAL(currentIndexChanged(int)), warpWidget->wWindow, SLOT(setInterpMethod(int)));

    bsPrecisionSpinB = new QSpinBox();

    bsPrecisionSpinB->setMaximum(100);
    bsPrecisionSpinB->setMinimum(1);
    bsPrecisionSpinB->setValue(warpWidget->wWindow->getGridApproxPrecision());

    connect(bsPrecisionSpinB, SIGNAL(valueChanged(int)), this, SLOT(setGridApproxPrecision()));

    bsPolyDegSpinB = new QSpinBox();

    QSize gSize = warpWidget->wWindow->getGridSize();
    int maxPD = qMin(gSize.width(), gSize.height());
    bsPolyDegSpinB->setMaximum(maxPD);
    bsPolyDegSpinB->setMinimum(2);
    bsPolyDegSpinB->setValue(warpWidget->wWindow->getGridPolyDegree());

    connect(bsPolyDegSpinB, SIGNAL(valueChanged(int)), this, SLOT(setGridPolyDegree()));

    optionsLayout = new QGridLayout();
    optionsLayout->addWidget(btnLblInterpolate, 0, 0);
    optionsLayout->addWidget(comBoxInterpolate, 0, 1);
    optionsLayout->addWidget(btnLblBSPrecision, 1, 0);
    optionsLayout->addWidget(bsPrecisionSpinB, 1, 1);
    optionsLayout->addWidget(btnLblBSPolDeg, 2, 0);
    optionsLayout->addWidget(bsPolyDegSpinB, 2, 1);

    optionsGroupBox = new QGroupBox(tr("Options"));
    optionsGroupBox->setMaximumWidth(220);
    optionsGroupBox->setLayout(optionsLayout);

    // ----------------------------------------------------
    // Show Box

    chbShowBSplines = new QCheckBox(tr("B-Splines"));
    //chbShowSrcImg = new QCheckBox(tr("Original Image"));
    chbShowCtrlPts = new QCheckBox(tr("Control Points"));
    chbShowCtrlPtsGrid = new QCheckBox(tr("Control Points grid"));
    chbShowCtrlPts->setChecked(true);
    chbShowBSplines->setChecked(true);

    connect(chbShowBSplines, SIGNAL(toggled(bool)), warpWidget->wWindow, SLOT(showBSplines(bool)));
    connect(chbShowCtrlPtsGrid, SIGNAL(toggled(bool)), warpWidget->wWindow, SLOT(showCPGrid(bool)));
    connect(chbShowCtrlPts, SIGNAL(toggled(bool)), warpWidget, SLOT(showCtrlPts(bool)));

    showLayout = new QGridLayout();
    showLayout->addWidget(chbShowBSplines, 0, 0);
    //showLayout->addWidget(chbShowSrcImg, 0, 1);
    showLayout->addWidget(chbShowCtrlPts, 1, 0);
    showLayout->addWidget(chbShowCtrlPtsGrid, 2, 0);

    showGroupBox = new QGroupBox(tr("Show"));
    showGroupBox->setLayout(showLayout);

    // ----------------------------------------------------
    // Canvas Box

    btnLblWarpScale = new QLabel(tr("Zoom:"));
    btnLblWarpScale->setAlignment(Qt::AlignRight);

    warpScaleSpinB = new QSpinBox();
    warpScaleSpinB->setMaximumWidth(70);

    warpScaleSpinB->setMaximum(300);
    warpScaleSpinB->setValue(100);
    warpScaleSpinB->setMinimum(25);

    comBoxWarpScale = new QComboBox();
    comBoxWarpScale->addItem(tr("100%"));
    comBoxWarpScale->addItem(tr("25%"));
    comBoxWarpScale->addItem(tr("50%"));
    comBoxWarpScale->addItem(tr("75%"));
    comBoxWarpScale->addItem(tr("100%"));
    comBoxWarpScale->addItem(tr("150%"));
    comBoxWarpScale->addItem(tr("200%"));
    comBoxWarpScale->addItem(tr("300\%"));

    comBoxWarpScale->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    comBoxWarpScale->setMaximumWidth(70);
    comBoxWarpScale->setMinimumWidth(60);

    sliderWarpScale = new QSlider(Qt::Horizontal);
    sliderWarpScale->setMinimum(25);
    sliderWarpScale->setValue(100);
    sliderWarpScale->setMaximum(300);
    sliderWarpScale->setMaximumWidth(150);
    sliderWarpScale->setMinimumWidth(100);

    connect(comBoxWarpScale, SIGNAL(currentIndexChanged(int)), this, SLOT(setWarpScaleFromComBox(int)));

    connect(warpScaleSpinB, SIGNAL(valueChanged(int)), sliderWarpScale, SLOT(setValue(int)));
    connect(sliderWarpScale, SIGNAL(valueChanged(int)), warpScaleSpinB, SLOT(setValue(int)));
    connect(sliderWarpScale, SIGNAL(valueChanged(int)), this, SLOT(setScaleComBoxText(int)));
    connect(sliderWarpScale, SIGNAL(valueChanged(int)), warpWidget, SLOT(setScale(int)));

    warpHolderLayout = new QGridLayout();
    warpScaleLayout = new QGridLayout();
    warpScaleLayout->addWidget(btnLblWarpScale, 0, 0, Qt::AlignRight);
    warpScaleLayout->addWidget(sliderWarpScale, 0, 1, Qt::AlignRight);
    warpScaleLayout->addWidget(warpScaleSpinB, 0, 2, Qt::AlignRight);
    warpScaleLayout->addWidget(comBoxWarpScale, 0, 3, Qt::AlignRight);
    warpHolderLayout->addLayout(warpScaleLayout, 0, 0, Qt::AlignRight);
    warpHolderLayout->addWidget(warpWidget, 1, 0);

    //    warpHolderGroupBox = new QGroupBox(tr("Canvas"));
    //    warpHolderGroupBox->setLayout(warpHolderLayout);

    // ----------------------------------------------------
}

void MainWindow::showOptions(bool checked)
{
    if(checked){
        gridOptGroupBox->show();
        optionsGroupBox->show();
        showGroupBox->show();
    }else{
        gridOptGroupBox->hide();
        optionsGroupBox->hide();
        showGroupBox->hide();
    }
    centralLayout->update();
}

void MainWindow::createActions()
{
    loadImageAct = new QAction(tr("&Load Image"), this);
    loadImageAct->setShortcut(tr("Ctrl+L"));
    connect(loadImageAct, SIGNAL(triggered()),
            this, SLOT(loadImage()));

    saveImageAct = new QAction(tr("&Save Image"), this);
    saveImageAct->setShortcut(tr("Ctrl+S"));
    connect(saveImageAct, SIGNAL(triggered()),
            this, SLOT(saveImage()));

    showOptionsAct = new QAction(tr("&Show options"), this);
    showOptionsAct->setShortcut(tr("Ctrl+O"));
    showOptionsAct->setCheckable(true);
    showOptionsAct->setChecked(true);
    connect(showOptionsAct, SIGNAL(toggled(bool)),
            this, SLOT(showOptions(bool)));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(loadImageAct);
    fileMenu->addAction(saveImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    windowMenu = menuBar()->addMenu(tr("&Options"));
    windowMenu->addAction(showOptionsAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::loadImage()
{
    QString fileName =
            QFileDialog::getOpenFileName( this,
                                         "Select image file to open:",
                                         QDir::currentPath(),
                                         "Images (*.jpg *.jpeg *.png *.bmp)" );
    if ( fileName != QString::null )
    {
        QPixmap pixMapTest;
        if(!pixMapTest.load(fileName)){
            QMessageBox::information(0, "Bacha!", "Unsupported image format!");
        }else{
            warpWidget->loadSrcImage(fileName);
        }
    }
}

void MainWindow::saveImage()
{
    QString fileName =
            QFileDialog::getSaveFileName( this,
                                         "Save Image:",
                                         QDir::currentPath() + "/untitled.jpg",
                                         "JPG (*.jpg *.jpeg);;PNG (*.png);;Bitmap (*.bmp)");
    if ( fileName != QString::null )
    {
        warpWidget->save(fileName);
    }
}

void MainWindow::about()
{
    QString aboutText = "Made by:\n\n  Miroslav Schery\n    xscher01@stud.fit.vutbr.cz\n  Zsolt Horváth\n    xhorva3@stud.fit.vutbr.cz      ";
    QMessageBox::about(this, tr("About Warp"),aboutText);
    //tr("<b>Image Warp</b><br />Made by:<br />Miroslav Schery<br />\tZsolt Horváth"));
}

MainWindow::~MainWindow()
{
    //uvolni pamet
}
