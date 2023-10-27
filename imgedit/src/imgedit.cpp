#include "imgedit.hpp"

imgedit::imgedit(QWidget *parent)
    : QWidget(parent)
{
    argv = QCoreApplication::arguments();
    argc = argv.size();

    qDebug() << argc << argv;
    if (argc <= 1) {
        qDebug() << "引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)";
        QMessageBox::critical(this, tr("致命的エラー") + QString::number(argc), tr("引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)"));
        exit(1);
    }

    if (3 <= argc && argc <= 5) {
        qDebug() << "引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)";
        QMessageBox::critical(this, tr("致命的エラー") + QString::number(argc), tr("引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)"));
        exit(1);
    }

    if (argc == 6) {
        bool ok[4] = {false};
        width = QString(argv[2]).toInt(&ok[0]);
        height = QString(argv[3]).toInt(&ok[1]);
        thumb = QString(argv[4]).toInt(&ok[2]);
        ovrrd = QString(argv[5]).toInt(&ok[3]);
        for (int i = 0; i < 4; i++) {
            if (!ok[i]) {
                qDebug() << "引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)";
                QMessageBox::critical(this, tr("致命的エラー 入力は全て整数値で"), tr("引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)"));
                exit(1);
            }
        }
        edit();
        exit(0);
    }

    if (argc > 6) {
        qDebug() << "引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)";
        QMessageBox::critical(this, tr("致命的エラー") + QString::number(argc), tr("引数は ページソース {横(指定しない場合は0) 縦(指定しない場合は0) サムネイル生成するか(0,1) 既存を上書きするか(0,1) }(省略可)"));
        exit(1);
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
    width = w_le->text().toInt();
    height = h_le->text().toInt();
    if (w_chkbox->checkState() != Qt::Checked) {
        width = 0;
    }
    if (h_chkbox->checkState() != Qt::Checked) {
        height = 0;
    }
    thumb = generate_thumbnail_chkbox->checkState() == Qt::Checked;
    ovrrd = override_existing_imgs_chkbox->checkState() == Qt::Checked;
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
        // アンカーは除外
        if (img_param.startsWith("{{id: ")) {
            continue;
        }
        img_params << img_param;
        qDebug() << img_param;
    }
    for (const QString p : img_params) {
        QString new_param = p;
        if (!ovrrd) {
            if (p.contains("?")) {
                /* ファイル名に?がある場合そもそもzim側がファイル名は最初の?までと認識してしまふから
                    ifの条件はこれでよい。*/
                /* すでにパラメタが設定されてゐて上書きしない場合 */
                continue;
            }
        }
        /* 正規表現を用ゐて?から}}の手前までの文字列を取得し、overrideのため取得した文字列部分を無に置換してhref情報を削除する。 */
        QRegularExpression param_re("\\?[^(}})]+");
        QRegularExpressionMatch param_match = param_re.match(p);
        if (param_match.hasMatch()) {
        new_param.replace(param_match.captured(0), "");
        qDebug() << new_param;
        }

        QString source_dir_ap = QFileInfo(source).path() + "/" + QFileInfo(source).completeBaseName() + "/";

        QString thumbnail_suffix = "__thumb";
        new_param.replace(thumbnail_suffix, "");
        QString thumbs_dir_name = "_thumbs";
        new_param.replace(thumbs_dir_name + "/", "");
        QString img_path = new_param;
        img_path.replace("{{", "");
        img_path.replace("}}", "");

        QFileInfo img_path_fi(img_path);
        QString img_basename = img_path_fi.completeBaseName();
        QString img_extension = img_path_fi.suffix();
        qDebug() << new_param << img_path << img_basename;
        QString img_ap = source_dir_ap + img_path_fi.fileName();
        if (!img_path.startsWith("./")) {
            // チルダによるホームフォルダ表記はQtでは使へないから置換
            QString img_path_tilde_replaced = img_path;
            img_path_tilde_replaced.replace("~/", QDir::homePath() + "/");
            if (QDir(source_dir_ap).exists() == false) {
                if (QDir().mkdir(source_dir_ap) == false) {
                    QMessageBox::critical(this, "FATAL ERROR", " " + QString(__FILE__) + ":" + QString::number(__LINE__) + " QDir().mkdir(thumbs_dir_ap)");
                }
            }
            if (QFile::copy(img_path_tilde_replaced, img_ap) == false) {
                QMessageBox::critical(this, "FATAL ERROR", "copy " + img_path + " to " + img_ap + " failed or already exists.");
            }
        }
        QString img_rp = img_ap;
        img_rp.replace(source_dir_ap, "./");

        QString adding_params = "?href=" + img_rp;
        if (thumb) {
            QString thumbs_dir_ap = source_dir_ap + thumbs_dir_name + "/";
            if (QDir(thumbs_dir_ap).exists() == false) {
                if (QDir().mkdir(thumbs_dir_ap) == false) {
                    QMessageBox::critical(this, "FATAL ERROR", " " + QString(__FILE__) + ":" + QString::number(__LINE__) + " QDir().mkdir(thumbs_dir_ap)");
                }
            }
            QString thumb_ap = thumbs_dir_ap + img_basename + thumbnail_suffix + "." + img_extension;
            QString thumb_rp = thumb_ap;
            thumb_rp.replace(source_dir_ap, "./");

            new_param.replace(img_rp, thumb_rp);
            new_param.replace(img_path, thumb_rp);

            QImage img(img_ap);
            if (img.isNull()) {
                qDebug() << img_ap << "is null.";
                QMessageBox::critical(this, "FATAL ERROR", img_ap + " is null.");
                exit(1);
            }
            if (width != 0 && height != 0) {
                img = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }
            else if (width == 0 && height != 0) {
                img = img.scaledToHeight(height, Qt::SmoothTransformation);
            }
            else if (width != 0 && height == 0) {
                img = img.scaledToWidth(width, Qt::SmoothTransformation);
            }

            img.save(thumb_ap);
        }
        else {
            if (width != 0) {
                adding_params += ("&width=" + QString::number(width));

            }
            if (height != 0) {
                adding_params += ("&height=" + QString::number(height));
            }

            new_param.replace(img_path, img_rp);
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
