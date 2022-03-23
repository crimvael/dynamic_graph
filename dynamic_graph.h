#ifndef DYNAMIC_GRAPH_H
#define DYNAMIC_GRAPH_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class dynamic_graph; }
QT_END_NAMESPACE

class dynamic_graph : public QMainWindow
{
    Q_OBJECT

public:
    dynamic_graph(QWidget *parent = nullptr);
    ~dynamic_graph();

private slots:
    void paintEvent(QPaintEvent *);
    void new_elements();
    void new_nodes();
    void on_Automatico_clicked();
    void on_Manuale_clicked();
    void on_next_clicked();
    void on_stop_clicked();
    void set_var();
    void on_checkBox_toggled(bool checked);
    void on_checkBox_6_toggled(bool checked);
    void on_info_clicked();
    void on_indietro_clicked();
    void on_stop_2_clicked();
    void set_col(int m, int* r, int *g, int* b);
    void on_pushButton_clicked();

    void on_checkBox_2_toggled(bool checked);

private:
    Ui::dynamic_graph *ui;
};
#endif // DYNAMIC_GRAPH_H
