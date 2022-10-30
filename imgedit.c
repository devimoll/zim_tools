#include <stdio.h>
#include <string.h>

#define BUF_SIZE 256
#define TXT_MAX_SIZE 1048576
int main (int argc, char const *argv[]) {
    if (argc != 2) {
        printf("引数の数が不正です。\n");
        return 1;
    }
    FILE *fp;
    char buf[BUF_SIZE];
    char path[BUF_SIZE] = {'\0'};
    strncpy(path, argv[1], BUF_SIZE);
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("failed to open file.\n");
        return 1;
    }

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
                printf("Double brace is not closed.\n");
                return 1;
            }
            // }}から{{の前までをnew_txtに連結
            strncat(new_txt, &txt[copy_start_index], img_start_index - copy_start_index);
            // 次回は1つ目の}からnew_txtに連結する
            copy_start_index = img_end_index - 1;
            // {{から}}の前までの文字数
            int n = img_end_index - img_start_index - 1;
            if (n >= BUF_SIZE) {
                printf("%d, %d, URI of image is too long.\n", img_end_index, img_start_index);
                return 1;
            }
            strncpy(buf, &txt[i], n);
            buf[n] = '\0';
            strcat(new_txt, buf);

            char href[] = "?href=";
            char width[] = "&width=";
            char *href_match_ptr = strstr(buf, href);
            char *width_match_ptr = strstr(buf, width);
            if (href_match_ptr == NULL
                && width_match_ptr == NULL) {
                // 画像が貼られた直後で何も設定されてない状態
                // 相対ファイルパス
                char file_rp[BUF_SIZE];
                strncpy(file_rp, &buf[2], n-2);
                file_rp[n-2] = '\0';
                char hrefwidth[BUF_SIZE];
                strcpy(hrefwidth, href);
                strcat(hrefwidth, file_rp);
                strcat(hrefwidth, width);
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
        printf("failed to open file.\n");
        return 1;
    }
    fprintf(fp, "%s", new_txt);
    fclose(fp);
    return 0;
}
