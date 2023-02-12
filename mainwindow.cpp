#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QtNetwork>
#include <QFile>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->gridLayout);
    Scarica();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete graphView;//distruggo il puntatore
    delete graphView2;//distruggo il puntatore
    delete graphView3;//distruggo il puntatore
}


void MainWindow::Scarica()
{

    download("Universal");
    download("Emi");
    ins_artist(Universal, Emi);
    count_firstLetter_artist(Universal, "Universal");
    count_firstLetter_artist(Emi, "Emi");
    count_artist(Universal, Emi);
    QString currentPath = QDir::currentPath();
    QFile file(currentPath+"/Universal.txt");
    if (file.exists()) {
        file.remove();
    }
    QString currentPath2 = QDir::currentPath();
    QFile file2(currentPath2+"/Emi.txt");
    if (file2.exists()) {
        file2.remove();
    }
}

void MainWindow::download(const QString &etichetta){
    QUrl url;
    if(etichetta == "Universal"){
         url = QString("http://www.ivl.disco.unimib.it/minisites/cpp/List_of_Universal_artists.txt");
    }else if(etichetta == "Emi"){
         url = QString("http://www.ivl.disco.unimib.it/minisites/cpp/List_of_EMI_artists.txt");
    }
    QNetworkAccessManager manager;
        QNetworkReply *response = manager.get(QNetworkRequest(url));
        QEventLoop event;
        QObject::connect(response, &QNetworkReply::finished, &event, &QEventLoop::quit);
        event.exec();

        if (response->error() != QNetworkReply::NoError) {
            qDebug() << "Error: " << response->errorString();
            return;
        }

        QFile file(etichetta+".txt");
           file.open(QIODevice::WriteOnly);
           file.write(response->readAll());
           file.close();
               if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                   qDebug() << "Cannot open file for reading";
                   return;
               }

               QTextStream in(&file);
               if(etichetta == "Universal"){
                   while (!in.atEnd()) {
                       QString line = in.readLine();
                       QString key = line.split(" ")[1].trimmed().replace("_", " ");
                       if(key[0] == QChar(32))
                          key.remove(0,1);
                       key[0] =key[0].toUpper();

                       QString value = line.split(" ")[0].trimmed();

                       Universal.insert(key,value);
                   }
               }else if(etichetta == "Emi"){
                   while (!in.atEnd()) {
                       QString line = in.readLine();

                       Emi.insert(line.split(" ")[1].trimmed().replace("_", " "),line.split(" ")[0].trimmed());
                   }
               }
           file.close();
           delete response;

}
void MainWindow::ins_artist(QMap <QString,QString> Universal,QMap <QString,QString> Emi){
    for (const QString &key : Universal.keys()){
        QString app = Universal.value(key);
        QString app2 = app;
        QLabel *label =  new QLabel();
        //qDebug()<< app[1];
        if(app[1] == QChar(104)){
            app2.remove(0,9);
            app2.remove(2,app2.size());
            app.remove(0,26);
            app.remove(app.size()-1,app.size());
            //label->setText("<a href='https://en.wikipedia.org/wiki/"+key+"'>"+key+"</a>");
            label->setText("<a href='https://"+app2+".wikipedia.org/"+app+"'>"+key+"</a>");
        }else{
            app.remove(0,2);
            app.remove(app.size()-1,app.size());
            //label->setText("<a href='https://en.wikipedia.org/wiki/"+key+"'>"+key+"</a>");
            label->setText("<a href='https://en.wikipedia.org/"+app+"'>"+key+"</a>");
        }
        label->setOpenExternalLinks(true);
        label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        ui->scrollArea_2->widget()->layout()->addWidget(label);
    }
    for (const QString &key : Emi.keys()){
        QString app = Emi.value(key);
        app.remove(0,2);
        app.remove(app.size()-1,app.size());
        QLabel *label =  new QLabel();
        label->setText("<a href='https://en.wikipedia.org/"+app+"'>"+key+"</a>");
        label->setOpenExternalLinks(true);
        label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        ui->scrollArea->widget()->layout()->addWidget(label);
    }
}
void MainWindow::count_firstLetter_artist(const QMap<QString, QString> etichetta,const QString &str){
    QMap<QString, int> artistMap;
    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Numero artisti per lettera");
    QStringList categories;
    if(str == "Universal"){
    for (const QString &key : etichetta.keys()){
        QString firstLetter = key.left(1);
        if (artistMap.contains(firstLetter)) {
                    artistMap[firstLetter]++;
                } else {
                    artistMap[firstLetter] = 1;
                }
    }
    if(graphView == nullptr){

        //creo le tab del grapho
        for (const auto &letter : artistMap.keys()) {

            *set << artistMap.value(letter);
            categories << letter;
            //series->append(set);
        }
        series->append(set);
        //setto le tab del grafo con i valori ottenuti dalal scansione
        int max = 150;
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Grafico Artisti Universal per lettera");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0,max);   //SITEMARE 0, MAX
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        graphView = new QChartView(chart);
        graphView->setRenderHint(QPainter::Antialiasing);
        graphView->setMaximumSize(720,300);
        ui->horizontalLayout->addWidget(graphView);
        show();
        }
}
    if(str == "Emi"){
    for (const QString &key : etichetta.keys()){
        QString firstLetter = key.left(1);
        if (artistMap.contains(firstLetter)) {
                    artistMap[firstLetter]++;
                } else {
                    artistMap[firstLetter] = 1;
                }
    }
    if(graphView2 == nullptr){

        //creo le tab del grapho
        for (const auto &letter : artistMap.keys()) {

            *set << artistMap.value(letter);
            categories << letter;
            //series->append(set);
        }
        series->append(set);
        //setto le tab del grafo con i valori ottenuti dalal scansione
        int max = 150;
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Grafico Artisti EMI per lettera");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0,max);   //SITEMARE 0, MAX
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        graphView2 = new QChartView(chart);
        graphView2->setRenderHint(QPainter::Antialiasing);
        graphView2->setMaximumSize(720,300);
        ui->horizontalLayout_2->addWidget(graphView2);
        show();

        }
}
}
void MainWindow::count_artist(QMap <QString,QString> Universal,QMap <QString,QString> Emi){
    int contU = Universal.size();
    int contE = Emi.size();
    int max = 0;
    if(graphView3 == nullptr){
        //creo le tab del grapho
        QBarSet *set0 = new QBarSet("Universal");
        QBarSet *set1 = new QBarSet("Emi");
        //setto le tab del grafo con i valori ottenuti dalal scansione
        *set0 << contU;
        *set1 << contE;
        QBarSeries *series = new QBarSeries();
        series->append(set0);
        series->append(set1);
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Numero Artisti");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        QStringList categories;
        categories << "";
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0,max);   //SITEMARE 0, MAX
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        graphView3 = new QChartView(chart);
        graphView3->setRenderHint(QPainter::Antialiasing);
        graphView3->setMaximumSize(250,300);
        ui->horizontalLayout_3->addWidget(graphView3);
        show();

        }
}


