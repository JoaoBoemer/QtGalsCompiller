#ifndef FRMTABELA_H
#define FRMTABELA_H

#include <QDialog>
#include "Semantico.h"

namespace Ui {
class frmTabela;
}

class frmTabela : public QDialog
{
    Q_OBJECT

public:
    explicit frmTabela(QWidget *parent = nullptr);
    ~frmTabela();

public:
    explicit frmTabela(list<Simbolo> tabSim, QWidget *parent = nullptr);

    list<Simbolo> tabelaSimbolos;
private slots:
    void on_btnClose_clicked();

private:
    Ui::frmTabela *ui;
};

#endif // FRMTABELA_H
