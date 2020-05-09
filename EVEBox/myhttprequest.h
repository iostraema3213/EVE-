#ifndef MYHTTPREQUEST_H
#define MYHTTPREQUEST_H

#include <QObject>
#include <QtNetwork>
#include <QQueue>
#include <QJsonValue>
#include <QMetaObject>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QTimer>
//#include "envenum.h"
#include "tools.h"

// TODO 网络ping查询不是跨平台接口

struct SendInfo {
    QObject         *caller;    // 调用者
    QByteArray      slotName;   // 槽函数名字
    QNetworkRequest request;    // 请求的requset
    QByteArray      data;       // 请求的数据
    QNetworkReply   *reply;

    int             retryCount;     // 重试次数
    qint64          curTime;        // 发送请求是当时的时间
    int             passwordSender; // 取货码的发送者
};

class MyHttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit MyHttpRequest(QObject *parent = 0);
    ~MyHttpRequest();
    int getUntreatedJob();

    /*!
     * \brief getInstance
     * \return 这个对象
     */
    static MyHttpRequest *getInstance();

    /*!
     * \brief startCheckNetTimer
     */
    void startCheckNetTimer();

    /*!
     * \brief getErrorInfoStr 通过错误号返回错误信息
     * \param errorNum 错误号
     * \return 返回错误号对应的错误信息
     */
    QString getErrorInfoStr(int errorNum);
signals:
    /*!
     * \brief networkAvailable
     */
    void networkAvailable();
public slots:
    /*!
     * \brief get
     * \param caller
     * \param slotName
     * \param request
     * \param data
     * \param retryCount
     * \return
     */
    int get(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data = "", int retryCount = 0);

    /*!
     * \brief post
     * \param caller
     * \param slotName
     * \param request
     * \param data
     * \param retryCount
     * \param passwordSender
     * \param curTime
     * \return 0：发送成功 1：caller或者slotname为空 2：不是槽函数
     */
    int post(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data,
             int retryCount = 20, int passwordSender = -1, qint64 curTime = Tools::getCurrentTimeStamp());

private slots:
    /*!
     * \brief sslErrorHandler 通过ssl访问后台新增槽函数
     * \param reply
     */
    void sslErrorHandler(QNetworkReply *reply, const QList<QSslError>);

    /*!
     * \brief finished
     * \param reply 请求的回复
     */
    void finished(QNetworkReply *reply);

    /*!
     * \brief sendErrorList
     */
    void sendErrorList();

private:
    static MyHttpRequest  *g_instance;
    QList<SendInfo>       sendInfoList;
    QList<SendInfo>       errorList;
    QNetworkAccessManager *manamger;
    QTimer                *checkNetworkAvailableTimer;
    QNetworkRequest       request;
    QProcess              *queryStatus_process;
    QProcess              *queryBakUrlStatus_process;

    /*!
     * \brief send
     * \param caller 调用者
     * \param slotName 槽函数名字
     * \param request request
     * \param data 请求需要的数据
     * \param retryCount 重发次数
     * \param curTime 发送请求的当前时间
     * \param passwordSender 密码输入源 1左触摸板，2右触摸板， 3主屏幕
     * \return 0：发送成功 1：caller或者slotname为空 2：不是槽函数
     */
    int send(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data,
             int retryCount = 20, int passwordSender = -1, qint64 curTime = Tools::getCurrentTimeStamp());

    class CGarbo //它的唯一工作就是在析构函数中删除MyHttpRequest的实例
    {
    public:
        ~CGarbo()
        {
            if (MyHttpRequest::g_instance) {
                delete MyHttpRequest::g_instance;
            }
        }
    };
    static CGarbo Garbo; //定义一个静态成员，程序结束时，系统会自动调用它的析构函数
};

#endif // MYHTTPREQUEST_H
