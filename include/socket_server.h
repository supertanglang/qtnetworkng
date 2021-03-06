#ifndef QTNG_SOCKET_SERVER_H
#define QTNG_SOCKET_SERVER_H

#include "socket_utils.h"
#include "coroutine_utils.h"
#ifndef QTNG_NO_CRYPTO
#include "ssl.h"
#endif

QTNETWORKNG_NAMESPACE_BEGIN

class BaseStreamServerPrivate;
class BaseStreamServer
{
public:
    BaseStreamServer(const QHostAddress &serverAddress, quint16 serverPort);
    virtual ~BaseStreamServer();
public:
    bool allowReuseAddress() const;
    void setAllowReuseAddress(bool b);
    int requestQueueSize() const;
    void setRequestQueueSize(int requestQueueSize);
    bool serveForever();
    bool start();
    void stop();
    virtual bool isSecure() const;
public:
    quint16 serverPort() const;
    QHostAddress serverAddress() const;
public:
    QSharedPointer<Event> started;
    QSharedPointer<Event> stopped;
protected:
    virtual bool serverBind();
    virtual bool serverActivate();
    virtual void serverClose();
    virtual bool serviceActions();
    virtual QSharedPointer<SocketLike> getRequest();
    virtual bool verifyRequest(QSharedPointer<SocketLike> request);
    virtual void processRequest(QSharedPointer<SocketLike> request);
    virtual void handleError(QSharedPointer<SocketLike> request);
    virtual void shutdownRequest(QSharedPointer<SocketLike> request);
    virtual void closeRequest(QSharedPointer<SocketLike> request);
protected:
    BaseStreamServerPrivate * const d_ptr;
    BaseStreamServer(BaseStreamServerPrivate *d);
private:
    Q_DECLARE_PRIVATE(BaseStreamServer)
};


template<typename RequestHandler>
class TcpServer: public BaseStreamServer
{
public:
    TcpServer(const QHostAddress &serverAddress, quint16 serverPort)
        :BaseStreamServer(serverAddress, serverPort) {}
protected:
    virtual void processRequest(QSharedPointer<SocketLike> request) override;
};


template<typename RequestHandler>
void TcpServer<RequestHandler>::processRequest(QSharedPointer<SocketLike> request)
{
    RequestHandler handler(request, this);
    handler.run();
}


#ifndef QTNG_NO_CRYPTO
class BaseSslStreamServerPrivate;
class BaseSslStreamServer: public BaseStreamServer
{
public:
    BaseSslStreamServer(const QHostAddress &serverAddress, quint16 serverPort, const SslConfiguration &configuration);
    BaseSslStreamServer(const QHostAddress &serverAddress, quint16 serverPort);
public:
    void setSslConfiguration(const SslConfiguration &configuration);
    SslConfiguration sslConfiguratino() const;
    virtual bool isSecure() const override;
protected:
    virtual QSharedPointer<SocketLike> getRequest() override;
private:
    Q_DECLARE_PRIVATE(BaseSslStreamServer)
};


template<typename RequestHandler>
class SslServer: public BaseSslStreamServer
{
public:
    SslServer(const QHostAddress &serverAddress, quint16 serverPort)
        :BaseSslStreamServer(serverAddress, serverPort) {}
    SslServer(const QHostAddress &serverAddress, quint16 serverPort, const SslConfiguration &configuration)
        :BaseSslStreamServer(serverAddress, serverPort, configuration) {}
protected:
    virtual void processRequest(QSharedPointer<SocketLike> request) override;
};


template<typename RequestHandler>
void SslServer<RequestHandler>::processRequest(QSharedPointer<SocketLike> request)
{
    RequestHandler handler(request, this);
    handler.run();
}

#endif

class BaseRequestHandler
{
public:
    BaseRequestHandler(QSharedPointer<SocketLike> request, BaseStreamServer *server);
    virtual ~BaseRequestHandler();
public:
    void run();
protected:
    virtual void setup();
    virtual void handle();
    virtual void finish();
protected:
    QSharedPointer<SocketLike> request;
    BaseStreamServer *server;
};


class Socks5RequestHandlerPrivate;
class Socks5RequestHandler: public qtng::BaseRequestHandler
{
public:
    Socks5RequestHandler(QSharedPointer<qtng::SocketLike> request, qtng::BaseStreamServer *server);
    virtual ~Socks5RequestHandler() override;
protected:
    virtual void doConnect(const QString &hostName, const QHostAddress &hostAddress, quint16 port);
    bool sendConnectReply(const QHostAddress &hostAddress, quint16 port);
    virtual void doFailed(const QString &hostName, const QHostAddress &hostAddress, quint16 port);
    bool sendFailedReply();
    virtual void log(const QString &hostName, const QHostAddress &hostAddress, quint16 port, bool success);
protected:
    virtual void handle() override;
//    virtual void logMessage(const QString &hostName, const QHostAddress &hostAddress, const quint16 port);
private:
    Socks5RequestHandlerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Socks5RequestHandler)
};

QTNETWORKNG_NAMESPACE_END

#endif
