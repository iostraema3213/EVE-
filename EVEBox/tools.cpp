#include "tools.h"
#ifdef COMPILE_FOR_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <jni.h>
#endif
#ifdef COMPILE_FOR_WINDOWS
#include <Windows.h>
#include <Wininet.h>
#endif

QNetworkAccessManager *Tools::networkAccessManager = NULL;

Tools::Tools(QObject *parent) :
    QObject(parent)
{
    networkAccessManager = new (std::nothrow)QNetworkAccessManager();
}

Tools::~Tools()
{
    if (networkAccessManager) {
        delete networkAccessManager;
        networkAccessManager = NULL;
    }
}

QString Tools::trimLeftZero(QString str)
{
    QString returnedStr;

    for (int i = 0; i < str.size(); i++) {
        if (str.at(i) != '0') {
            returnedStr = str.right(str.size() - i);
            break;
        }
    }
    return returnedStr;
}

QStringList Tools::trimRepeatQStringList(QStringList strList)
{
    QStringList returnedStrList;
    int         state;

    for (int i = 0; i < strList.size(); i++) {
        state = 0;

        for (int j = 0; j < returnedStrList.size(); j++) {
            if (returnedStrList.at(j) == strList.at(i)) {
                state = 1;
            }
        }

        if (state == 0) {
            returnedStrList.append(strList.at(i));
        }
    }
    return returnedStrList;
}


QString Tools::GetErrString(unsigned short code)
{
    if (code == 0) {
        return QString::fromUtf8("成功");
    }
    QString returnValue;
    for (int i = 0; i < 16; i++) {
        if (code & (1 << i)) {
            switch (i) {
            case 0:
                returnValue += QString::fromUtf8("指令执行失败 ");
                break;

            case 1:
                returnValue += QString::fromUtf8("BCC校验错误 ");
                break;

            case 2:
                returnValue += QString::fromUtf8("系统自检中 ");
                break;

            case 3:
                returnValue += QString::fromUtf8("箱门故障 ");
                break;

            case 4:
                returnValue += QString::fromUtf8("制热故障 ");
                break;

            case 5:
                returnValue += QString::fromUtf8("制冷故障 ");
                break;

            case 6:
                returnValue += QString::fromUtf8("底板温度传感器故障 ");
                break;

            case 7:
                returnValue += QString::fromUtf8("箱体温度传感器故障 ");
                break;

            case 8:
                returnValue += QString::fromUtf8("主机柜门故障 ");
                break;

            case 9:
                returnValue += QString::fromUtf8("快递柜门故障 ");
                break;

            case 10:
                returnValue += QString::fromUtf8("从机通讯故障 ");
                break;

            case 11:
                returnValue += QString::fromUtf8("从机离线 ");
                break;

            case 12:
                returnValue += QString::fromUtf8("预留 ");
                break;

            case 13:
                returnValue += QString::fromUtf8("预留 ");
                break;

            case 14:
                returnValue += QString::fromUtf8("预留 ");
                break;

            case 15:
                returnValue += QString::fromUtf8("其它系统状态故障 ");
                break;
            }
        }
    }
    return returnValue;
}


QString Tools::getExecutor(int executor)
{
    switch (executor) {
    case 0:
        return QString::fromUtf8("服务端");

    case 1:
        return QString::fromUtf8("入柜");

    case 2:
        return QString::fromUtf8("取货");

    case 3:
        return QString::fromUtf8("现售");

    case 4:
        return QString::fromUtf8("调试");

    case 5:
        return QString::fromUtf8("第三方预订");

    case 6:
        return QString::fromUtf8("闪灯内部发命令");

    default:
        return QString::fromUtf8("未知");
    }
}

int Tools::CheckLocalNet()
{
    int iState = 1;

#ifdef COMPILE_FOR_LINUX
    //iState = QAndroidJniObject::callStaticMethod<jint>("an/qt/autobooth/ExtendsQtWithJava", "networkState", "()I");
#endif
#ifdef COMPILE_FOR_ANDROID
    //iState = QAndroidJniObject::callStaticMethod<jint>("an/qt/autobooth/ExtendsQtWithJava", "networkState", "()I");
#endif
#ifdef COMPILE_FOR_WINDOWS
    unsigned long dwFlags = 0;
    //    iState = InternetGetConnectedState(&dwFlags, 0);
#endif
    return iState;
}

QByteArray Tools::enCode(QString sourceCode)
{
    QTextCodec *codecGB2312            = QTextCodec::codecForName("UTF-8");
    QByteArray byteArrayGB2312         = codecGB2312->fromUnicode(sourceCode);
    QByteArray byteArrayPercentEncoded = byteArrayGB2312.toPercentEncoding();

    return byteArrayPercentEncoded;
}
QString Tools::getProgramStartTime()
{
    QString programStartTime;

    programStartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return programStartTime;
}

QString Tools::getProgramStopTime()
{
    QString programStopTime;

    programStopTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return programStopTime;
}

QString Tools::getCurrentDataTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

qint64 Tools::getCurrentTimeStamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

bool Tools::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\", "/");
    if (sourceDir == toDir) {
        return true;
    }
    if (!QFile::exists(sourceDir)) {
        return false;
    }
    QDir *createfile = new QDir;
    bool exist       = createfile->exists(toDir);
    if (exist) {
        if (coverFileIfExist) {
            createfile->remove(toDir);
        }
    }
    if (!QFile::copy(sourceDir, toDir)) {
        return false;
    }
    return true;
}

void Tools::setSysTime(qint64 timeStamp)
{
    qint64    dwTime   = (timeStamp + 1) * 1000;//考虑网络延迟因素
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(dwTime, Qt::UTC, 8);
    QDate     date     = dateTime.date();
    QTime     time     = dateTime.time();

#ifdef COMPILE_FOR_LINUX
    //QAndroidJniObject::callStaticMethod<void>("an/qt/autobooth/ExtendsQtWithJava", "setSystemTime", "(IIIIII)V"
    //                                          , date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second());
#endif
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject::callStaticMethod<void>("an/qt/autobooth/ExtendsQtWithJava", "setSystemTime", "(IIIIII)V"
                                              , date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second());
#endif
#ifdef COMPILE_FOR_WINDOWS
    SYSTEMTIME st;
    GetSystemTime(&st);    // Win32 API 获取系统当前时间，并存入结构体st中
    st.wYear   = static_cast<unsigned short>(date.year());
    st.wMonth  = static_cast<unsigned short>(date.month());
    st.wDay    = static_cast<unsigned short>(date.day());
    st.wHour   = static_cast<unsigned short>(time.hour());
    st.wMinute = static_cast<unsigned short>(time.minute());
    st.wSecond = static_cast<unsigned short>(time.second());
    DWORD errNo = GetLastError();
    SetSystemTime(&st);    //Win32 API 设置系统时间
    errNo = GetLastError();
    if (errNo != 0) {
        qCritical() << QString::fromUtf8("校时错误：") << errNo;
    }
#endif
}
//组包
QByteArray Tools::generatePostString(QMap<QString, QString> postMap)
{
    QList<QString> keyList = postMap.uniqueKeys();
    QString        str;
    int            size = keyList.size();
    for (int i = 0; i < size; i++) {
        QString key   = keyList.at(i);
        QString value = postMap.value(key);
        if (i == keyList.size() - 1) {
            str = str + key + "=" + value;
        } else {
            str = str + key + "=" + value + "&";
        }
    }
    QUrl       url      = "/" + str;
    QByteArray tmp      = url.toEncoded();
    QByteArray postData = tmp.right(tmp.size() - 1);
    return postData;
}

void Tools::sendWeiXinMessage(QString msg)
{
    Q_UNUSED(msg)
}

QString Tools::getVersionString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getVersion",
                                                                              "()Ljava/lang/String;");
    qDebug() << stringArray.toString();
    return stringArray.toString();

#else
    return QString("1.60");
#endif
}

QString Tools::getSIMNumberString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getSIMInfo",
                                                                              "()Ljava/lang/String;");
    qDebug() << "+++++++++" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getNetStateString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getCurrentNetType",
                                                                              "()Ljava/lang/String;");
    qDebug() << "网络类型" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getCpuRateString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getProcessCpuRate",
                                                                              "()Ljava/lang/String;");

    qDebug() << "cpu使用率" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getMemoryRateString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getMemoryRate",
                                                                              "()Ljava/lang/String;");

    qDebug() << "内存的使用情况" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getStoreRateString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getStoreRate",
                                                                              "()Ljava/lang/String;");

    qDebug() << "存储使用情况" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getSignalStrengthString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "getSignalStrength",
                                                                              "()Ljava/lang/String;");

    qDebug() << "信号强度" << stringArray.toString();
    return stringArray.toString();

#else
    return QString("");
#endif
}

QString Tools::getDeviceID()
{
    return QString("asdfqwerasdfasdvzxcvasdf5648");
}

void Tools::initSignalStrengthString()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                              "initSignalStrengthString",
                                              "()V;");
#endif
}

void Tools::startVideo(double videoHeight)
{
#ifdef COMPILE_FOR_ANDROID
    jdouble la = QAndroidJniObject::callStaticMethod<jdouble>("com/jpgk/autobooth/ExtendsQtWithJava",
                                                              "startVideo",
                                                              "(D)D", videoHeight);
    Q_UNUSED(la);
#endif
}

void Tools::stopVideo()
{
#ifdef COMPILE_FOR_ANDROID
    jdouble la = QAndroidJniObject::callStaticMethod<jdouble>("com/jpgk/autobooth/ExtendsQtWithJava",
                                                              "stopVideo",
                                                              "()D");
    Q_UNUSED(la);
#endif
}

void Tools::backToHome()
{
#ifdef COMPILE_FOR_ANDROID
    qDebug() << "调用返回android主界面java方法";
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "toHome",
                                                                              "()Ljava/lang/String;");
    QString result = stringArray.toString();
#endif
#ifdef COMPILE_FOR_WINDOWS
    QString result = "";
    qDebug() << result;
#endif

}

void Tools::qtIsReady()
{
#ifdef COMPILE_FOR_ANDROID
    qDebug() << "设置qt函数可用";
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "qtIsReady",
                                                                              "()Ljava/lang/String;");

    qDebug() << stringArray.toString();
#endif
}

bool Tools::getNetworkState()
{
#ifdef COMPILE_FOR_ANDROID
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("com/jpgk/autobooth/ExtendsQtWithJava",
                                                                              "isNetworkConnected",
                                                                              "()Ljava/lang/String;");

    qDebug() << "物理网络状态：1，有网 0，没网。 结果是： " << stringArray.toString();
    return stringArray.toString() == "1" ? true : false;

#else
    return true;
#endif
}
