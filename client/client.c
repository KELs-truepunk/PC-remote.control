#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define PORT 8080
#define BUF_SIZE 1024
#define SERVER "127.0.0.1"
typedef struct sockaddr_in sockaddr_in; //база сокета

// Структура для передачи данных в кал-бэк кнопок
typedef struct {
    int sock;
    GtkWidget *label;
} AppData;

//кал-бэк кнопки для выключения компа в локальной сети
static void button_send_shutdown(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;//извлекаем из универсального указателя указатель на структуру
    char *msg = "shutdown";//сообщение для отправки
    char buffer[BUF_SIZE] = {0};//TODO: выделить из кучи (malloc или g_malloc)
    memset(buffer, 0, BUF_SIZE);//Обнуляем буфер

    g_print("Sending data...\n");

    // 1. Отправка
    send(app_data->sock, msg, strlen(msg), 0);

    // 2. Ждем ответ от сервера (блокирующий вызов)
    int bytes_received = recv(app_data->sock, buffer, sizeof(buffer) - 1, 0); //TODO: сделать неблокирующим

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Гарантируем конец строки, тк может и не быть
        // Обновляем текст в интерфейсе
        gtk_label_set_text(GTK_LABEL(app_data->label), buffer);
    } else {
        gtk_label_set_text(GTK_LABEL(app_data->label), "500: Server error");
    }
}
static void button_send_reboot(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    char *msg = "reboot";
    char buffer[BUF_SIZE] = {0};
    memset(buffer, 0, BUF_SIZE);
    g_print("Sending data...\n");
    send(app_data->sock, msg, strlen(msg), 0);
    int bytes_received = recv(app_data->sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        gtk_label_set_text(GTK_LABEL(app_data->label), buffer);
    } else {
        gtk_label_set_text(GTK_LABEL(app_data->label), "500: Server error");
    }
}
static void button_send_stop(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    char *msg = "stop";
    char buffer[BUF_SIZE] = {0};
    memset(buffer, 0, BUF_SIZE);
    g_print("Sending data...\n");
    send(app_data->sock, msg, strlen(msg), 0);
    int bytes_received = recv(app_data->sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        gtk_label_set_text(GTK_LABEL(app_data->label), buffer);
    }else {
        gtk_label_set_text(GTK_LABEL(app_data->label), "500: Server error");
    }
}
static void button_send_clear(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    gtk_label_set_text(GTK_LABEL(app_data->label), "");
}

static void app_activate(GtkApplication *app, gpointer user_data) {
    // Выделяем память под данные приложения
    AppData *app_data = g_malloc(sizeof(AppData));

    // Socket init
    app_data->sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server;//экземпляр сервера
    server.sin_family = AF_INET;//iP-v4
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER, &server.sin_addr);

    if (connect(app_data->sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        g_print("Connection failed\n");
    }

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Socket Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *shutdow_button = gtk_button_new_with_label("Shut down");
    GtkWidget *reboot_button = gtk_button_new_with_label("Reboot");
    GtkWidget *stop_button = gtk_button_new_with_label("Stop server");
    GtkWidget *clear_button = gtk_button_new_with_label("Clear");

    app_data->label = gtk_label_new("");

    // Передаем структуру app_data в callback
    g_signal_connect(shutdow_button, "clicked", G_CALLBACK(button_send_shutdown), app_data);
    g_signal_connect(reboot_button, "clicked", G_CALLBACK(button_send_reboot), app_data);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(button_send_stop), app_data);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(button_send_clear), app_data);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append(GTK_BOX(box), shutdow_button);
    gtk_box_append(GTK_BOX(box), reboot_button);
    gtk_box_append(GTK_BOX(box), stop_button);
    gtk_box_append(GTK_BOX(box), clear_button);

    gtk_box_append(GTK_BOX(box), app_data->label);
    gtk_window_set_child(GTK_WINDOW(window), box);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("io.kels.test", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
