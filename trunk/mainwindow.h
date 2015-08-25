/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "warpWindow.h"
#include "warpWidget.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE
class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void showOptions(bool checked);
    void about();
    void loadImage();
    void saveImage();

    void setGridSize();
    void resetGridSize();
    void setGridPolyDegree();
    void setGridApproxPrecision();

    void setScaleComBoxText(int newScale);
    void setWarpScaleFromComBox(int index);

public:
    MainWindow();
    ~MainWindow();

private:
    QGridLayout *centralLayout;
    QWidget *centralWidget;

    QMenu *fileMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;

    QAction *showOptionsAct;
    QAction *aboutAct;
    QAction *exitAct;
    QAction *loadImageAct;
    QAction *saveImageAct;

    WarpWidget *warpWidget;

    void createOptions();

    void createActions();
    void createMenus();

    QGroupBox *optionsGroupBox;
    QGroupBox *gridOptGroupBox;
    QGroupBox *showGroupBox;
//    QGroupBox *warpHolderGroupBox;

    QGridLayout *allOptionsLayout;
    QGridLayout *optionsLayout;
    QGridLayout *gridOptLayout;
    QGridLayout *showLayout;
    QGridLayout *warpScaleLayout;
    QGridLayout *warpHolderLayout;

    QLabel *btnLblRows;
    QLabel *btnLblColumns;
    QLabel *btnLblInterpolate;
    QLabel *btnLblBSPrecision;
    QLabel *btnLblBSPolDeg;
    QLabel *btnLblWarpScale;

    QComboBox *comBoxInterpolate;
    QComboBox *comBoxWarpScale;

    QSlider *sliderWarpScale;

    QSpinBox *rowsSpinB;
    QSpinBox *columnsSpinB;
    QSpinBox *bsPolyDegSpinB;

    QSpinBox *bsPrecisionSpinB;

    QSpinBox *warpScaleSpinB;

    QPushButton *btnGridApply;
    QPushButton *btnGridReset;

    QCheckBox *chbShowBSplines;
    QCheckBox *chbShowSrcImg;
    QCheckBox *chbShowCtrlPts;
    QCheckBox *chbShowCtrlPtsGrid;
};

#endif // MAINWINDOW_H
