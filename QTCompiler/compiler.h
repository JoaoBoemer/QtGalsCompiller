
#ifndef COMPILER_H
#define COMPILER_H

#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class Compiler; }
QT_END_NAMESPACE

class Compiler : public QMainWindow

{
    Q_OBJECT

public:
    Compiler(QWidget *parent = nullptr);
    ~Compiler();

private slots:
    void on_btnCompile_clicked();

    void on_btnSave_clicked();

    void on_btnLoad_clicked();

private:
    Ui::Compiler *ui;
};

#endif // COMPILER_H
