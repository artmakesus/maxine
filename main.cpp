#include <QApplication>

#include <MxMainWindow.hpp>

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	MxMainWindow window;
	window.show();

	return app.exec();
}
