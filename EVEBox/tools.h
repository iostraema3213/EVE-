#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QNetworkAccessManager>

#ifdef COMPILE_FOR_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <jni.h>
#endif
#include <QTimer>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
//#include "envenum.h"

// TODO 检查网络和设置系统时间不是跨平台接口

//版本号
#define verison    2.10

class Tools : public QObject
{
    Q_OBJECT
public:
    Tools(QObject *parent = nullptr);
    ~Tools();

    static QString trimLeftZero(QString str);
    static QStringList trimRepeatQStringList(QStringList strList);
    static QString GetErrString(unsigned short code);
    static QString getExecutor(int executor);
    static QByteArray enCode(QString sourceCode);
    static QString getProgramStartTime();
    static QString getProgramStopTime();
    static int CheckLocalNet();
    static QString getCurrentDataTime();
    static qint64 getCurrentTimeStamp();
    static bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);
    static void setSysTime(qint64 timeStamp);
    static QByteArray generatePostString(QMap<QString, QString> postMap);

    static void sendWeiXinMessage(QString msg);
    static QString getVersionString();
    static QString getSIMNumberString();
    static QString getNetStateString();
    static QString getCpuRateString();
    static QString getMemoryRateString();
    static QString getStoreRateString();
    static QString getSignalStrengthString();
    static QString getDeviceID();
    static void startVideo(double videoHeight);
    static void stopVideo();
    static void backToHome();
    static void qtIsReady();
    static bool getNetworkState();
public slots:
    static void initSignalStrengthString();
private:
    static QNetworkAccessManager *networkAccessManager;
};

#endif // TOOLS_H
