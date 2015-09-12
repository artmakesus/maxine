#ifndef MX_SCENEIPC_HPP
#define MX_SCENEIPC_HPP

#include <QObject>

class MxScene;

class MxSceneIPC : public QObject {
	Q_OBJECT
public:
	MxSceneIPC(MxScene *scene);

	// Connect to DBus
	bool connect();

	// Is registered on DBus?
	bool isRegistered() const;
	
public slots:
	// DBus method that creates shared texture
	Q_SCRIPTABLE bool createSharedTexture(const QString &key, int index, int width, int height);

	// DBus method that invalidates shared texture
	Q_SCRIPTABLE bool invalidateSharedTexture(int index);

private:
	static const QString DBUS_SERVICE_NAME;

	bool mIsRegistered;
};

#endif
