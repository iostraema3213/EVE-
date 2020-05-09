#include "myhttprequest.h"
MyHttpRequest         *MyHttpRequest::g_instance = NULL;
MyHttpRequest::CGarbo MyHttpRequest::Garbo;

MyHttpRequest::MyHttpRequest(QObject *parent) :
    QObject(parent),
    sendInfoList(),
    errorList(),
    manamger(NULL),
    checkNetworkAvailableTimer(NULL)
{
    manamger = new QNetworkAccessManager(this);
    connect(manamger, SIGNAL(finished(QNetworkReply *)), SLOT(finished(QNetworkReply *)));
    connect(manamger, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)), this, SLOT(sslErrorHandler(QNetworkReply *,
                                                                                                       QList<QSslError>)));

    queryStatus_process = new QProcess(this);
    queryBakUrlStatus_process = new QProcess(this);

    checkNetworkAvailableTimer = new QTimer(this);
    checkNetworkAvailableTimer->setSingleShot(true);
    checkNetworkAvailableTimer->setInterval(1000);
}

MyHttpRequest::~MyHttpRequest()
{
    queryStatus_process->close();
    delete queryStatus_process;
    queryBakUrlStatus_process->close();
    delete queryBakUrlStatus_process;
    checkNetworkAvailableTimer->stop();
    delete checkNetworkAvailableTimer;
}

int MyHttpRequest::getUntreatedJob()
{
    return sendInfoList.size() + errorList.size();
}

MyHttpRequest *MyHttpRequest::getInstance()
{
    if (g_instance == NULL) {
        g_instance = new (std::nothrow)MyHttpRequest;
    }
    return g_instance;
}

int MyHttpRequest::send(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data, int retryCount,
                        int passwordSender, qint64 curTime)
{
    int iRet = -1;

    if (caller == NULL || slotName == "") {
        iRet = 1;
    } else {
        qDebug() << QString::fromUtf8("请求的url") << request.url().toString() << QString::fromUtf8("数据：") << data;
        QRegExp reg("[_A-Za-z][_A-Za-z0-9]*(.+)$");//提取函数名
        if (reg.indexIn(slotName) >= 0) {
            SendInfo temp;
            temp.caller         = caller;
            temp.slotName       = reg.cap(0).toLatin1();
            temp.request        = request;
            temp.data           = data;
            temp.retryCount     = retryCount;
            temp.curTime        = curTime;
            temp.passwordSender = passwordSender;

            if (data == "") {
                QNetworkReply *reply = manamger->get(request);
                temp.reply = reply;
            } else {
                QNetworkReply *reply = manamger->post(request, data);
                temp.reply = reply;
            }
            sendInfoList.append(temp);
            iRet = 0;
        } else {
            qDebug() << reg.errorString();
            qDebug() << "MyHttpRequest:" << slotName << QString::fromUtf8("不是槽函数");
            iRet = 2;
        }
    }
    return iRet;
}

QString MyHttpRequest::getErrorInfoStr(int errorNum)
{
    QString sRet;

    switch (errorNum) {
    case 0:
        sRet = QString::fromUtf8("发送成功");
        break;

    case 1:
        sRet = QString::fromUtf8("caller或者slotname为空");
        break;

    case 2:
        sRet = QString::fromUtf8("不是槽函数");
        break;

    default:
        break;
    }
    return sRet;
}


void MyHttpRequest::finished(QNetworkReply *reply)
{
    QObject    *obj;
    QByteArray slotName;
    SendInfo   info;
    QByteArray replyError;
    bool       isError;
    QByteArray data;

    if (reply == NULL) {
        isError    = true;
        replyError = QString::fromUtf8("收到reply为NULL").toUtf8();
    } else {
        data       = reply->readAll();
        isError    = !(reply->error() == QNetworkReply::NoError);
        replyError = reply->errorString().toUtf8();

        if (!isError) {
            //解析json格式数据，验证是否解析正确
            QJsonParseError jsonError;
            QJsonDocument::fromJson(data, &jsonError);
            for (int i = 0; i < sendInfoList.size(); i++) {
                if (sendInfoList.at(i).reply == reply) {
                    sendInfoList[i].retryCount--;

                    if (data.isEmpty() || (jsonError.error != QJsonParseError::NoError && (!sendInfoList.at(i).data.isEmpty()))) {
                        isError = true;
                        if (sendInfoList.at(i).retryCount <= 0) {
                            replyError = QString::fromUtf8("retry send request over your set limit").toUtf8();
                            info       = sendInfoList.at(i);
                            sendInfoList.removeAt(i);
                        } else {
                            if (jsonError.error != QJsonParseError::NoError) {
                                replyError = QString(jsonError.error).toUtf8();
                            } else {
                                replyError = QString::fromUtf8("服务器回复为空").toUtf8();
                            }
                            info = sendInfoList.at(i);
                            sendInfoList.removeAt(i);
                            errorList.append(info);
                            startCheckNetTimer();
                            reply->deleteLater();
                            return;
                        }
                    } else {
                        info = sendInfoList.at(i);
                        sendInfoList.removeAt(i);
                    }
                    emit networkAvailable();
                }
            }
        } else {
            for (int i = 0; i < sendInfoList.size(); i++) {
                if (sendInfoList.at(i).reply == reply) {
                    sendInfoList[i].retryCount--;
                    if (sendInfoList.at(i).retryCount <= 0) {
                        info       = sendInfoList.at(i);
                        sendInfoList.removeAt(i);
                    } else {
                        replyError = QString::fromUtf8("服务器回复出错").toUtf8();
                        info       = sendInfoList.at(i);
                        sendInfoList.removeAt(i);
                        errorList.append(info);
                        startCheckNetTimer();
                        reply->deleteLater();
                        return;
                    }
                }
            }
        }
    }

    obj      = info.caller;
    slotName = info.slotName;

    if (obj != NULL) {
        bool ok;//记录调用槽是否成功
        int  idx = obj->metaObject()->indexOfMethod(slotName.constData());
        if (idx < 0) {
            QByteArray norm = QMetaObject::normalizedSignature(slotName.constData());
            idx = obj->metaObject()->indexOfMethod(norm.constData());
        }

        int     parameterCount = obj->metaObject()->method(idx).parameterTypes().length();
        QRegExp reg("^[^(]+");
        reg.indexIn(slotName);
        slotName = reg.cap(0).toLatin1();
        if (parameterCount == 0) {
            ok = QMetaObject::invokeMethod(obj, slotName);
            reply->deleteLater();
        } else if (parameterCount == 1) {
            ok = QMetaObject::invokeMethod(obj, slotName, Q_ARG(QNetworkReply *, reply));
        } else if (parameterCount == 3) {
            ok = QMetaObject::invokeMethod(obj, slotName, Q_ARG(QNetworkReply *, reply),
                                           Q_ARG(bool, isError),
                                           Q_ARG(QByteArray, (isError ? replyError : data)));
        } else if (parameterCount == 4) {
            ok = QMetaObject::invokeMethod(obj, slotName, Q_ARG(QNetworkReply *, reply),
                                           Q_ARG(bool, isError),
                                           Q_ARG(QByteArray, (isError ? replyError : data)),
                                           Q_ARG(SendInfo, info));
        } else {
            ok = false;
            reply->deleteLater();
        }
        if (!ok) {
            qDebug() << QString::fromUtf8("MyHttpRequest:调用槽") + slotName + QString::fromUtf8("失败");
            reply->deleteLater();
        }
    }
}

void MyHttpRequest::sendErrorList()
{
    if (errorList.size() != 0) {
        qDebug() << QString::fromUtf8("重发列表非空, 重发所有数据");
    } else {
        return;
    }
    QNetworkReply *reply;
    SendInfo      temp;

    while (errorList.size() > 0) {
        qDebug() << QString::fromUtf8("请求的url：") << errorList.first().request.url().toString()
                 << QString::fromUtf8("请求的数据：") << errorList.first().data;
        temp.caller         = errorList.first().caller;
        temp.slotName       = errorList.first().slotName;
        temp.request        = errorList.first().request;
        temp.data           = errorList.first().data;
        temp.retryCount     = errorList.first().retryCount;
        temp.curTime        = errorList.first().curTime;
        temp.passwordSender = errorList.first().passwordSender;

        if (!temp.data.isEmpty()) {
            reply = manamger->post(temp.request, temp.data);
            errorList.first().reply = reply;
        } else {
            reply = manamger->get(errorList.first().request);
            errorList.first().reply = reply;
        }

        sendInfoList.append(errorList.first());
        errorList.removeFirst();
    }
}

void MyHttpRequest::startCheckNetTimer()
{
    checkNetworkAvailableTimer->start();
}

int MyHttpRequest::get(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data, int retryCount)
{
    return send(caller, slotName, request, data, retryCount);
}

int MyHttpRequest::post(QObject *caller, QByteArray slotName, QNetworkRequest request, QByteArray data, int retryCount,
                        int passwordSender, qint64 curTime)
{
    return send(caller, slotName, request, data, retryCount, passwordSender, curTime);
}

void MyHttpRequest::sslErrorHandler(QNetworkReply *reply, const QList<QSslError>)
{
    reply->ignoreSslErrors();
}
