#include "frmtabela.h"
#include "ui_frmtabela.h"
#include "Semantico.h"


frmTabela::frmTabela(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmTabela)
{
    ui->setupUi(this);
}

frmTabela::frmTabela(list<Simbolo> tabSim, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::frmTabela)
{
    ui->setupUi(this);
    tabelaSimbolos = tabSim;
    ui->textEdit->setText("");
    string Mensagem = "";

    for(Simbolo sim : tabSim){
        Mensagem.append(sim.id);
        Mensagem.append(" | ");
        Mensagem.append(sim.tipo);
        Mensagem.append(" | ");
        Mensagem.append(to_string(sim.escopo));
        Mensagem.append("\n");
    }
    ui->textEdit->append("ID | TIPO | ESCOPO | FUNCAO | ");
    ui->textEdit->append(Mensagem.c_str());
}

frmTabela::~frmTabela()
{
    delete ui;
}

void frmTabela::on_btnClose_clicked()
{
    this->close();
}

