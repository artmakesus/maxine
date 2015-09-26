#include <MxWebView.hpp>

#include <QDebug>
#include <QPaintEvent>
#include <QWidget>

MxWebView::MxWebView(QWidget *parent) :
    QWebView(parent)
{
    mTimer = new QTimer(this);
    mTimer->setInterval(16);
    connect(mTimer, &QTimer::timeout, this, &MxWebView::onPaint);
    mTimer->start();
}
