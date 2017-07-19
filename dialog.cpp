#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    // removing the "?" as it's not used
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // setting a regexp validator for text input
    // the regexp is basically "the string contains only letters, numbers, whitespaces and ()/*+- symbols"
    QRegExp re("^[0-9a-z\\+\\-\\/\\(\\)\\* ]*$");
    ui->exprEdit->setValidator(new QRegExpValidator(re, this));
    // an object to evaluate expressions
    parser = new ExpressionParser();
}

Dialog::~Dialog()
{
    delete ui;
    delete parser;
}

// enables the button when there is some text in the text field
void Dialog::on_exprEdit_textChanged(const QString &arg1)
{
    bool isEnabled = (arg1.length() > 0);
    ui->calcButton->setEnabled(isEnabled);
}
// on button click, evaluate the expression
void Dialog::on_calcButton_clicked()
{
    QString result = parser->calc(ui->exprEdit->text());
    ui->resultLabel->setText(result);
    ui->exprEdit->setFocus();
}
