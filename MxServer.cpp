#include <MxServer.hpp>
#include <MxScene.hpp>
#include <MxSceneItem.hpp>

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const int MxServer::DEFAULT_PORT = 4444;

MxServer::MxServer(MxScene *scene, QObject *parent) :
	QObject(parent),
	mScene(scene),
	mServer(new QTcpServer(this))
{
	mServer->listen(QHostAddress::LocalHost, DEFAULT_PORT);
	connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	printf("%s%d\n", "Listening at port ", DEFAULT_PORT);
}

void MxServer::handleRequest(QTcpSocket *client, const QString &type, const QJsonObject &data)
{
	if (type == "invalidateSharedTexture") {
		int id = data["id"].toInt();
		mScene->invalidateSharedTexture(id);
		printf("Received command: %s\n", "invalidateSharedTexture");
	} else if (type == "items") {
		QJsonArray array;
		auto items = mScene->items();
		for (auto i = 0; i < items.size(); i++) {
			auto item = static_cast<MxSceneItem *>(items[i]);
			QJsonValue value(item->id());
			array << value;
		}
		QJsonDocument document(array);
		client->write(document.toJson());
		printf("Received command: %s\n", "items");
	} else if (type == "createSharedTexture") {
		int id = data["id"].toInt();
		int width = data["width"].toInt();
		int height = data["height"].toInt();
		if (width > 0 && height > 0) {
			mScene->createSharedTexture(id, width, height);
		}
		printf("Received command: %s %d %d %d\n", "createSharedTexture", id, width, height);
	} else if (type == "destroySharedTexture") {
		int id = data["id"].toInt();
		mScene->destroySharedTexture(id);
		printf("Received command: %s %d\n", "destroySharedTexture", id);
	} else {
		printf("Received unknown command\n");
	}
}

void MxServer::newConnection()
{
	auto client = mServer->nextPendingConnection();
	if (client) {
		connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
		connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
		printf("%s\n", "Client connected");
	}
}

void MxServer::clientDisconnected()
{
	auto client = static_cast<QTcpSocket *>(QObject::sender());
	if (client) {
		printf("%s\n", "Client disconnected");
	}
}

void MxServer::readyRead()
{
	auto client = static_cast<QTcpSocket *>(QObject::sender());
	if (client) {
		QJsonParseError error;
		auto data = client->readAll();
		auto document = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			fprintf(stderr, "Error parsing JSON\n");
			return;
		}
		if (document.isObject()) {
			auto object = document.object();
			auto typeValue = object["type"];
			if (typeValue.isString()) {
				auto type = typeValue.toString();
				handleRequest(client, type, object);
			}
		}
	}
}
