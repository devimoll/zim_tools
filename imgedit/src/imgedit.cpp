#include "imgedit.hpp"

imgedit::imgedit(QWidget *parent)
    : QWidget(parent)
{
    argv = QCoreApplication::arguments();
    argc = argv.size();

    qDebug() << argc << argv;
    if (argc < 2) {
        qDebug() << "引数が少い。";
        QMessageBox::critical(this, tr("致命的エラー"), tr("引数が少い。"));
        //exit(1);
    }

    if (argc > 2) {

    }

    int_validator = new QIntValidator();
    int_validator->setRange(1, 9999);

    w_chkbox = new QCheckBox(tr("横: "));
    w_chkbox->setCheckState(Qt::Checked);
    h_chkbox = new QCheckBox(tr("縦: "));
    w_le = new QLineEdit(QString::number(default_width));
    w_le->setValidator(int_validator);
    h_le = new QLineEdit();
    h_le->setValidator(int_validator);
    generate_thumbnail_chkbox = new QCheckBox("サムネ生成");
    generate_thumbnail_chkbox->setCheckState(Qt::Checked);
    override_existing_imgs_chkbox = new QCheckBox("既存の上書");
    exec_btn = new QPushButton("実行");

    layout = new QGridLayout();
    layout->addWidget(w_chkbox, 0, 0);
    layout->addWidget(h_chkbox, 1, 0);
    layout->addWidget(w_le, 0, 1);
    layout->addWidget(h_le, 1, 1);
    layout->addWidget(generate_thumbnail_chkbox, 2, 0, 1, 2);
    layout->addWidget(override_existing_imgs_chkbox, 3, 0, 1, 2);
    layout->addWidget(exec_btn, 4, 0, 1, 2);

    setLayout(layout);

    connect(w_chkbox, &QCheckBox::stateChanged,
            this, &imgedit::w_chkbox_state_changed);
    connect(h_chkbox, &QCheckBox::stateChanged,
            this, &imgedit::h_chkbox_state_changed);
    connect(exec_btn, &QPushButton::pressed,
            this, &imgedit::exec_btn_pressed);
}

imgedit::~imgedit()
{
    delete int_validator;
    delete w_chkbox;
    delete h_chkbox;
    delete w_le;
    delete h_le;
    delete generate_thumbnail_chkbox;
    delete override_existing_imgs_chkbox;
    delete exec_btn;
    // layoutの中身をdeleteしてからlayoutをdeleteしないとsegmentation fault
    delete layout;
}

void imgedit::change_generate_thumbnail_chkbox_state()
{
    if (w_chkbox->checkState() == Qt::Checked
     || h_chkbox->checkState() == Qt::Checked) {
        generate_thumbnail_chkbox->setEnabled(true);
    }
    else {
        generate_thumbnail_chkbox->setEnabled(false);
        generate_thumbnail_chkbox->setCheckState(Qt::Unchecked);
    }

}

void imgedit::w_chkbox_state_changed()
{
    change_generate_thumbnail_chkbox_state();
}

void imgedit::h_chkbox_state_changed()
{
    change_generate_thumbnail_chkbox_state();
}

void imgedit::exec_btn_pressed()
{
    edit();
    exit(0);
}

void imgedit::edit()
{
/*
    const int BUF_SIZE = 256;
    const int TXT_MAX_SIZE = 1048576;

    FILE *fp;
    char buf[BUF_SIZE];
    char path[BUF_SIZE] = {'\0'};
    strncpy(path, argv[1].toUtf8().data(), BUF_SIZE);
    fp = fopen(path, "r");
    if (fp == NULL) {
        qDebug() << "failed to open file.\n";
        exit(1);
    }
*/
    QString source_ap = argv[1];
    QFile source(source_ap);
    if (!source.open(QIODevice::ReadOnly)) {
        qDebug() << "zimソースファイルの読み込みに失敗";
        QMessageBox::critical(this, tr("zimソースファイルの読み込みに失敗"), source_ap);
        exit(1);
    }
/*
    char txt[TXT_MAX_SIZE];
    int index = 0;
    while ((fgets(buf, BUF_SIZE, fp)) != NULL) {
        strcpy(&txt[index], buf);
        for (int i = 0; i < BUF_SIZE; i++) {
        if (buf[i] == '\0') {
            break;
        }
        index++;
        }
    }
    //printf("%s", txt);
    fclose(fp);
*/
    QString text = source.readAll();
    source.close();

    QString new_text = text;
    QRegularExpression img_re("({{)[^(}})]+");
    QRegularExpressionMatchIterator i = img_re.globalMatch(text);
    QStringList img_params;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString img_param = match.captured(0);
        img_params << img_param;
        qDebug() << img_param;
    }
    for (const QString p : img_params) {
        QString new_param = p;
        if (override_existing_imgs_chkbox->checkState() == Qt::Checked) {
            /* 置換するために?から}}の手前までの文字列を取得する必要があるにより正規表現を用ゐる。 */
            QRegularExpression param_re("\\?[^(}})]+");
            QRegularExpressionMatch param_match = param_re.match(p);
            if (param_match.hasMatch()) {
                new_param.replace(param_match.captured(0), "");
                qDebug() << new_param;
            }
        }
        else {
            if (p.contains("?")) {
                /* ファイル名に?がある場合そもそもzim側がファイル名は最初の?までと認識してしまふから
                    ifの条件はこれでよい。*/
                /* すでにパラメタが設定されてゐて上書きしない場合 */
                continue;
            }
        }
        QString thumbnail_suffix = "__thumb";
        new_param.replace(thumbnail_suffix, "");
        QString img_rp = new_param;
        img_rp.replace("{{", "");
        img_rp.replace("}}", "");
        QFileInfo img_rp_fi(img_rp);
        QString img_basename = img_rp_fi.completeBaseName();
        QString img_extension = img_rp_fi.suffix();
        qDebug() << new_param << img_rp << img_basename;
        QString source_dir_ap = QFileInfo(source).path() + "/" + QFileInfo(source).completeBaseName() + "/";
        QString thumb_ap = source_dir_ap + img_basename + thumbnail_suffix + "." + img_extension;
        QString thumb_rp = "./" + img_basename + thumbnail_suffix + "." + img_extension;
        QString img_ap = source_dir_ap + img_rp_fi.fileName();

        QString adding_params = "?href=" + img_rp;
        if (generate_thumbnail_chkbox->checkState() == Qt::Checked) {
            QImage img(img_ap);
            if (img.isNull()) {
                qDebug() << img_ap << "is null.";
                QMessageBox::critical(this, "FATAL ERROR", img_ap + " is null.");
                exit(1);
            }
            if (w_chkbox->checkState() == Qt::Checked
             && h_chkbox->checkState() == Qt::Checked) {
                img = img.scaled(w_le->text().toInt(), h_le->text().toInt(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }
            else if (w_chkbox->checkState() != Qt::Checked
                  && h_chkbox->checkState() == Qt::Checked) {
                img = img.scaledToHeight(h_le->text().toInt(), Qt::SmoothTransformation);
            }
            else if (w_chkbox->checkState() == Qt::Checked
                  && h_chkbox->checkState() != Qt::Checked) {
                img = img.scaledToWidth(w_le->text().toInt(), Qt::SmoothTransformation);
            }

            img.save(thumb_ap);
            new_param.replace(img_rp, thumb_rp);
        }
        else {
            if (w_chkbox->checkState() == Qt::Checked) {
                adding_params += ("&width=" + w_le->text());

            }
            if (h_chkbox->checkState() == Qt::Checked) {
                adding_params += ("&height=" + h_le->text());
            }
        }
        new_param += adding_params;
        new_text.replace(p, new_param);
    }

    if (!source.open(QIODevice::WriteOnly)) {
        qDebug() << "zimのソースファイルを書き込み用で開けなかった。";
        QMessageBox::critical(this, tr("致命的エラー"), tr("zimのソースファイルを書き込み用で開けなかった。"));
        exit(1);
    }
    QTextStream out(&source);
    out << new_text;
    source.close();

/*
    char new_txt[TXT_MAX_SIZE];
    new_txt[0] = '\0';
    int copy_start_index = 0;
    for (int i = 0; i+2 < TXT_MAX_SIZE; i++) {
        if (txt[i] == '{'
        && txt[i+1] == '{'
        && txt[i+2] != '{') {
        int img_start_index = i;
        int img_end_index = -1;
        for (int j = i+2; j+2 < TXT_MAX_SIZE; j++) {
            if (txt[j] == '}'
             && txt[j+1] == '}'
             && txt[j+2] != '}') {
            img_end_index = j+1;
            break;
            }
        }
        if (img_end_index == -1) {
            qDebug() << "Double brace is not closed.\n";
            exit(1);
        }
        // }}から{{の前までをnew_txtに連結
        strncat(new_txt, &txt[copy_start_index], img_start_index - copy_start_index);
        // 次回は1つ目の}からnew_txtに連結する
        copy_start_index = img_end_index - 1;
        // {{から}}の前までの文字数
        int n = img_end_index - img_start_index - 1;
        if (n >= BUF_SIZE) {
            qDebug() << "%d, %d, URI of image is too long.\n", img_end_index, img_start_index;
            exit(1);
        }
        strncpy(buf, &txt[i], n);
        buf[n] = '\0';
        strcat(new_txt, buf);

        char href_prefix[] = "?href_prefix=";
        char width_prefix[] = "&width_prefix=";
        char height_prefix[] = "&height_prefix=";
        char *href_match_ptr = strstr(buf, href_prefix);
        char *width_match_ptr = strstr(buf, width_prefix);
        char *height_match_ptr = strstr(buf, height_prefix);
        if (href_match_ptr == NULL
         && width_match_ptr == NULL
         && height_match_ptr == NULL) {
            // 画像が貼られた直後で何も設定されてない状態
            // 相対ファイル場所
            char file_rp[BUF_SIZE];
            strncpy(file_rp, &buf[2], n-2);
            file_rp[n-2] = '\0';
            char hrefwidth[BUF_SIZE];
            strcpy(hrefwidth, href_prefix);
            strcat(hrefwidth, file_rp);
            strcat(hrefwidth, width_prefix);
            strcat(hrefwidth, "300");
            strcat(new_txt, hrefwidth);
        }
        //printf("%s\n", buf);
        }
    }
    if (copy_start_index < TXT_MAX_SIZE) {
        strcat(new_txt, &txt[copy_start_index]);
    }
    //printf("%s", new_txt);

    fp = fopen(path, "w");
    if (fp == NULL) {
        qDebug() << "failed to open file.\n";
        exit(1);
    }
    fprintf(fp, "%s", new_txt);
    fclose(fp);
    */
}
