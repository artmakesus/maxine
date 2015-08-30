#include <MxSceneIPC.hpp>
#include <MxScene.hpp>

#include <QtDBus/QtDBus>

const QString MxSceneIPC::DBUS_SERVICE_NAME = "com.artmakesus.maxine";

MxSceneIPC::MxSceneIPC(MxScene *scene) :
	QObject(scene)
{
	mIsRegistered = connect();
}

bool MxSceneIPC::connect()
{
	if (!QDBusConnection::sessionBus().isConnected()) {
		fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
				"To start it, run:\n"
				"\teval `dbus-launch --auto-syntax`\n");
		return false;
	}

	if (!QDBusConnection::sessionBus().registerService(DBUS_SERVICE_NAME)) {
		fprintf(stderr, "%s\n", qPrintable(QDBusConnection::sessionBus().lastError().message()));
		return false;
	}

	QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSlots);
	return true;
}

bool MxSceneIPC::isRegistered() const
{
	return mIsRegistered;
}

bool MxSceneIPC::createSharedTexture(const QString &key, int index, int width, int height)
{
	auto scene = static_cast<MxScene*>(parent());
	return scene->createSharedTexture(key, index, width, height);
}

bool MxSceneIPC::invalidateSharedTexture(int index)
{
	auto scene = static_cast<MxScene*>(parent());
	return scene->invalidateSharedTexture(index);
}
