#ifndef MX_SERVER_HPP
#define MX_SERVER_HPP

#include <QObject>
#include <QJsonObject>

class MxScene;

class QTcpServer;
class QTcpSocket;

class MxServer : public QObject {
	Q_OBJECT
public:
	MxServer(MxScene *scene, QObject *parent = 0);

private:
	static const int DEFAULT_PORT;

	MxScene *mScene;
	QTcpServer *mServer;

	void handleRequest(QTcpSocket *client, const QString &type, const QJsonObject &data);

private slots:
	void newConnection();
	void clientDisconnected();
	void readyRead();
};

#endif
