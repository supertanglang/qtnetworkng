#ifndef QTNG_HTTPD_H
#define QTNG_HTTPD_H
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "config.h"
#include "socket_server.h"
#include "http_utils.h"

QTNETWORKNG_NAMESPACE_BEGIN

class BaseHttpRequestHandler: public BaseRequestHandler, HeaderOperationMixin
{
public:
    BaseHttpRequestHandler(QSharedPointer<SocketLike> request, BaseStreamServer *server);
protected:
    virtual void handle();
    virtual void handleOneRequest();
    virtual bool parseRequest();
    virtual void doMethod();
    virtual bool sendError(HttpStatus status, const QString &longMessage);
    virtual bool sendResponse(HttpStatus status);
    virtual QString errorMessage(HttpStatus status, const QString &shortMessage, const QString &longMessage);
    virtual QString errorMessageContentType();
    virtual void logRequest(HttpStatus status, int bodySize);
    virtual void logError(HttpStatus status, const QString &shortMessage, const QString &longMessage);
    virtual QString serverName();
    virtual QString dateTimeString();
    void sendCommandLine(HttpStatus status, const QString &shortMessage);
    void sendHeader(const QByteArray &name, const QByteArray &value);
    bool endHeader();
protected:
    virtual void doGET();
    virtual void doPOST();
    virtual void doPUT();
    virtual void doDELETE();
    virtual void doPATCH();
    virtual void doHEAD();
    virtual void doOPTIONS();
    virtual void doTRACE();
    virtual void doCONNECT();
private:
    QByteArrayList headerCache;
protected:
    QString method;
    QString path;
    HttpVersion version;
    HttpVersion serverVersion;
    bool closeConnection;
};


class SimpleHttpRequestHandler: public BaseHttpRequestHandler
{
public:
    SimpleHttpRequestHandler(QSharedPointer<SocketLike> request, BaseStreamServer *server)
        :BaseHttpRequestHandler(request, server) {}
protected:
    virtual void doGET() override;
    virtual void doHEAD() override;
    virtual QSharedPointer<QFile> serveStaticFiles();
    virtual QSharedPointer<QFile> listDirectory(const QDir &dir, const QString &displayDir);
    void sendFile(QSharedPointer<QFile> f);
    QFileInfo translatePath(const QString &path);
};

QTNETWORKNG_NAMESPACE_END

#endif
