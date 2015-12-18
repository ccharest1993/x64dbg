#include "QGraphView.h"
#include <QTimeLine>
#include <iostream>

QGraphView::QGraphView(QWidget* parent)
    : QGraphicsView(parent)
{
    bAnimationFinished = false;
}

void QGraphView::wheelEvent(QWheelEvent* event)
{
    if(!(event->modifiers() & Qt::ControlModifier))
    {
        QGraphicsView::wheelEvent(event);
        return;
    }

    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    _numScheduledScalings += numSteps;
    if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
        _numScheduledScalings = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));

    QPointF mappedMousePos = this->mapToScene(event->pos().x(), event->pos().y());
    if(scene()->itemsBoundingRect().contains(mappedMousePos.x(), mappedMousePos.y()))
        centerOn(mappedMousePos.x(), mappedMousePos.y());
//    std::cout << mappedMousePos.x() << " - " << mappedMousePos.y() << std::endl;


    anim->start();
    bAnimationFinished = false;
}

void QGraphView::scalingTime(qreal x)
{
    Q_UNUSED(x)
    qreal factor = 1.0 + qreal(_numScheduledScalings) / 300.0;
    scale(factor, factor);
}

void QGraphView::animFinished()
{
    if (_numScheduledScalings > 0)
        _numScheduledScalings--;
    else
        _numScheduledScalings++;

    sender()->~QObject();
    bAnimationFinished = true;
}
