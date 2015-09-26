#ifndef MX_WEBVIEW_HPP
#define MX_WEBVIEW_HPP

#include <QTimer>
#include <QWebView>

class QWidget;
class QPaintEvent;

class MxWebView : public QWebView {
    Q_OBJECT
public:
    MxWebView(QWidget *parent = 0);

signals:
    void onPaint();

private:
    QTimer *mTimer;
};

#endif
