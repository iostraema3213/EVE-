#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QWebEngineView>
#include "myhttprequest.h"

namespace Ui
{
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void searchCharacterId(QString name);
    void searchCharacterInfo(long long id);

private slots:
    void onSearchCharacterId(QNetworkReply *reply, bool isError, QByteArray data);
    void onSearchCharacterInfo(QNetworkReply *reply, bool isError, QByteArray data);

    void on_pushButton_clicked();
    void onAnchorClicked(const QUrl &link);

private:
    Ui::Widget *ui;
    MyHttpRequest *m_pHTTPTool;
    long long characterId;
    QWebEngineView *webView;
};

#endif // WIDGET_H
