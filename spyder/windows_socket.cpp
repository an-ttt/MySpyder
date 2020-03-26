#include <winsock2.h>
#include <QDebug>

int mybind(SOCKET server, const char* host, u_short port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inet_addr(host);
    addr.sin_port = htons(port);

    return bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof (addr));
}

int myconnect(SOCKET client, const char* remote_host, u_short port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inet_addr(remote_host);
    addr.sin_port = htons(port);

    return connect(client, reinterpret_cast<sockaddr*>(&addr), sizeof (addr));
}

int windows_server()
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        WSACleanup();
        return -1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        qDebug() << "error: " << WSAGetLastError();
        WSACleanup();
        return -2;
    }

    if (mybind(server, "127.0.0.1", 5344) == SOCKET_ERROR) {
        qDebug() << "error: " << WSAGetLastError();
        closesocket(server);
        WSACleanup();
        return -3;
    }

    if (listen(server, 5) == SOCKET_ERROR) {
        qDebug() << "error: " << WSAGetLastError();
        closesocket(server);
        WSACleanup();
        return -4;
    }

    while (1) {
        qDebug() << "等待客户端连接...";
        SOCKET client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET) {
            qDebug() << "acceptfailedwitherror: " << WSAGetLastError();
            closesocket(server);
            WSACleanup();
            return -5;
        }
        else {
            qDebug() << "客户端已连接";
            const char* msg = "hello";
            send(client, msg, strlen(msg)+1, 0);
            char buf[1024] = { 0 };
            int len = recv(client, buf, 1024, 0);
            if (len > 0)
                qDebug() << "来自客户端：" << buf;
            break;
        }
    }
    closesocket(server);
    WSACleanup();
}

int windows_client()
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        WSACleanup();
        return -1;
    }

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == INVALID_SOCKET) {
        qDebug() << "error: " << WSAGetLastError();
        WSACleanup();
        return -2;
    }

    if (myconnect(client, "127.0.0.1", 5344) != 0) {
        qDebug() << "error: " << WSAGetLastError();
        closesocket(client);
        WSACleanup();
        return -3;
    }
    else {
        qDebug() << "连接服务器成功";
        char buf[1024] = { 0 };
        int len = recv(client, buf, 1024, 0);
        if (len > 0)
            qDebug() << "来自服务器：" << buf;
        const char* msg = "world";
        send(client, msg, strlen(msg)+1, 0);
    }
    closesocket(client);
    WSACleanup();
}
