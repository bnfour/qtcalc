#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <expressionparser.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    ExpressionParser *parser;

private slots:
    void on_exprEdit_textChanged(const QString &arg1);
    void on_calcButton_clicked();
};
#endif // DIALOG_H
