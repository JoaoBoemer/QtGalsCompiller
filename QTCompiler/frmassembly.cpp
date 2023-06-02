#include "frmassembly.h"
#include "ui_frmassembly.h"
#include <iostream>
#include <qfile.h>
#include <qfiledialog.h>
#include "qtextstream.h"

frmAssembly::frmAssembly(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmAssembly)
{
    ui->setupUi(this);
}

frmAssembly::~frmAssembly()
{
    delete ui;
}

frmAssembly::frmAssembly(std::string assembly, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::frmAssembly)
{
    ui->setupUi(this);
    ui->txtAssembly->setText(assembly.c_str());
    //ui->textEdit->append(Mensagem.c_str());
}

void frmAssembly::on_btnClose_clicked()
{
    this->close();
}


void frmAssembly::on_btnSave_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Salvar assembly"), "",
                                                    tr("Assembly(*.asm);;All Files(*.*)"));
    QFile writeFile(filePath);
    QString content = ui->txtAssembly->toPlainText();
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

