#include "dynamic_graph.h"
#include "ui_dynamic_graph.h"
#include <QTimer>
#include <QPainter>
#include <QWidget>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <algorithm>
#include <QIntValidator>

extern int _time;
extern int max_nodes;
extern int max_new_nodes;
extern int max_new_edges;
extern int millisec;
extern bool flag;
extern bool is_auto;
extern bool _stop;
extern bool unlimited;
extern QTimer *timer;

int _time = 0;
int max_nodes = 30;
int max_new_nodes = 5;
int max_new_edges = 5;
int millisec = 1000;
bool flag = false;  // se vera, permette a QPainter di disegnare, altrimenti cancella tutti i disegni
bool is_auto = false; // vera quando è in modalità automatica
bool _stop = false;  // per il pulsante stop
bool unlimited = false;
QTimer *timer = new QTimer();

struct node {
    int x; // coordinata x del nodo
    int y; // coordinata y del nodo

    node(int xx, int yy) : x(xx), y(yy) {}
};

struct edge {
    node p1; // primo nodo
    node p2; // secondo nodo
    int   n; // numero di iterazioni

    edge(node xx, node yy) : p1(xx), p2(yy), n(1) {}
};

extern QList<node> nodes;
extern QList<edge> edges;
QList<node> nodes;
QList<edge> edges;

dynamic_graph::dynamic_graph(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::dynamic_graph)
{
    ui->setupUi(this);
    ui->lineEdit_10->setValidator(new QIntValidator(0, 100, this));
    ui->lineEdit_11->setValidator(new QIntValidator(0, 100, this));
    ui->lineEdit_12->setValidator(new QIntValidator(0, 100, this));
    ui->lineEdit->setValidator(new QIntValidator(0, 10000, this));
    ui->frame->setVisible(false);
    ui->frame_3->setVisible(false);
    ui->frame_4->setVisible(false);
    ui->frame_5->setVisible(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(new_elements()));
}

dynamic_graph::~dynamic_graph()
{
    delete ui;
}

// inizializza l'applicazione
void dynamic_graph::set_var()
{
    bool ok;
    max_nodes = ui->lineEdit_10->text().toInt(&ok, 10);
    max_new_nodes = ui->lineEdit_11->text().toInt(&ok, 10);
    max_new_edges = ui->lineEdit_12->text().toInt(&ok, 10);
    millisec = ui->lineEdit->text().toInt(&ok, 10);
    if(ui->checkBox_6->isChecked()) {
        max_nodes = 30;
        max_new_nodes = 30;
        millisec = 1000;
    }
    ui->frame->setVisible(true);
    ui->frame_2->setVisible(false);
    ui->frame_3->setVisible(true);
    ui->frame_5->setVisible(true);
    ui->label_9->setNum(0);
    ui->label_10->setNum(nodes.size());
    ui->label_11->setNum(edges.size());
}

void dynamic_graph::on_Automatico_clicked()
{
    set_var();
    new_nodes();
    update();
    ui->next->setEnabled(false);
    ui->pushButton->setEnabled(true);
    timer->start(millisec);
    flag = true;
    is_auto = true;
}

void dynamic_graph::on_Manuale_clicked()
{
    set_var();
    new_nodes();
    update();
    ui->next->setEnabled(true);
    ui->pushButton->setEnabled(false);
    flag = true;
}

void dynamic_graph::on_next_clicked()
{
    new_elements();
    update();
}

// ritorna al menu
void dynamic_graph::on_stop_clicked()
{
    flag = false;
    timer->stop();
    nodes.clear();
    edges.clear();
    _time = 0;
    max_nodes = 30;
    max_new_nodes = 5;
    max_new_edges = 5;
    if(is_auto) {
        _stop = false;
        ui->pushButton->setText("Stop");
        is_auto = false;
    }
    ui->frame->setVisible(false);
    ui->frame_2->setVisible(true);
    ui->frame_3->setVisible(false);
    ui->frame_5->setVisible(false);
    ui->label_9->clear();
    ui->label_10->clear();
    ui->label_11->clear();
    ui->label_13->clear();
    ui->label_14->clear();
    update();
}

void dynamic_graph::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(flag==true) {

        // disegna tutti gli archi
        for (int i = 0; i<edges.size(); i++) {
            QLine line(edges.at(i).p1.x+8, edges.at(i).p1.y+8, edges.at(i).p2.x+8, edges.at(i).p2.y+8);
            painter.drawLine(line);
        }

        // visualizza contatore interazioni
        for (int i = 0; i<edges.size(); i++) {
            painter.drawText((edges[i].p1.x + edges[i].p2.x)/2, (edges[i].p1.y + edges[i].p2.y)/2, QString("+" + QString::number(edges[i].n)));
        }

        // disegna tutti i nodi
        for (int i = 0; i<nodes.size(); i++) {
            QRectF rectangle(nodes.at(i).x, nodes.at(i).y, 16, 16);
            painter.setBrush(QColor(250,0,0));
            painter.drawEllipse(rectangle);
        }

        // cerca l'arco con il maggior numero di interazioni
        int r = 1;
        for (int i = 0; i<edges.size(); i++) {
            if(edges.at(i).n >= r)
                r = edges.at(i).n;
        }

        // a partire dall' arco con meno interazioni, vengono aggiornati i colori dei nodi
        for (int n = 1; n <= r; n++) {
            for (int i = 0; i<edges.size(); i++) {
                if(edges.at(i).n == n) {
                    QRectF rectangle(edges.at(i).p1.x, edges.at(i).p1.y, 16, 16);
                    QRectF rectangle2(edges.at(i).p2.x, edges.at(i).p2.y, 16, 16);
                    int m = edges[i].n;    // fino a un massimo di 30 colori diversi
                    while (m > 30) {       // per ogni iterazione, poi si ripete
                        m -= 30;
                    }
                    int r = 0, g = 0, b = 0;
                    set_col(m, &r, &g, &b); // assegna un colore in funzione del numero di interazione
                    painter.setBrush(QColor(r,g,b));
                    painter.drawEllipse(rectangle);
                    painter.drawEllipse(rectangle2);
                }
            }
        }

        // visualizza la tavolozza dei colori
        for (int i=1, x=550; i<=30; i++, x+=23) {
            int r = 0, g = 0, b = 0;
            set_col(i, &r, &g, &b);
            painter.setBrush(QColor(r,g,b));
            QRectF rectangle3(x, 595, 16, 16);
            painter.drawEllipse(rectangle3);
            painter.drawText(x+4, 625, QString::number(i));
        }
    }
    else
        // cancella il disegno
        painter.eraseRect(0, 0, 1280, 720);
}

void dynamic_graph::set_col(int m, int* r, int* g, int* b)
{
    if(m == 1){*r=250; *g=0; *b=0;}
    if(m == 2){*r=250; *g=50; *b=0;}
    if(m == 3){*r=250; *g=100; *b=0;}
    if(m == 4){*r=250; *g=150; *b=0;}
    if(m == 5){*r=250; *g=200; *b=0;}
    if(m == 6){*r=250; *g=250; *b=0;}
    if(m == 7){*r=200; *g=250; *b=0;}
    if(m == 8){*r=150; *g=250; *b=0;}
    if(m == 9){*r=100; *g=250; *b=0;}
    if(m == 10){*r=50; *g=250; *b=0;}
    if(m == 11){*r=0; *g=250; *b=0;}
    if(m == 12){*r=0; *g=250; *b=50;}
    if(m == 13){*r=0; *g=250; *b=100;}
    if(m == 14){*r=0; *g=250; *b=150;}
    if(m == 15){*r=0; *g=250; *b=200;}
    if(m == 16){*r=0; *g=250; *b=250;}
    if(m == 17){*r=0; *g=200; *b=250;}
    if(m == 18){*r=0; *g=150; *b=250;}
    if(m == 19){*r=0; *g=100; *b=250;}
    if(m == 20){*r=0; *g=50; *b=250;}
    if(m == 21){*r=0; *g=0; *b=250;}
    if(m == 22){*r=50; *g=0; *b=250;}
    if(m == 23){*r=100; *g=0; *b=250;}
    if(m == 24){*r=150; *g=0; *b=250;}
    if(m == 25){*r=200; *g=0; *b=250;}
    if(m == 26){*r=250; *g=0; *b=250;}
    if(m == 27){*r=250; *g=0; *b=200;}
    if(m == 28){*r=250; *g=0; *b=150;}
    if(m == 29){*r=250; *g=0; *b=100;}
    if(m == 30){*r=250; *g=0; *b=50;}
}

// genera casualmente da 0 a max_new_nodes nuovi nodi
void dynamic_graph::new_nodes()
{
    if(nodes.size() <= max_nodes) {
        int num = qrand() % (max_new_nodes+1);
        if(num + nodes.size() <= max_nodes) {
            for (int e = num; e > 0; e--) {
                int randomValue1 = qrand() % 1220;
                int randomValue2 = qrand() % 490;
                if(randomValue1 < 50) // genera coordinate all' interno del frame
                    randomValue1 += 50;
                if(randomValue2 < 50)
                    randomValue2 += 50;
                nodes.append(node(randomValue1, randomValue2));
            }
            QString str4 = "+" + QString::number(num);
            ui->label_13->setText(str4);
        }
        else {
            QString str4 = "+" + QString::number(0);
            ui->label_13->setText(str4);
        }
        if(nodes.size() == max_nodes)
            ui->label_13->setText("max");
        ui->label_9->setNum(_time);
        ui->label_10->setText(QString::number(nodes.size())+"/"+QString::number(max_nodes));
        ui->label_11->setText(QString::number(edges.size())+"/"+QString::number((max_nodes*(max_nodes-1)/2)));
    }
}

// genera un numero casuale di nuovi nodi e nuovi archi
void dynamic_graph::new_elements()
{

    new_nodes();


    int m = (nodes.size()*(nodes.size()-1))/2; // numero massimo di archi per n nodi
    int num2;
    // genera tanti archi quanto il numero di nodi presenti
    if(m <= max_new_edges)
        num2 = qrand() % (m+1);
    else
        num2 = qrand() % (max_new_edges+1);
    if(ui->checkBox->isChecked())
        num2 = (nodes.size()*(nodes.size()-1))/2;
    for (int e = num2; e > 0; e--) {
        int randomValue1 = qrand() % nodes.size();
        int randomValue2 = qrand() % nodes.size();
        while (randomValue1 == randomValue2) { // i cappi vengono ignorati
            randomValue1 = qrand() % nodes.size();
            randomValue2 = qrand() % nodes.size();
        }
        // sceglie due nodi casuali e crea un arco
        edge new_edge(nodes.at(randomValue1), nodes.at(randomValue2));
        bool found = false;
        if(edges.size() >= 1){
            for (int i=0; i < edges.size(); i++) {
                // due archi sono uguali anche se hanno le coordinate dei punti invertiti,
                // quindi ne scelgo solo una, ma aggiorno il contatore delle interazioni
                if(((new_edge.p1.x == edges[i].p1.x) && (new_edge.p1.y == edges[i].p1.y) && (new_edge.p2.x == edges[i].p2.x) && (new_edge.p2.y == edges[i].p2.y)) || ((new_edge.p1.x == edges[i].p2.x) && (new_edge.p1.y == edges[i].p2.y) && (new_edge.p2.x == edges[i].p1.x) && (new_edge.p2.y == edges[i].p1.y))) {
                    edges[i].n++;
                    num2--;
                    found = true;
                }
            }
            if(!found) // se è un nuovo arco, lo aggiungo nella lista
                edges.append(new_edge);
        }
        else if(edges.size() == 0) // per il primo arco
            edges.append(new_edge);
    }
    QString str5 = "+" + QString::number(num2); // visualizza il numero di archi generati, esclusi i duplicati
    ui->label_14->setText(str5);

    if(edges.size() == (max_nodes*(max_nodes-1)/2))
        ui->label_14->setText("max");

    _time++;
    ui->label_9->setNum(_time);
    ui->label_10->setText(QString::number(nodes.size())+"/"+QString::number(max_nodes));
    ui->label_11->setText(QString::number(edges.size())+"/"+QString::number((max_nodes*(max_nodes-1)/2)));
    update();
    if(!unlimited)
        if(nodes.size() == max_nodes){
            if(is_auto)
                timer->stop();
            else
                ui->next->setEnabled(false);
        }
}

// imposta i parametri dell'applicazione secondo le richieste del testo
void dynamic_graph::on_checkBox_6_toggled(bool checked)
{
    if(checked) {
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_10->setEnabled(false);
        ui->lineEdit_11->setEnabled(false);
        ui->lineEdit_12->setEnabled(false);
        if(!ui->checkBox->isChecked())
            ui->checkBox->toggle();
        if(ui->checkBox_2->isChecked())
            ui->checkBox_2->toggle();
        ui->checkBox_2->setEnabled(false);
        ui->checkBox->setEnabled(false);
        ui->lineEdit->setText("1000");
        ui->lineEdit_10->setText("30");
        ui->lineEdit_11->setText("30");
        ui->lineEdit_12->setText("");
    }
    else {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_10->setEnabled(true);
        ui->lineEdit_11->setEnabled(true);
        ui->lineEdit_12->setEnabled(true);
        if(ui->checkBox->isChecked())
            ui->checkBox->toggle();
        if(ui->checkBox_2->isChecked())
            ui->checkBox_2->toggle();
        ui->checkBox_2->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->lineEdit->setText("1000");
        ui->lineEdit_10->setText("30");
        ui->lineEdit_11->setText("5");
        ui->lineEdit_12->setText("5");
    }
}

// se attivato, il numero massimo di archi generati dipende sempre dal numero dei nodi presenti
void dynamic_graph::on_checkBox_toggled(bool checked)
{
    if(checked)
        ui->lineEdit_12->setEnabled(false);
    else
        ui->lineEdit_12->setEnabled(true);
}

void dynamic_graph::on_info_clicked()
{
    ui->frame_2->setVisible(false);
    ui->frame_4->setVisible(true);
}

void dynamic_graph::on_indietro_clicked()
{
    ui->frame_2->setVisible(true);
    ui->frame_4->setVisible(false);
}

// riavvia il grafo
void dynamic_graph::on_stop_2_clicked()
{
    timer->stop();
    nodes.clear();
    edges.clear();
    _time = 0;
    ui->label_9->setNum(0);
    ui->label_10->clear();
    ui->label_11->clear();
    ui->label_13->clear();
    ui->label_14->clear();
    flag = false;
    update();
    new_nodes();
    flag = true;
    update();
    if(is_auto) {
        _stop = false;
        ui->pushButton->setText("Stop");
        timer->start();
    }
    else
        ui->next->setEnabled(true);
}

// mette in pausa
void dynamic_graph::on_pushButton_clicked()
{
    if(_stop) {
        _stop = false;
        ui->pushButton->setText("Stop");
        timer->start();
    }
    else {
        _stop = true;
        ui->pushButton->setText("Continua");
        timer->stop();
    }
}

void dynamic_graph::on_checkBox_2_toggled(bool checked)
{
    if(checked)
        unlimited = true;
    else
        unlimited = false;
}
