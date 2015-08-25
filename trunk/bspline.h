/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef BSPLINE_H
#define BSPLINE_H

#include <QVector3D>

#define DEF_PRECISION 0.5
#define DEF_POLY_DEGREE 4
#define DEF_APPROX_PRECISION 50 // [1-100]

class BSpline // : public BSpline
{
public:
    BSpline();
    BSpline(int control_pts_num,
            int approxResolution,
            int polynom_degree = DEF_POLY_DEGREE,
            int approxPrecision = DEF_APPROX_PRECISION,
            bool smooth_edges = false);

    ~BSpline();


    void setBSplineAttr(int control_pts_num,
                        int approxResolution,
                        int polynom_degree = DEF_POLY_DEGREE,
                        int approxPrecision = DEF_APPROX_PRECISION,
                        bool smooth_edges = false);

    QPointF *calculateCurve(int resolution);
    void setCtrlPoint(QVector3D point, int index);
    QVector3D getCtrlPoint(int index);
    int getCtrlPointNum() {return numExternCtrlPts;}

    QPointF getSplineAtPosX(int x, float precision, int maxIteration = 12);
    QPointF getSplineAtPosY(int y, float precision, int maxIteration = 12);

    // ODMAZAT!!!!!!!!!!!!!!!!
    QString double2string(double val);

    // nepouzivat - nedokoncene >>>>>>>>>>>>>>>>>>>>>>>>>>>
    QPointF *calcVertPrecise(int resolution);
    QPointF *calcVertPrecise();
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    QPointF *calcVertAprxArray(int resolution, int precision);
    QPointF *calcVertAprxArray();
    QPointF *calcHorizAprxArray(int resolution, int precision);
    QPointF *calcHorizAprxArray();
    QPointF *calcIntervalArray(int sampleNum);
    QPointF *calcIntervalArray();

    void setPolynomDegree(int degree);
    void setApproxPrecision(int precision);
    void setApproxResolution(int resolution);

    int getPolynomDegree();
    int getApproxPrecision();
    int getApproxResolution();
    int getIArrLength();

    QPointF getApproxSplineAt(int position);
    QPointF getIntervalSplineAt(int position);

private:
    int numInternCtrlPts;
    int numExternCtrlPts;
    int numCtrlPts;
    int polynomDegree;

    bool smoothEdges;

    QVector3D *controlPts;

    int iaPrecision;
    QPointF *intervalArray;
    bool iaUpToDate;
    int iaLength;

    QPointF *approxArray;
    bool aaUpToDate;
    int aaLength;

    float paPrecision;
    QPointF *preciseArray;
    bool paUpToDate;
    int paLength;

    void addCtrlPts();
    void bspline(int ctrlPtsNum, int polyDegree, QVector3D *ctrlPts,
                 QPointF *output, int num_output);
    void compute_intervals(float *knotVector, int ctrlPtsNum, int polyDegree);
    void compute_point(float *knotVector, int ctrlPtsNum, int polyDegree, double interval,
                       QVector3D *ctrlPts, QPointF *output);
    double blend(int k, int polyDegree, float *knotVector, double interval);

};

#endif // BSPLINE_H
