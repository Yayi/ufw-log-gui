#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(15);
    ui->tableWidget->horizontalHeader()->setStyleSheet("background-color: #E6ECF6");
    ui->tableWidget->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    process = new QProcess();
    QStringList options;
    options << "-c" << "journalctl -f";
    process->start("/bin/bash", options);
    connect(process , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_process_readoutput()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_process_readoutput()
{
    QString buf = process->readLine().data();
    if(buf.contains("[UFW ",Qt::CaseInsensitive))
    {
        int ufw_index=buf.indexOf("UFW ")+4;
        int ufw_lastdex=buf.indexOf("] ");
        QString tab_Operational=buf.mid(ufw_index,ufw_lastdex-ufw_index);

        ufw_index=buf.indexOf("SRC=")+4;
        ufw_lastdex=buf.indexOf(" DST=");
        QString tab_Source=buf.mid(ufw_index,ufw_lastdex-ufw_index);

        ufw_index=buf.indexOf("SPT=")+4;
        ufw_lastdex=buf.indexOf(" DPT=");
        QString tab_SPT=buf.mid(ufw_index,ufw_lastdex-ufw_index);
        if(ufw_index<4)
            tab_SPT=" - ";

        ufw_index=buf.indexOf("DPT=")+4;
        ufw_lastdex=buf.indexOf(" WINDOW=");
        QString tab_DPT;

        if(ufw_index<4)
            tab_DPT=" - ";
        else {
            if(ufw_lastdex<0)
                ufw_lastdex=buf.lastIndexOf(" LEN=");
            tab_DPT=buf.mid(ufw_index,ufw_lastdex-ufw_index);
        }

        ufw_index=buf.indexOf("DST=")+4;
        ufw_lastdex=buf.indexOf(" LEN=");
        QString tab_Destination=buf.mid(ufw_index,ufw_lastdex-ufw_index);

        ufw_index=buf.indexOf("PROTO=")+6;
        ufw_lastdex=buf.indexOf(" SPT=");
        QString tab_Protocol=buf.mid(ufw_index,ufw_lastdex-ufw_index);

        ufw_index=buf.indexOf(":")-2;
        ufw_lastdex=ufw_index+8;
        QString tab_Time=buf.mid(ufw_index,8);
        QString tab_Date=buf.mid(0,5);

        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(tab_Date));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(tab_Time));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(tab_Source));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(tab_SPT));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,4,new QTableWidgetItem(tab_Destination));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,5,new QTableWidgetItem(tab_DPT));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,6,new QTableWidgetItem(tab_Protocol));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,7,new QTableWidgetItem(tab_Operational));

        ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

        for (int i=0; i<8 ; i++ )
            ui->tableWidget->item(ui->tableWidget->rowCount()-1,i)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

