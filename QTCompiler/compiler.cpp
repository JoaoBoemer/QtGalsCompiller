#include "compiler.h"
#include "ui_compiler.h"

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
}

Compiler::~Compiler()
{
    delete ui;
}



void Compiler::on_btnCompile_clicked()
{
    QString text = ui->txtCode->toPlainText();
    string teste = text.toStdString();
    const char * code = teste.c_str();
    Lexico* lex = new Lexico();
    Sintatico* sint = new Sintatico();
    Semantico* sem = new Semantico();

    lex->setInput(code);

    try
    {
        sint->parse(lex, sem);
        ui->txtResult->setText("Compilou :D");
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
        ui->txtResult->setText(e.getMessage());
    }

}


void Compiler::on_btnSave_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Salva il File"), "",
                                                    tr("File di Testo (.txt);;C++ File (.cpp *.h)"));

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
