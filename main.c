#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct form {
    char name[30];
    char surname[30];
    int age;
    int workExpYears;
} form_type;

void setFlockState(struct flock *fl, short l_type, short l_whence, __off_t l_start, __off_t l_len) {
    fl->l_type = l_type;
    fl->l_whence = l_whence;
    fl->l_start = l_start;
    fl->l_len = l_len;
}

int openFormForRead(int form_number) {
    int my_file;
    struct flock fl;
    form_type f;
    my_file = open("forms.bin", O_RDWR, S_IRWXU | S_IRGRP | S_IROTH);
    if (my_file == -1) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    memset(&fl, 0, sizeof(fl));
    setFlockState(&fl, F_RDLCK, SEEK_SET, 0,
                  sizeof(form_type));

    fcntl(my_file, F_SETLKW, &fl);
    lseek(my_file, sizeof(form_type) * form_number, SEEK_SET);
    if (read(my_file, &f, sizeof(form_type)) == -1) {
        printf("%s\n", strerror(errno));
        return 2;
    }
    printf("Имя: %s\n", f.name);
    printf("Фамилия: %s\n", f.surname);
    printf("Возраст: %d\n", f.age);
    printf("Опыт работы (в годах): %d\n", f.workExpYears);

    char temp;
    scanf("%c", &temp);
    close(my_file);
    return 0;
}

int openFormForWrite(int form_number, form_type form) {
    int my_file;
    struct flock fl;
    my_file = open("forms.bin", O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    if (my_file == -1) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    setFlockState(&fl, F_WRLCK, SEEK_SET, 0,
                  sizeof(form));
    fcntl(my_file, F_SETLKW, &fl);

    char name[30];
    printf("Введите ваше имя: ");
    scanf("%s", name);
    strcpy(form.name, name);

    char surname[30];
    printf("Введите вашу фамилию: ");
    scanf("%s", surname);
    strcpy(form.surname, surname);

    int age;
    printf("Введите ваш возраст: ");
    scanf("%d", &age);
    form.age = age;

    int workExpYears;
    printf("Введите ваш опыт работы (в годах): ");
    scanf("%d", &workExpYears);
    form.workExpYears = workExpYears;

    lseek(my_file, sizeof(form_type) * form_number, SEEK_SET);
    if (write(my_file, &form, sizeof(form)) == -1) {
        printf("%s", strerror(errno));
        return 2;
    }

    char temp;
    scanf("%c", &temp);
    close(my_file);
    return 0;
}

int getOperationIndex() {
    printf("Введите номер операции, которую хотели бы выполнить:\n");
    printf("1. Чтение анкеты;\n");
    printf("2. Создание новой анкеты;\n");
    printf("3. Редактирование анкеты;\n");
    printf("Введите номер выбранной операции: ");
    int operation_index;
    scanf("%d[^\n]", &operation_index);
    return operation_index;
}

int getFormIndex() {
    printf("Введите номер анкеты, над которой будете производить операции: ");
    int formIndex;
    scanf("%d[^\n]", &formIndex);
    return formIndex - 1;
}

int main() {
    char flag = 0;

    while (1) {

        int form_index = getFormIndex();
        if (form_index < 0) {
            printf("Нет анкеты с таким номером\n");
            continue;
        }

        int operation_index = getOperationIndex();

        switch (operation_index) {
            case 1: {
                int read_result = openFormForRead(form_index);
                if (read_result == 1) {
                    printf("Не удалось открыть файл с анкетами для чтения\n");
                } else if (read_result == 2) {
                    printf("Не удалось прочитать анкету из файла\n");
                }
                break;
            }
            case 2: {
                form_type f = {};
                int write_result = openFormForWrite(form_index, f);
                if (write_result == 1) {
                    printf("Не удалось открыть файл с анкетами для записи\n");
                } else if (write_result == 2) {
                    printf("Не удалось записать анкету в файл\n");
                }
                break;
            }
            case 3: {
                form_type f = {};
                int write_result = openFormForWrite(form_index, f);
                if (write_result == 1) {
                    printf("Не удалось открыть файл с анкетами для записи\n");
                } else if (write_result == 2) {
                    printf("Не удалось записать анкету в файл\n");
                }
                break;
            }
            default:
                flag = 1;
                break;
        }
        if (flag) {
            break;
        }
    }

    return 0;
}