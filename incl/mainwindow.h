#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QMainWindow>
#include <QtGui/QLineEdit>

namespace Enc
{
class ExSetView;
class ExSetModel;
class ExSetSortFilterModel;
class ExSetFilterWidget;
class ExSetCheckWidget;
struct CatalogEntry;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void onCalcCRC4File();
    void onReadCellHeader();
    void onAboutQt();
    void onAbout();
    void onOpenClicked();
    void readAndViewCatalog();

private:

    QLineEdit * sourceEdt;

    Enc::ExSetFilterWidget * exSetFilterWgt;
    Enc::ExSetView * exSetView;
    Enc::ExSetCheckWidget * exSetCheckWgt;
    Enc::ExSetModel * exSetModel;
    Enc::ExSetSortFilterModel * sortFilterModel;
};

#endif // MAINWINDOW_H
