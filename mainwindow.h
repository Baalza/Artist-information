#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QNetworkReply>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:




private:
    void Scarica();
    Ui::MainWindow *ui;
    QChartView *graphView = nullptr;
    QChartView *graphView2 = nullptr;
    QChartView *graphView3 = nullptr;
    void download(const QString &etichetta);
    QMap<QString, QString> Universal;
    QMap<QString, QString> Emi;
    void ins_artist( QMap<QString, QString> Universal, QMap<QString, QString> Emi);
    void count_firstLetter_artist( QMap<QString, QString> disc, const QString &str);
    void count_artist( QMap<QString, QString> Universal, QMap<QString, QString> Emi);
};
#endif // MAINWINDOW_H
