#ifndef MX_MAINWINDOW_HPP
#define MX_MAINWINDOW_HPP

#include <QMainWindow>

class MxScene;
class QGraphicsView;
class QShortcut;

class MxMainWindow : public QMainWindow {
public:
	MxMainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

private:
	MxScene *mScene;
	QGraphicsView *mGraphicsView;

	QMenu *mFileMenu;
	QMenu *mEditMenu;
	QMenu *mViewMenu;

	QAction *mNewAction;
	QAction *mLoadAction;
	QAction *mSaveAction;
	QAction *mAddShapeAction;
	QAction *mDeleteShapeAction;
	QAction *mToggleFullscreenAction;
	QAction *mToggleMarkersAction;
	QAction *mToggleScrollBarAction;

	QShortcut *mNewShortcut;
	QShortcut *mLoadShortcut;
	QShortcut *mSaveShortcut;
	QShortcut *mAddShapeShortcut;
	QShortcut *mDeleteShapeShortcut;
	QShortcut *mToggleFullscreenShortcut;
	QShortcut *mToggleMarkersShortcut;
	QShortcut *mToggleScrollBarShortcut;

	void initMenuBar();
	void initMenus();
	void initActions();
	void initShortcuts();

private slots:
	void new_();
	void save();
	void load();
	void toggleFullscreen();
	void toggleScrollBar();
};

#endif
