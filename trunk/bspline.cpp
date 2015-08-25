/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#include "bspline.h"
#include <stdlib.h>
#include <QtGui>

#include <iostream>
#include <sstream>
#include <string>


BSpline::BSpline()
{
    controlPts = NULL;
    smoothEdges = false;

    numInternCtrlPts = 0;
    numCtrlPts = 0;
    polynomDegree = 0;

    approxArray = NULL;
    preciseArray = NULL;

    iaPrecision = DEF_APPROX_PRECISION;
    paPrecision = DEF_PRECISION;
    aaUpToDate = false;
    paUpToDate = false;

    aaLength = 0;
    paLength = 0;
}

BSpline::BSpline(int control_pts_num,
                 int approxResolution,
                 int polynom_degree,
                 int approxPrecision,
                 bool smooth_edges)
{
    controlPts = NULL;
    smoothEdges = smooth_edges;

    approxArray = NULL;
    preciseArray = NULL;
    intervalArray = NULL;

    aaLength = 0;
    paLength = 0;
    iaLength = 0;

    paPrecision = DEF_PRECISION;
    aaUpToDate = false;
    paUpToDate = false;
    iaUpToDate = false;


    setBSplineAttr(control_pts_num, approxResolution,
                   polynom_degree, approxPrecision, smooth_edges);
}

void BSpline::setBSplineAttr(int control_pts_num,
                             int approxResolution,
                             int polynom_degree,
                             int approxPrecision,
                             bool smooth_edges)
{
    if(control_pts_num + 1 <= polynom_degree)
        QMessageBox::information(0, "Error", "Podmienka (control_pts_num + 1 > polynomDegree) nie je splnena!");

    numExternCtrlPts = control_pts_num;
    if(smooth_edges) control_pts_num += polynomDegree;
    QVector3D *newControlPts = new QVector3D[control_pts_num];
    if(controlPts){
        //QMessageBox::information(0, "Test", "realokacia a kopirovanie bodikov");
        if (smoothEdges){
            if (smooth_edges){
                if(control_pts_num < numCtrlPts){
                    memcpy(newControlPts, controlPts, control_pts_num * sizeof(QVector3D));
                }else{
                    memcpy(newControlPts, controlPts, numCtrlPts * sizeof(QVector3D));
                }
            }else{
                if(control_pts_num < numCtrlPts - polynomDegree){
                    memcpy(newControlPts, controlPts + polynomDegree - 2, control_pts_num * sizeof(QVector3D));
                }else{
                    memcpy(newControlPts, controlPts + polynomDegree - 2, numCtrlPts * sizeof(QVector3D));
                }
            }
        }else{
            if (smooth_edges){
                if(control_pts_num - polynomDegree < numCtrlPts){
                    memcpy(newControlPts + polynomDegree - 2, controlPts, control_pts_num - polynomDegree * sizeof(QVector3D));
                }else{
                    memcpy(newControlPts + polynomDegree - 2, controlPts, numCtrlPts * sizeof(QVector3D));
                }
            }else{
                if(control_pts_num < numCtrlPts){
                    memcpy(newControlPts, controlPts, control_pts_num * sizeof(QVector3D));
                }else{
                    memcpy(newControlPts, controlPts, numCtrlPts * sizeof(QVector3D));
                }
            }
        }
        delete controlPts;
    }

    controlPts = newControlPts;
    newControlPts = NULL;

    smoothEdges = smooth_edges;
    numCtrlPts = control_pts_num;
    numInternCtrlPts = control_pts_num - 1;

    setPolynomDegree(polynom_degree);
    setApproxPrecision(approxPrecision);
    setApproxResolution(approxResolution);

    //approxArray = new QPointF[aaLength];
}

int BSpline::getPolynomDegree(){
    return polynomDegree;
}

int BSpline::getApproxPrecision(){
    return iaPrecision;
}

int BSpline::getApproxResolution(){
    return aaLength;
}

int BSpline::getIArrLength(){
    return iaLength;
}

void BSpline::setApproxPrecision(int precision)
{
    if(precision < 1) precision = 1;
    if(precision > 100) precision = 100;
    if(precision == iaPrecision) return;
    iaPrecision = precision;
    iaUpToDate = aaUpToDate = false;

    iaLength = (int)((double)aaLength / 100.0) * iaPrecision;
    delete[] intervalArray;
    intervalArray = new QPointF[iaLength];
}

void BSpline::setApproxResolution(int resolution)
{
    if(resolution == aaLength) return;
    aaLength = resolution;
    aaUpToDate = false;
}

void BSpline::setPolynomDegree(int degree)
{
    if(numExternCtrlPts < degree){
        std::string result2 = "Nedodrzana podmienka [pocet kontr. bodov] >= [stupen polynomu]\n";
        char result[512] = {'\0'};
        sprintf(result, "[%d] < [%d]", numExternCtrlPts, polynomDegree);
        result2.append(result);
        QMessageBox::information(0, "Error", result2.c_str());
        exit(EXIT_FAILURE);
    }
    polynomDegree = degree;
    iaUpToDate = aaUpToDate = paUpToDate = false;
}

/////////////////////////////////////////////////////////////////////
// ODMAZAT !!!
QString BSpline::double2string(double val)
{
    std::stringstream ost;
    //    ost.setf(0,std::ios::floatfield);            // floatfield not set
    ost.precision(5);
    ost << val;
    std::string out = ost.str();
    return QString(out.c_str());
}
// ODMAZAT !!!
/////////////////////////////////////////////////////////////////////

BSpline::~BSpline()
{
    if(controlPts) delete controlPts;
    if(approxArray) delete approxArray;
    if(preciseArray) delete preciseArray;
    if(intervalArray) delete intervalArray;
}

void BSpline::addCtrlPts()
{
    if(smoothEdges){
        for(int i = 0; i < polynomDegree - 2; i++){
            controlPts[i] = controlPts[polynomDegree - 2];
            controlPts[numCtrlPts - i-1] = controlPts[numCtrlPts - (polynomDegree - 2) - 1];
        }
    }
}

void BSpline::setCtrlPoint(QVector3D point, int index)
{
    if(index >= numExternCtrlPts){
        QMessageBox::information(0, "Error", "BSpline::setCtrlPoint() Index mimo pola!");
        exit(EXIT_FAILURE);
    }
    if(smoothEdges){
        controlPts[index + polynomDegree - 2] = point;
        if(index == 0 || index == numExternCtrlPts - 1) addCtrlPts();
    }else{
        controlPts[index] = point;
    }

    iaUpToDate = aaUpToDate = paUpToDate = false;
}

QVector3D BSpline::getCtrlPoint(int index)
{
    if(index >= numExternCtrlPts){
        QMessageBox::information(0, "Error", "Index mimo pola! (getCtrlPoint())");
        exit(EXIT_FAILURE);
    }
    if(smoothEdges){
        return controlPts[index + polynomDegree - 2];
    }else{
        return controlPts[index];
    }
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [resolution]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcVertPrecise(int resolution)
{
    QMessageBox::information(0, "splineAtPosX()", "BSpline::calcVertPrecise Nie je doimplementovane!");
    exit(EXIT_FAILURE);

    paLength = resolution;
    paUpToDate = false;
    delete preciseArray;
    preciseArray = new QPointF[paLength];

    return calcVertPrecise();
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [paLength]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcVertPrecise()
{
    QMessageBox::information(0, "splineAtPosX()", "BSpline::calcVertPrecise Nie je doimplementovane!");
    exit(EXIT_FAILURE);
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [resolution]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcVertAprxArray(int resolution, int precision)
{
    if(iaPrecision == precision && aaLength == resolution && iaUpToDate && aaUpToDate){
        return approxArray;
        // netreba pocitat znovu, pole je uz vypocitane
    }

    if(precision < 1) precision = 1;
    if(precision > 100) precision = 100;

    aaLength = resolution;
    iaPrecision = precision;
    aaUpToDate = false;
    delete[] approxArray;
    approxArray = new QPointF[aaLength];

    iaLength = (int)((double)resolution / 100.0) * precision;
    iaUpToDate = false;
    delete[] intervalArray;
    intervalArray = new QPointF[iaLength];

    return calcVertAprxArray();
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [paLength]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcVertAprxArray()
{
    if(!approxArray){
        QMessageBox::information(0, "splineAtPosX()", "BSpline::calcHorizAprxArray() pole neinicializovane!");
        exit(EXIT_FAILURE);
    }
    if(aaUpToDate) return approxArray;

    if(!iaUpToDate) calcIntervalArray(iaLength);

    ////////////////////////////////////////////////
    int topEdge = round(intervalArray[0].y());
    int bottomEdge;

    approxArray[0] = intervalArray[0];

    for (int i = 0; i < iaLength - 1; i++)
    {
        bottomEdge = round(intervalArray[i+1].y());
        if(bottomEdge - topEdge > 1){
            float delta = intervalArray[i+1].x() - intervalArray[i].x();
            float increment = (bottomEdge - topEdge)?delta/(bottomEdge - topEdge) : 0;
            for (int j = 1; j < bottomEdge - topEdge; j++){
                approxArray[topEdge + j] = QPointF(intervalArray[i].x() + increment * j, topEdge + j);
            }
        }
        approxArray[bottomEdge] = intervalArray[i+1];
        topEdge = bottomEdge;
    }

    aaUpToDate = true;
    return approxArray;
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [resolution]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcHorizAprxArray(int resolution, int precision)
{
    if(iaPrecision == precision && aaLength == resolution && iaUpToDate && aaUpToDate){
        return approxArray;
        // netreba pocitat znovu, pole je uz vypocitane
    }

    if(precision < 1) precision = 1;
    if(precision > 100) precision = 100;

    aaLength = resolution;
    iaPrecision = precision;
    aaUpToDate = false;
    delete[] approxArray;
    approxArray = new QPointF[aaLength];


    iaLength = (int)((double)resolution / 100.0) * precision;
    iaUpToDate = false;
    delete[] intervalArray;
    intervalArray = new QPointF[iaLength];

    return calcHorizAprxArray();
}

// WARNING !!!
// use only if control points have first coordinate "y" in 0
// and last in [paLength]
// y-values HAVE to be in ascending order
QPointF *BSpline::calcHorizAprxArray()
{
    if(!approxArray){
        QMessageBox::information(0, "splineAtPosX()", "BSpline::calcHorizAprxArray() pole neinicializovane!");
        exit(EXIT_FAILURE);
    }
    if(aaUpToDate) return approxArray;

    if(!iaUpToDate) calcIntervalArray(iaLength);

    ////////////////////////////////////////////////
    int leftEdge = round(intervalArray[0].x());
    int rightEdge;

    approxArray[0] = intervalArray[0];

    for (int i = 0; i < iaLength - 1; i++)
    {
        rightEdge = round(intervalArray[i+1].x());

        if(rightEdge - leftEdge > 1){
            float delta = intervalArray[i+1].y() - intervalArray[i].y();
            float increment = (rightEdge - leftEdge)?delta/(rightEdge - leftEdge) : 0;
            for (int j = 1; j < rightEdge - leftEdge; j++){
                approxArray[leftEdge + j] = QPointF(leftEdge + j, intervalArray[i].y() + increment * j);
            }
        }
        approxArray[rightEdge] = intervalArray[i+1];
        leftEdge = rightEdge;
    }
    aaUpToDate = true;
    return approxArray;
}

QPointF *BSpline::calcIntervalArray(int sampleNum)
{
    if(sampleNum == iaLength && iaUpToDate) return intervalArray;

    iaLength = sampleNum;
    iaUpToDate = false;
    delete[] intervalArray;
    intervalArray = new QPointF[iaLength];

    return calcIntervalArray();
}

QPointF *BSpline::calcIntervalArray()
{
    if(!intervalArray){
        QMessageBox::information(0, "splineAtPosX()", "BSpline::calcIntervalArray() pole neinicializovane!");
        exit(EXIT_FAILURE);
    }
    if(iaUpToDate) return intervalArray;

    float *knotVector;
    double increment, interval;
    QPointF bsPoint;
    int outIndex;

    iaUpToDate = false;

    knotVector = new float[numInternCtrlPts + polynomDegree + 1];
    compute_intervals(knotVector, numInternCtrlPts, polynomDegree);

    increment=(double) 1 / (iaLength - 1);  // how much parameter goes up each time
    interval=0.0;

    for (outIndex = 0; outIndex < iaLength - 1; outIndex++)
    {
        compute_point(knotVector, numInternCtrlPts, polynomDegree, interval, controlPts, &bsPoint);
        intervalArray[outIndex].setX(bsPoint.x());
        intervalArray[outIndex].setY(bsPoint.y());
        interval=interval+increment;  // increment our parameter
    }
    intervalArray[iaLength-1].setX(controlPts[numInternCtrlPts].x());   // put in the last point
    intervalArray[iaLength-1].setY(controlPts[numInternCtrlPts].y());

    iaUpToDate = true;
    delete knotVector;
    return intervalArray;
}

QPointF BSpline::getApproxSplineAt(int position)
{
    if(!approxArray || position >= aaLength){
        QMessageBox::information(0, "splineAtPosX()", "BSpline::getApproxSplineAt() pole neinicializovane alebo index mimo pola!");
        exit(EXIT_FAILURE);
    }
    return approxArray[position];
}

QPointF BSpline::getIntervalSplineAt(int position)
{
    if(!intervalArray || position >= iaLength){
        QMessageBox::information(0, "splineAtPosX()", "BSpline::getIntervalSplineAt() pole neinicializovane alebo index mimo pola!");
        exit(EXIT_FAILURE);
    }
    return intervalArray[position];
}


///////////////////////////////////////////////////////////////////////////////
// STARE FUNKCIE - NEPOUZIVAJU SA >>>>>
QPointF *BSpline::calculateCurve(int resolution)
{
    QPointF *out_pts;
    out_pts = new QPointF[resolution];

    bspline(numInternCtrlPts, polynomDegree, controlPts, out_pts, resolution);
    return out_pts;
}

QPointF BSpline::getSplineAtPosX(int x, float precision, int maxIteration)
{
    float interval = 0.25;
    float intPosition = 0.5;
    float *knotVector;
    QPointF calcxyz;

    knotVector = new float[numInternCtrlPts + polynomDegree + 1];
    compute_intervals(knotVector, numInternCtrlPts, polynomDegree);

    while(true){
        compute_point(knotVector, numInternCtrlPts, polynomDegree, intPosition, controlPts, &calcxyz);

        if(calcxyz.x() > x){
            intPosition -= interval;
        }else{
            intPosition += interval;
        }
        interval /= 2;

        if(fabs(calcxyz.x() - x) < precision)
            break;
        if(maxIteration <= 0){
            break;
        }

        maxIteration--;
    }
    return calcxyz;
}

QPointF BSpline::getSplineAtPosY(int y, float precision, int maxIteration)
{
    float interval = 0.25;
    float intPosition = 0.5;
    float *knotVector;
    QPointF calcxyz;

    knotVector = new float[numInternCtrlPts + polynomDegree + 1];
    compute_intervals(knotVector, numInternCtrlPts, polynomDegree);

    while(true){
        compute_point(knotVector, numInternCtrlPts, polynomDegree, intPosition, controlPts, &calcxyz);

        if(calcxyz.y() > y){
            intPosition -= interval;
        }else{
            intPosition += interval;
        }
        interval /= 2;

        if(fabs(calcxyz.y() - y) < precision)
            break;
        if(maxIteration <= 0){
            break;
        }

        maxIteration--;
    }
    return calcxyz;
}


/////////////////////////////////////////////////////////////////////
//                      --- B-Spline ---
void BSpline::bspline(int ctrlPtsNum, int polyDegree, QVector3D *ctrlPts,
                      QPointF *output, int num_output)
{
    float *knotVector;
    double increment, interval;
    QPointF calcxyz;
    int output_index;

    knotVector = new float[ctrlPtsNum+polyDegree+1];
    compute_intervals(knotVector, ctrlPtsNum, polyDegree);

    increment=(double) 1 / (num_output-1);  // how much parameter goes up each time
    interval=0.0;

    std::string result2 = "test: ";
    char result[512] = {'\0'};
    for(int i = 0; i<=ctrlPtsNum+polyDegree ;i++){
        sprintf(result, "%f, ", knotVector[i]);
        result2.append(result);
    }

    for (output_index=0; output_index<num_output-1; output_index++)
    {
        compute_point(knotVector, ctrlPtsNum, polyDegree, interval, ctrlPts, &calcxyz);
        output[output_index].setX(calcxyz.x());
        output[output_index].setY(calcxyz.y());
        interval=interval+increment;  // increment our parameter
    }
    output[num_output-1].setX(ctrlPts[ctrlPtsNum].x());   // put in the last point
    output[num_output-1].setY(ctrlPts[ctrlPtsNum].y());

    delete knotVector;
}
// <<<<< STARE FUNKCIE - NEPOUZIVAJU SA
///////////////////////////////////////////////////////////////////////////////

void BSpline::compute_intervals(float *knotVector, int ctrlPtsNum, int polyDegree)   // figure out the knots
{
    int j;

    for (j=0; j<=ctrlPtsNum+polyDegree; j++)
    {
        if (j<polyDegree)
            knotVector[j] = 0;
        else
            if (j<=ctrlPtsNum) //  && (polyDegree<=j)
                knotVector[j] = (double)(j-polyDegree+1)/(ctrlPtsNum-polyDegree+2);
            else
                //if (j>ctrlPtsNum)
                knotVector[j] = 1;  // if n-t=-2 then we're screwed, everything goes to 0
    }
}

void BSpline::compute_point(float *knotVector, int ctrlPtsNum, int polyDegree, double interval,
                            QVector3D *ctrlPts, QPointF *output)
{
    int k;
    double temp;

    // initialize the variables that will hold our outputted point
    output->setX(0);
    output->setY(0);

    QString test;
    test.append("interval: ");
    test.append(double2string(interval));
    test.append("\n");
    for (k=0; k <= ctrlPtsNum; k++)
    {
        temp = blend(k, polyDegree, knotVector, interval);  // same blend is used for each dimension coordinate
        test.append(double2string(temp));
        test.append("\n");
        output->setX(output->x() + ctrlPts[k].x() * temp);
        output->setY(output->y() + ctrlPts[k].y() * temp);
    }
}

double BSpline::blend(int k, int polyDegree, float *knotVector, double interval)  // calculate the blending value
{
    double value;

    if (polyDegree==1)			// base case for the recursion
    {
        if ((knotVector[k]<=interval) && (interval<knotVector[k+1]))
            value=1;
        else
            value=0;
    }
    else
    {
        if ((knotVector[k+polyDegree-1]==knotVector[k]) && (knotVector[k+polyDegree]==knotVector[k+1]))  // check for divide by zero
            value = 0;
        else
            if (knotVector[k+polyDegree-1]==knotVector[k]) // if a term's denominator is zero,use just the other
                value = (knotVector[k+polyDegree] - interval) / (knotVector[k+polyDegree] - knotVector[k+1]) * blend(k+1, polyDegree-1, knotVector, interval);
            else
                if (knotVector[k+polyDegree]==knotVector[k+1])
                    value = (interval - knotVector[k]) / (knotVector[k+polyDegree-1] - knotVector[k]) * blend(k, polyDegree-1, knotVector, interval);
                else
                    value = (interval - knotVector[k]) / (knotVector[k+polyDegree-1] - knotVector[k]) * blend(k, polyDegree-1, knotVector, interval) +
                            (knotVector[k+polyDegree] - interval) / (knotVector[k+polyDegree] - knotVector[k+1]) * blend(k+1, polyDegree-1, knotVector, interval);
    }
    return value;
}
