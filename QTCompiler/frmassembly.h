#ifndef FRMASSEMBLY_H
#define FRMASSEMBLY_H

#include <QDialog>

namespace Ui {
class frmAssembly;
}

class frmAssembly : public QDialog
{
    Q_OBJECT

public:
    explicit frmAssembly(QWidget *parent = nullptr);
    ~frmAssembly();

    explicit frmAssembly(std::string assembly, QWidget *parent = nullptr);

private slots:
    void on_btnClose_clicked();

    void on_btnSave_clicked();

private:
    Ui::frmAssembly *ui;
};

#endif // FRMASSEMBLY_H
