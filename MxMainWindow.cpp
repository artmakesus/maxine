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
	// Set window title
	setWindowTitle(tr("Maxine"));

	// Setup Scene
	mScene = new MxScene;
	mGraphicsView = new QGraphicsView(mScene);
	mGraphicsView->setViewport(new MxOpenGLWidget);
	mGraphicsView->setFrameShape(QFrame::NoFrame);

	// Setup the rest of the window
	initMenuBar();
	initMenus();
	initActions();
	initShortcuts();

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
	mAddShapeAction = mEditMenu->addAction(tr("Add Shape"));
	mDeleteShapeAction = mEditMenu->addAction(tr("Delete Shape"));

	// View menu
	mViewMenu = menuBar()->addMenu(tr("&View"));
	mToggleFullscreenAction = mViewMenu->addAction(tr("Toggle Fullscreen"));
	mToggleMarkersAction = mViewMenu->addAction(tr("Toggle Markers"));
	mToggleScrollBarAction = mViewMenu->addAction(tr("Toggle ScrollBar"));
}

void MxMainWindow::initActions()
{
	// File actions
	connect(mNewAction, &QAction::triggered, this, &MxMainWindow::new_);
	connect(mLoadAction, &QAction::triggered, this, &MxMainWindow::load);
	connect(mSaveAction, &QAction::triggered, this, &MxMainWindow::save);

	// Edit actions
	connect(mAddShapeAction, &QAction::triggered, mScene, &MxScene::addEmptyShape);
	connect(mDeleteShapeAction, &QAction::triggered, mScene, &MxScene::deleteSelectedShapes);

	// View actions
	connect(mToggleFullscreenAction, &QAction::triggered, this, &MxMainWindow::toggleFullscreen);
	connect(mToggleMarkersAction, &QAction::triggered, mScene, &MxScene::toggleMarkers);
	connect(mToggleScrollBarAction, &QAction::triggered, this, &MxMainWindow::toggleScrollBar);
}

void MxMainWindow::initShortcuts()
{
	// File shortcuts
	mNewShortcut = new QShortcut(QKeySequence(tr("Ctrl+N", "File|New Project")), this);
	mLoadShortcut = new QShortcut(QKeySequence(tr("Ctrl+L", "File|Load Project")), this);
	mSaveShortcut = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save Project")), this);
	connect(mNewShortcut, &QShortcut::activated, this, &MxMainWindow::new_);
	connect(mLoadShortcut, &QShortcut::activated, this, &MxMainWindow::load);
	connect(mSaveShortcut, &QShortcut::activated, this, &MxMainWindow::save);

	// Edit shortcuts
	mAddShapeShortcut = new QShortcut(QKeySequence(tr("N", "Edit|Add Shape")), this);
	mDeleteShapeShortcut = new QShortcut(QKeySequence(tr("Backspace", "Edit|Delete Shape")), this);
	connect(mAddShapeShortcut, &QShortcut::activated, mScene, &MxScene::addEmptyShape);
	connect(mDeleteShapeShortcut, &QShortcut::activated, mScene, &MxScene::deleteSelectedShapes);

	// View shortcuts
	mToggleFullscreenShortcut = new QShortcut(QKeySequence(tr("F11", "View|Toggle Fullscreen")), this);
	mToggleMarkersShortcut = new QShortcut(QKeySequence(tr("M", "View|Toggle Markers")), this);
	mToggleScrollBarShortcut = new QShortcut(QKeySequence(tr("F12", "View|Toggle Scroll Bar")), this);
	connect(mToggleFullscreenShortcut, &QShortcut::activated, this, &MxMainWindow::toggleFullscreen);
	connect(mToggleMarkersShortcut, &QShortcut::activated, mScene, &MxScene::toggleMarkers);
	connect(mToggleScrollBarShortcut, &QShortcut::activated, this, &MxMainWindow::toggleScrollBar);
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
		menuBar()->setVisible(true);
	} else {
		showFullScreen();
		menuBar()->setVisible(false);
	}
}

void MxMainWindow::toggleScrollBar()
{
	if (mGraphicsView->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff) {
		mGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		mGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	} else {
		mGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		mGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}
