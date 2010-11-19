#include "mainwindow.h"
#include "globals.h"
#include "mytexts.h"
#include <exsetview.h>
#include <exSetFilterWidget.h>
#include <catalog031reader.h>
#include <checksum.h>
#include <cellheader_widget.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QSettings>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QFileDialog> 
#include <QtGui/QMessageBox> 
#include <QtGui/QHeaderView>

using namespace Enc;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), exSetModel(0)
{
    setWindowTitle(QString("%1 %2").arg(ExSetCheck::APP_DIALOG_NAME).arg(ExSetCheck::APP_VERSION));
    //**** The Menues ****
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
    // fileMenu->addAction(openAct);

    QMenu * optMenu = menuBar()->addMenu(tr("&Options"));

    QMenu * toolMenu = menuBar()->addMenu(tr("&Tools"));
    QAction * calcCRC4FileAct = toolMenu->addAction("Calculate CRC...", this, SLOT(onCalcCRC4File()));
    QAction * cellHeaderAct = toolMenu->addAction("Read Cell Header...", this, SLOT(onReadCellHeader()));

    QMenu * hlpMenu = menuBar()->addMenu(tr("&Help"));
    QAction * aboutAct = hlpMenu->addAction(QString("About ") +ExSetCheck::APP_DIALOG_NAME, this, SLOT(onAbout()));
    QAction * aboutQtAct = hlpMenu->addAction("About Qt", this, SLOT(onAboutQt()));

    //****** Central widget and Layout ******
    QWidget * centralWgt = new QWidget(this); 
    setCentralWidget(centralWgt);
    QVBoxLayout * mainLyt = new QVBoxLayout(centralWgt);
 
    //**** the Source ExSet: File Selection ****
    QHBoxLayout * sourceLyt = new QHBoxLayout();
    mainLyt->addLayout(sourceLyt);
    QLabel * sourceLbl = new QLabel(tr("Exchange Set: "), centralWidget());
	sourceEdt = new QLineEdit(centralWidget()); 
    QPushButton * sourceSelBtn = new QPushButton(tr("Open"), centralWidget());
    sourceLyt->addWidget(sourceLbl, 0);
    sourceLyt->addWidget(sourceEdt, 1);
    sourceLyt->addWidget(sourceSelBtn, 0);

    //**** Display the Catalog-content ****
    exSetFilterWgt = new ExSetFilterWidget(centralWidget());  
    exSetView = new ExSetView(centralWidget());
    exSetModel = new ExSetModel(this);
    sortFilterModel = new ExSetSortFilterModel(this);
    sortFilterModel->setSourceModel(exSetModel);
    exSetView->setModel(sortFilterModel);
    exSetView->showAllColumns(exSetFilterWgt->allColumnsOn());

    mainLyt->addWidget(exSetFilterWgt, 0);
    mainLyt->addWidget(exSetView, 1);

    exSetCheckWgt = new ExSetCheckWidget(centralWidget());
    mainLyt->addWidget(exSetCheckWgt, 0);

    //**** Connections ****
    connect(sourceSelBtn,  SIGNAL(clicked()), this, SLOT(onOpenClicked()));
    connect(exSetFilterWgt, SIGNAL(showAllColumns(bool)), exSetView, SLOT(showAllColumns(bool)));
    connect(exSetFilterWgt, SIGNAL(filter(ExSetFilterSettings)), exSetView, SLOT(doFilter(ExSetFilterSettings)));
    connect(exSetFilterWgt, SIGNAL(unFilter()), exSetView, SLOT(unFilter()));

    //**** inits ****
    //** set old size - rem: maybe screen has changed **
    QSettings mySettings(ExSetCheck::ORG_SETTING_NAME, ExSetCheck::APP_SETTING_NAME);
    bool wasFullScreen = mySettings.value("mainwindow/fullScreen").toBool();
    if (wasFullScreen) showMaximized();
    else
    {
        QSize oldWinSize = mySettings.value("mainwindow/size").toSize();
        const QRect screenRect = QApplication::desktop()->availableGeometry(-1);
        if (oldWinSize.width() <= screenRect.size().width() && oldWinSize.height() <= screenRect.size().height()) {
            resize(oldWinSize);
        }
    }
}

MainWindow::~MainWindow()
{
    //**** save Settings ****
    QSettings mySettings(ExSetCheck::ORG_SETTING_NAME, ExSetCheck::APP_SETTING_NAME);
    mySettings.setValue("mainwindow/size", size());
    mySettings.setValue("mainwindow/fullScreen", isMaximized());
    if (!sourceEdt->text().isEmpty()){
        mySettings.setValue("mainwindow/catalogFile", sourceEdt->text());
    }
}
void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this, ExSetCheck::APP_DIALOG_NAME);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, ExSetCheck::APP_DIALOG_NAME, aboutText);
}

//******************************************************************************
/// Select a File and calculate the CRC32 
/*!
***************************************************************************** */
void MainWindow::onCalcCRC4File()
{
    CheckSumDialog * crcDlg = new CheckSumDialog(this);
    crcDlg->show();  //not modal! - we can show several checksum-dialogs!
}

//******************************************************************************
/// Parse and Display the cell-header of a S-57 file
/*!
***************************************************************************** */
void MainWindow::onReadCellHeader()
{
    CellHeaderDialog * cellHeaderDlg = new CellHeaderDialog(this);
    cellHeaderDlg->show();
}

//******************************************************************************
/// Select Catalog-File and open it
/*!
***************************************************************************** */
void MainWindow::onOpenClicked()
{
    QString dir;
    //** if catalog is opened first time in session, try to use catalog of last session as start **
    if (sourceEdt->text().isEmpty())
    {
        QSettings mySettings(ExSetCheck::ORG_SETTING_NAME, ExSetCheck::APP_SETTING_NAME);
        QString lastCatalog = mySettings.value("mainwindow/catalogFile").toString();
        QDir recentDir(QFileInfo(lastCatalog).dir());
        if (recentDir.exists()) dir = recentDir.absolutePath();
    }
    //** start with the parent dir of last exchange set, or with home-dir **
    else
    {
        QDir recentDir(QFileInfo(sourceEdt->text()).dir());
        if (recentDir.exists())
        {
            recentDir.cdUp();
            dir = recentDir.absolutePath();
        }
        else dir = QDir::homePath();
    }
    QString exSetCat = QFileDialog::getOpenFileName(this, tr("Selecte Exchange Set Catalog"), dir, tr("Exchange Set Catalog (CATALOG.031);; All Files (*.*)"));
    if (exSetCat.isEmpty()) return;
    sourceEdt->setText(exSetCat);

    readAndViewCatalog();
}

//******************************************************************************
/// Open and Parse the Catalog File (read from the catalog-lineedit)
/*!
***************************************************************************** */
void MainWindow::readAndViewCatalog()
{
    exSetModel->init(sourceEdt->text());
    exSetView->resizeColumnToContents(CatalogEntry::COL_FILE);
    exSetView->horizontalHeader()->setClickable(true);
}