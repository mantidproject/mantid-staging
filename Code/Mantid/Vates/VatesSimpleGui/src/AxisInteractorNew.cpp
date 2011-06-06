#include "AxisInteractorNew.h"
#include "Indicator.h"
#include "ScalePicker.h"

#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <qwt_scale_widget.h>

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGridLayout>
#include <QList>
#include <QMouseEvent>
#include <QString>

#include <cmath>
#include <iostream>
AxisInteractorNew::AxisInteractorNew(QWidget *parent) : QWidget(parent)
{
  this->orientation = Qt::Vertical;
  this->scalePos = AxisInteractorNew::RightScale;

  this->setStyleSheet(QString::fromUtf8("QGraphicsView {background: transparent;}"));

  this->graphicsView = new QGraphicsView(this);
  this->graphicsView->setMouseTracking(true);
  this->graphicsView->setFrameShape(QFrame::NoFrame);
  this->graphicsView->setFrameShadow(QFrame::Plain);
  this->graphicsView->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

  this->gridLayout = new QGridLayout(this);
  this->scaleWidget = new QwtScaleWidget(this);

	this->scene = new QGraphicsScene(this);
	this->scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	this->isSceneGeomInit = false;

	//this->widgetLayout();

	this->graphicsView->setScene(this->scene);
	this->graphicsView->installEventFilter(this);

	this->engine = new QwtLinearScaleEngine;
	this->transform = new QwtScaleTransformation(QwtScaleTransformation::Linear);
	this->scalePicker = new ScalePicker(this->scaleWidget);
	QObject::connect(this->scalePicker, SIGNAL(makeIndicator(const QPoint &)),
			this, SLOT(createIndicator(const QPoint &)));
}

void AxisInteractorNew::widgetLayout()
{
  if (!this->gridLayout->isEmpty())
  {
    for (int i = 0; i < this->gridLayout->count(); ++i)
    {
      this->gridLayout->removeItem(this->gridLayout->itemAt(i));
    }
  }

  // All set for vertical orientation
  int scaleWidth = 75;
  int scaleHeight = 150;
  int gvWidth = 50;
  int gvHeight = 150;
  QSizePolicy policy(QSizePolicy::Fixed, QSizePolicy::Minimum);

  if (this->orientation == Qt::Vertical)
  {
    switch (this->scalePos)
    {
    case LeftScale:
    {
      this->scaleWidget->setAlignment(QwtScaleDraw::RightScale);
      this->gridLayout->addWidget(this->graphicsView, 0, 0, 1, 1);
      this->gridLayout->addWidget(this->scaleWidget, 0, 1, 1, 1);
      break;
    }
    case RightScale:
    default:
    {
      this->scaleWidget->setAlignment(QwtScaleDraw::LeftScale);
      this->gridLayout->addWidget(this->scaleWidget, 0, 0, 1, 1);
      this->gridLayout->addWidget(this->graphicsView, 0, 1, 1, 1);
      break;
    }
    }
  }
  else // Qt::Horizontal
  {
    qSwap(scaleWidth, scaleHeight);
    qSwap(gvWidth, gvHeight);
    policy.transpose();
    switch (this->scalePos)
    {
    case BottomScale:
    {
      this->scaleWidget->setAlignment(QwtScaleDraw::TopScale);
      this->gridLayout->addWidget(this->scaleWidget, 0, 0, 1, 1);
      this->gridLayout->addWidget(this->graphicsView, 1, 0, 1, 1);
      break;
    }
    case TopScale:
    default:
    {
      this->scaleWidget->setAlignment(QwtScaleDraw::BottomScale);
      this->gridLayout->addWidget(this->graphicsView, 0, 0, 1, 1);
      this->gridLayout->addWidget(this->scaleWidget, 1, 0, 1, 1);
      break;
    }
    }
  }
  this->scaleWidget->setMinimumSize(QSize(scaleWidth, scaleHeight));
  this->graphicsView->setMinimumSize(QSize(gvWidth, gvHeight));
  this->setSizePolicy(policy);
}

void AxisInteractorNew::setInformation(QString title, double min, double max)
{
	this->scaleWidget->setTitle(title);
	this->scaleWidget->setScaleDiv(this->transform,
			this->engine->divideScale(std::floor(min), std::ceil(max), 10, 0));
}

void AxisInteractorNew::createIndicator(const QPoint &point)
{
	QRect gv_rect = this->graphicsView->geometry();
	if (! this->isSceneGeomInit)
	{
		this->scene->setSceneRect(gv_rect);
		this->isSceneGeomInit = true;
	}
	Indicator *tri = new Indicator();
	tri->setPoints(point, gv_rect);
	this->scene->addItem(tri);
}

void AxisInteractorNew::setIndicatorName(const QString &name)
{
	QList<QGraphicsItem *> list = this->scene->items();
	for (int i = 0; i < list.count(); ++i)
	{
		QGraphicsItem *item = list.at(i);
		if (item->type() == IndicatorItemType)
		{
			if (item->toolTip().isEmpty())
			{
				// This must be the most recently added
				item->setToolTip(name);
			}
		}
	}
}

void AxisInteractorNew::selectIndicator(const QString &name)
{
	this->clearSelections();
	QList<QGraphicsItem *> list = this->scene->items();
	for (int i = 0; i < list.count(); ++i)
	{
		QGraphicsItem *item = list.at(i);
		if (item->type() == IndicatorItemType)
		{
			if (item->toolTip() == name)
			{
				item->setSelected(true);
			}
		}
	}
}

bool AxisInteractorNew::hasIndicator()
{
	QList<QGraphicsItem *> list = this->scene->selectedItems();
	if (list.count() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AxisInteractorNew::clearSelections()
{
	QList<QGraphicsItem *> list = this->scene->selectedItems();
	for (int i = 0; i < list.count(); ++i)
	{
		QGraphicsItem *item = list.at(i);
		if (item->type() == IndicatorItemType)
		{
			item->setSelected(false);
		}
	}
}

void AxisInteractorNew::updateIndicator(double value)
{
	QPoint *pos = this->scalePicker->getLocation(value);
	QList<QGraphicsItem *> list = this->scene->selectedItems();
	if (list.count() > 0)
	{
		Indicator *item = static_cast<Indicator *>(list.at(0));
		item->updatePos(*pos);
	}
}

bool AxisInteractorNew::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == this->graphicsView)
	{
		if (event->type() == QEvent::MouseButtonPress ||
				event->type() == QEvent::MouseButtonDblClick)
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			if (mouseEvent->button() == Qt::RightButton)
			{
				// Want to eat these so users don't add the indicators
				// via the QGraphicsView (Yum!)
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return AxisInteractorNew::eventFilter(obj, event);
	}
}

AxisInteractorNew::ScalePos AxisInteractorNew::scalePosition() const
{
  return this->scalePos;
}

void AxisInteractorNew::setOrientation(Qt::Orientation orient, ScalePos scalePos)
{
  this->scalePos = scalePos;
  this->orientation = orient;
  this->widgetLayout();
}

void AxisInteractorNew::setScalePosition(ScalePos scalePos)
{
    if ((scalePos == BottomScale) || (scalePos == TopScale))
    {
        this->setOrientation(Qt::Horizontal, scalePos);
    }
    else if ((scalePos == LeftScale) || (scalePos == RightScale))
    {
        this->setOrientation(Qt::Vertical, scalePos);
    }
}
