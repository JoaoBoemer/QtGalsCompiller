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
        Mensagem.append("ID: ");
        Mensagem.append(sim.id);
        Mensagem.append("\nTipo: ");
        Mensagem.append(sim.tipo);
        Mensagem.append("\nEscopo: ");
        Mensagem.append(to_string(sim.escopo));
        Mensagem.append("\nInicializado?: ");
        Mensagem.append(to_string(sim.inicializado));
        Mensagem.append("\nUsado?: ");
        Mensagem.append(to_string(sim.usado));
        Mensagem.append("\nFuncao? ");
        Mensagem.append(to_string(sim.funcao));
        Mensagem.append("\nParametro? ");
        Mensagem.append(to_string(sim.parametro));
        Mensagem.append("\nVetor? ");
        Mensagem.append(to_string(sim.vetor));
        Mensagem.append("\nPosicoesVetor? ");
        Mensagem.append(to_string(sim.posVetor));
        Mensagem.append("\nPosicaoParam ");
        Mensagem.append(to_string(sim.posParam));
        Mensagem.append("\n----------------------------\n");
    }
    //ui->textEdit->append("ID | TIPO | ESCOPO | FUNCAO | PARAMETRO | VETOR");
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

