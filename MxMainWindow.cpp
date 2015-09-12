#include <MxMainWindow.hpp>

#include <MxScene.hpp>
#include <MxOpenGLWidget.hpp>
#include <QMenuBar>
#include <QGraphicsView>
#include <QFileDialog>
#include <QShortcut>

MxMainWindow::MxMainWindow(QWidget *parent, Qt::WindowFlags f) :
	QMainWindow(parent, f)
{
	// Setup Window
	setWindowTitle(tr("Maxine"));
	initMenuBar();
	initMenus();
	initActions();

	// Setup Scene
	mScene = new MxScene;
	mGraphicsView = new QGraphicsView(mScene);
	mGraphicsView->setViewport(new MxOpenGLWidget);

	setCentralWidget(mGraphicsView);
}

void MxMainWindow::initMenuBar()
{
	menuBar()->setNativeMenuBar(true);
}

void MxMainWindow::initMenus()
{
	// File menu
	mFileMenu = menuBar()->addMenu(tr("&File"));
	mNewAction = mFileMenu->addAction(tr("&New Project"));
	mLoadAction = mFileMenu->addAction(tr("&Load Project"));
	mSaveAction = mFileMenu->addAction(tr("&Save Project"));

	// Edit menu
	mEditMenu = menuBar()->addMenu(tr("&Edit"));
	mAddShapeAction = mEditMenu->addAction(tr("&Add Shape"));
	mDeleteShapeAction = mEditMenu->addAction(tr("&Delete Shape"));

	// View menu
	mViewMenu = menuBar()->addMenu(tr("&View"));
	mToggleFullscreenAction = mViewMenu->addAction(tr("Toggle &Fullscreen"));
	mToggleMarkersAction = mViewMenu->addAction(tr("Toggle &Markers"));
}

void MxMainWindow::initActions()
{
	// File actions
	mNewAction->setShortcut(QKeySequence(tr("Ctrl+N", "File|New Project")));
	mLoadAction->setShortcut(QKeySequence(tr("Ctrl+L", "File|Load Project")));
	mSaveAction->setShortcut(QKeySequence(tr("Ctrl+S", "File|Save Project")));
	connect(mNewAction, &QAction::triggered, this, &MxMainWindow::new_);
	connect(mLoadAction, &QAction::triggered, this, &MxMainWindow::load);
	connect(mSaveAction, &QAction::triggered, this, &MxMainWindow::save);

	// Edit actions
	mAddShapeAction->setShortcut(QKeySequence(tr("A", "Edit|Add Shape")));
	mDeleteShapeAction->setShortcut(QKeySequence(tr("D", "Edit|Delete Shape")));
	connect(mAddShapeAction, &QAction::triggered, mScene, &MxScene::addEmptyShape);
	connect(mDeleteShapeAction, &QAction::triggered, mScene, &MxScene::deleteSelectedShapes);

	// View actions
	mToggleFullscreenAction->setShortcut(QKeySequence(tr("F11", "View|Toggle Fullscreen")));
	mToggleMarkersAction->setShortcut(QKeySequence(tr("M", "View|Toggle Markers")));
	connect(mToggleFullscreenAction, &QAction::triggered, this, &MxMainWindow::toggleFullscreen);
	connect(mToggleMarkersAction, &QAction::triggered, mScene, &MxScene::toggleMarkers);
}

void MxMainWindow::new_()
{
	mScene->new_();
}

void MxMainWindow::save() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Maxine file (*.mxn)"));
	if (filename.size() > 0) {
		mScene->save(filename);
	}
}

void MxMainWindow::load()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Maxine file (*.mxn)"));

	QStringList fileNames;
	if (dialog.exec()) {
		fileNames = dialog.selectedFiles();
		if (fileNames.size() == 0) {
			return;
		}
		mScene->load(fileNames[0]);
	}
}

void MxMainWindow::toggleFullscreen()
{
	if (isFullScreen()) {
		showNormal();
	} else {
		showFullScreen();
	}
}
