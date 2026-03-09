#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct sockaddr_in addr;

int main() {
    int server_fd, new_socket;
    addr address;
    int opt = 1;
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Опция, чтобы порт сразу освобождался после перезапуска сервера
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Сервер запущен. Ожидание команд...\n");

    while(1) { // БЕСКОНЕЧНЫЙ ЦИКЛ
        new_socket = accept(server_fd, NULL, NULL);
        
        memset(buffer, 0, 1024);
        int valread = read(new_socket, buffer, 1024);
        
        if (valread > 0) {
            // Убираем лишние символы переноса строки
            buffer[strcspn(buffer, "\r\n")] = 0;
            printf("Получено: [%s]\n", buffer);

            if (strcmp(buffer, "shutdown") == 0) {
                printf("Выполняю: shutdown\n");
                send(new_socket, "200: Shutdown initiated", 23, 0); // Отправляем код
                system("shutdown -h now");
            } 
            else if (strcmp(buffer, "stop") == 0) {
                printf("Завершение работы сервера...\n");
                send(new_socket, "0: Server stopped", 19, 0);
                close(new_socket);
                break; // Выход из цикла while(1)
            }else if (strcmp(buffer, "reboot") == 0) {
                system("reboot");
                send(new_socket, "201: Reboot initiated", 23, 0);
            }else{
                send(new_socket, "-1: Command not found",23,0);
            }
        }
        close(new_socket); // Закрываем клиентский сокет, но серверный живет
    }

    close(server_fd);
    return 0;
}
