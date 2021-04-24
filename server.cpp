#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <list>
#include <ctype.h>

using namespace std;
void Deal(int cfd, struct sockaddr_in addr)
{
    char cBuf[1024]{0};
    while (1)
    {
        memset(&cBuf, 0x00, sizeof(cBuf));
        int nLen = recv(cfd, cBuf, sizeof(cBuf), 0);
        if (nLen <= 0)
        {
            continue;
        }

        char *cIP = inet_ntoa(addr.sin_addr);
        short sPort = ntohs(addr.sin_port);
        printf("[%s: %d]: %s\n", cIP, sPort, cBuf);
        for (int i = 0; i < nLen; i++)
        {
            cBuf[i] = toupper(cBuf[i]);
        }

        nLen = send(cfd, cBuf, nLen, 0);
    }
    
    close(cfd);
}

int main(int argc, char* argv[])
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    int nRet = bind(lfd, (struct sockaddr *)&serv, 128);
    if (nRet == -1)
    {
        perror("bind error");
        return -1;
    }

    nRet = listen(lfd, 128);
    if (nRet == -1)
    {
        perror("listen error");
        return -1;
    }

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    while(1)
    {
        int cfd = accept(lfd, (struct sockaddr *)&client, &len);
        if (cfd < 0)
        {
            printf("有客户端连接请求失败！");
            continue;
        }

        char *cIP = inet_ntoa(client.sin_addr);
        short sPort = ntohs(client.sin_port);
        printf("[%s: %d] 成功连接服务器!\n", cIP, sPort);
        Deal(cfd, client);
    }

    close(lfd);
    return 0;
}