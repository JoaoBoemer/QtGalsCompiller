#include "compiler.h"
#include "ui_compiler.h"
#include "frmtabela.h"
#include "frmassembly.h"

#include <iostream>
#include <fstream>
#include "qfiledialog.h"
#include "qfile.h"
#include "qtextstream.h"

#include "Lexico.h"
#include "Sintatico.h"
#include "Semantico.h"

using namespace std;

Compiler::Compiler(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Compiler)
{
    ui->setupUi(this);
    this->sem = NULL;
}

Compiler::~Compiler()
{
    delete ui;
}



void Compiler::on_btnCompile_clicked()
{
    QString text = ui->txtCode->toPlainText();
    string teste = text.toStdString();
    string warning = "";
    const char * code = teste.c_str();
    Lexico* lex = new Lexico();
    Sintatico* sint = new Sintatico();

    lex->setInput(code);

    if(sem) {
        delete sem;
    }
    this->sem = new Semantico();

    try
    {
        sint->parse(lex, sem);

        for(Warning war : this->sem->Tabela.lstWarning)
        {
            warning.append("Warning: ");
            warning.append(war.error);
            warning.append(" | Var: ");
            warning.append(war.sim.id);
            warning.append(" | Escopo: ");
            warning.append(to_string(war.sim.escopo));
            warning.append("\n");
        }
        ui->txtResult->setText(warning.c_str());

        //ui->txtResult->append(this->sem->Tabela.assembly.c_str());

        ui->txtResult->append("Compilou :D");
    }
    catch( LexicalError e )
    {
        ui->txtResult->setText(e.getMessage());
    }
    catch ( SyntaticError e )
    {
        ui->txtResult->setText(e.getMessage());
    }
    catch ( SemanticError e )
    {
        if(this->sem != nullptr)
        {
            this->sem->Tabela.setUnusedWarning();
        }

        for(Warning war : this->sem->Tabela.lstWarning)
        {
            warning.append("Warning: ");
            warning.append(war.error);
            warning.append(" | Var: ");
            warning.append(war.sim.id);
            warning.append(" | Escopo: ");
            warning.append(to_string(war.sim.escopo));
            warning.append("\n");
        }
        ui->txtResult->setText(warning.c_str());

        ui->txtResult->append(e.getMessage());
    }

}


void Compiler::on_btnSave_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Salvar"), "",
                                                    tr("Bloco de notas(.txt);;C++ File (.cpp *.h)"));

    QFile writeFile(filePath);
    QString content = ui->txtCode->toPlainText();
    if(!writeFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        // Can't Open File.
    }
    else
    {
        if(content.length() > 0)
        {
            QTextStream in(&writeFile);

            in << content;
        }
    }
    writeFile.close();

}


void Compiler::on_btnLoad_clicked()
{
    ifstream file;
    QString text;

    QString dir = QFileDialog::getOpenFileName(this);

    file.open(dir.toStdString());

    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));

    std::cout << "File content:\n" << content << '\n';

    text = QString::fromStdString(content);
    ui->txtCode->setText(text);
    file.close();
}

void Compiler::on_btnTabela_clicked()
{
    if(this->sem == nullptr)
        return;
    if(this->sem->Tabela.lstSimbolos.empty())
        return;
    frmTabela * tabela = new frmTabela(this->sem->Tabela.lstSimbolos, this);
    tabela->show();
}


void Compiler::on_btnAssembly_clicked()
{
    if(this->sem == nullptr)
        return;
    frmAssembly * frmassembly = new frmAssembly(this->sem->Tabela.data, this);
    frmassembly->show();
}

