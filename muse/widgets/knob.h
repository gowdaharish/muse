#ifndef QWT_KNOB_H
#define QWT_KNOB_H

#include "sliderbase.h"
#include "sclif.h"
#include <QColor>
#include <QResizeEvent>
#include <QPaintEvent>


//---------------------------------------------------------
//   Knob
//---------------------------------------------------------

class Knob : public SliderBase, public ScaleIf
      {
  Q_OBJECT

   public:
      enum Symbol { Line, Dot };

   private:
      bool hasScale;

      int d_borderWidth;
      int d_shineWidth;
      int d_scaleDist;
      int d_maxScaleTicks;
      int d_newVal;
      int d_knobWidth;
      int d_dotWidth;

      Symbol d_symbol;
      double d_angle;
      double d_oldAngle;
      double d_totalAngle;
      double d_nTurns;

      double l_const;
      double l_slope;

      QRect  kRect;
      bool _faceColSel;
      QColor d_faceColor;
      QColor d_shinyColor;
      QColor d_rimColor;
      QColor d_curFaceColor;
      QColor d_altFaceColor;
      QColor d_markerColor;

      void recalcAngle();
      void valueChange();
      void rangeChange();
      void drawKnob(QPainter *p, const QRect &r);
      void drawMarker(QPainter *p, double arc, const QColor &c);

      void paintEvent(QPaintEvent *);
      void resizeEvent(QResizeEvent *e);
      double getValue(const QPoint &p);
      void getScrollMode( QPoint &p, const Qt::MouseButton &button, int &scrollMode, int &direction );
      void scaleChange()             { repaint(); }
      void fontChange(const QFont &) { repaint(); }

   public:
      Knob(QWidget* parent = 0, const char *name = 0);
      ~Knob() {}

      void setRange(double vmin, double vmax, double vstep = 0.0,
		    int pagesize = 1);
      void setKnobWidth(int w);
      void setTotalAngle (double angle);
      void setBorderWidth(int bw);
      void selectFaceColor(bool alt);
      bool selectedFaceColor() { return _faceColSel; }
      QColor faceColor() { return d_faceColor; }
      void setFaceColor(const QColor c);
      QColor altFaceColor() { return d_altFaceColor; }
      void setAltFaceColor(const QColor c);
      QColor markerColor() { return d_markerColor; }
      void setMarkerColor(const QColor c);
      };


#endif