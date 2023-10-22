#ifndef IMGEDIT_HPP
#define IMGEDIT_HPP

#include <QWidget>
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QIntValidator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QMessageBox>


class imgedit : public QWidget
{
    Q_OBJECT

public:
    imgedit(QWidget *parent = nullptr);
    ~imgedit();

private:
    QStringList argv;
    int argc;
    QString path;
    const int default_width = 300;
    QIntValidator *int_validator;
    int width;
    int height;
    bool thumb;
    bool ovrrd;

    QCheckBox *w_chkbox;
    QCheckBox *h_chkbox;
    QLineEdit *w_le;
    QLineEdit *h_le;
    QCheckBox *generate_thumbnail_chkbox;
    QCheckBox *override_existing_imgs_chkbox;
    QPushButton *exec_btn;

    QGridLayout *layout;

    void change_generate_thumbnail_chkbox_state();
    void w_chkbox_state_changed();
    void h_chkbox_state_changed();
    void exec_btn_pressed();
    void edit();
};
#endif // IMGEDIT_HPP
