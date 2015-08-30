#ifndef MX_SCENEIPC_HPP
#define MX_SCENEIPC_HPP

#include <QObject>

class MxScene;

class MxSceneIPC : public QObject {
	Q_OBJECT
public:
	MxSceneIPC(MxScene *scene);

	bool connect();
	bool isRegistered() const;
	
public slots:
	Q_SCRIPTABLE bool createSharedTexture(const QString &key, int index, int width, int height);
	Q_SCRIPTABLE bool invalidateSharedTexture(int index);

private:
	static const QString DBUS_SERVICE_NAME;

	bool mIsRegistered;
};

#endif
