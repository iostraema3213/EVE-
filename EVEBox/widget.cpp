#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_pHTTPTool = MyHttpRequest::getInstance();
    characterId = 0;

    connect(ui->textBrowser, &QTextBrowser::anchorClicked, this, &Widget::onAnchorClicked);
    //    ui->textBrowser->setVisible(false);
    //    webView = new QWebEngineView(this);
    //    connect(webView, &QWebEngineView::loadStarted, this, [] {
    //        qDebug() << "++++++++";
    //    });
    //    connect(webView, &QWebEngineView::loadProgress, this, [](int progress) {
    //        qDebug() << "------:" + QString::number(progress);
    //    });
    //    webView->setGeometry(100, 120, 421, 600);
    //    webView->load(QUrl("https://www.baidu.com/"));
    //    webView->adjustSize();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::searchCharacterId(QString name)
{
    QUrl approveUrl;

    approveUrl =
        QString("https://esi.evepc.163.com/latest/search/?categories=character&datasource=serenity&language=zh&search=%1&strict=true").arg(
            name);
    QNetworkRequest request;

    request.setUrl(approveUrl);
    int iRetValue = m_pHTTPTool->post(this, SLOT(onSearchCharacterId(QNetworkReply *, bool, QByteArray)), request, "");
}

void Widget::searchCharacterInfo(long long id)
{
    QUrl approveUrl;

    approveUrl = QString("https://esi.evepc.163.com/latest/characters/%1/?datasource=serenity").arg(id);
    QNetworkRequest request;

    request.setUrl(approveUrl);
    int iRetValue = m_pHTTPTool->post(this, SLOT(onSearchCharacterInfo(QNetworkReply *, bool, QByteArray)), request, "");
}

void Widget::onSearchCharacterId(QNetworkReply *reply, bool isError, QByteArray data)
{
    qDebug() << data;
    QJsonParseError jsonError;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        QJsonArray array = doc.object()["character"].toArray();
        if (!array.isEmpty()) {
            characterId = array[0].toVariant().toLongLong();
            qDebug() << characterId;
            searchCharacterInfo(characterId);
        }
    }
}

void Widget::onSearchCharacterInfo(QNetworkReply *reply, bool isError, QByteArray data)
{
    qDebug() << data;
    QJsonParseError jsonError;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QString birthday = obj["birthday"].toString();
        QString name = obj["name"].toString();
        QString description = obj["description"].toString();
        double security_status = obj["security_status"].toDouble();
        ui->textBrowser->append(u8"名字：" + name);
        ui->textBrowser->append(u8"建号时间：" + birthday);
        ui->textBrowser->append(u8"安等：" + QString::number(security_status, 'g', 3));
        ui->textBrowser->append(u8"个人信息：" + description);
    }
}

void Widget::on_pushButton_clicked()
{
    ui->textBrowser->clear();
    searchCharacterId(ui->lineEdit->text());
}

void Widget::onAnchorClicked(const QUrl &link)
{

}
