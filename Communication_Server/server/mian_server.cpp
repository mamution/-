#define _WINSOCK_DEPRECATED_NO_WARNINGS//vs�����±��붨�壬�����޷�ʹ��inet_addr����
#define _CRT_SECURE_NO_WARNINGS
#define PORT 55313
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include<cstring>
using namespace std;

//���������û�����Ϣ
struct clientInfo
{
	char userName[100];
	SOCKET connfd;
	struct clientInfo* next;
};
//ȫ�ֱ���
struct clientInfo* userlist;

//�̺߳���
DWORD WINAPI threadfunc(LPVOID lpParamter);

int main() {
	//��ʼ���û��б�
	userlist = (struct clientInfo*)malloc(sizeof(struct clientInfo));
	userlist->next = NULL;
	//��ʼ�����绷��
	WSADATA data;
	int res = WSAStartup(MAKEWORD(2, 2), &data);
	if (res) {
		cout << "��ʼ���������" << endl;
		return -1;
	}
	//�����׽���
	SOCKET socketserver = socket(AF_INET, SOCK_STREAM, 0);//unix������int��
	if (socketserver == -1) {
		cout << "�����׽��ִ���";
		return -1;
	}
	//��
	sockaddr_in addr;//�����׽��ֽṹ��
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
	if (bind(socketserver, (sockaddr*)&addr, sizeof(addr)) == -1) {
		cout << "�󶨵�ַ�˿�ʧ��";
		return -1;
	}
	//����
	if (listen(socketserver, 5) == -1) {
		cout << "�����׽���ʧ��";
		return -1;
	}
	cout << "������ѳ�ʼ���ɹ������ڵȴ�����..." << endl;
	//����
	sockaddr clientAddr;
	int len = sizeof(clientAddr);
	while (1) {
		SOCKET connfd = accept(socketserver, &clientAddr, &len);
		if (connfd == -1) {
			cout << "����ʧ��" << endl;
			return -1;
		}
		//cout << "һ�����û��������ӷ���ˣ�"<<connfd<<"��" << endl;
		HANDLE thread = CreateThread(NULL, 0, threadfunc, reinterpret_cast<LPVOID>(&connfd), 0, NULL);
		//CloseHandle(thread);
	}
	closesocket(socketserver);
	//free(userlist);
	WSACleanup();
	return 0;
}


DWORD WINAPI threadfunc(LPVOID lpParamter) {
	SOCKET* connfd0 = reinterpret_cast<SOCKET*>(lpParamter);
	SOCKET connfd = *(connfd0);

	char buff[32768];
	char message[32768];
	char username[100];

	//�����û����
	struct clientInfo* cn = (struct clientInfo*)malloc(sizeof(struct clientInfo));

	//�����û��б�
	struct clientInfo* p = userlist;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = cn;

	//��ʼ���û���Ϣ
	cn->connfd = connfd;
	cn->next = NULL;
	memset(buff, 0, sizeof(buff));
	int ret = recv(connfd, buff, sizeof(buff), 0);
	if (ret > 0) {
		buff[ret] = 0x00;
	}
	strcpy(cn->userName, buff);
	cout << "һ�����û���connfd��" << cn->connfd << "����username��" << cn->userName << "��" << endl;

	memset(buff, 0, sizeof(buff));
	ret = recv(connfd, buff, sizeof(buff), 0);
	if (ret > 0) {
		buff[ret] = '\0';
	}
	memset(username, 0, sizeof(username));
	strcpy(username, buff);
	cout << "�����û���" << cn->userName << "�����͵�Ŀ���û��ɹ�" << endl;
	SOCKET tmp;
	p = userlist->next;
	while (p != NULL) {
		if (strcmp(p->userName, username) == 0) {
			tmp = p->connfd;
			break;
		}
		p = p->next;
	}
	//ѭ������
	while (1) {
		//�����û���Ҫ���͵���Ϣ
		//����Ŀ���û���
		
		memset(buff, 0, sizeof(buff));
		ret = recv(connfd, buff, sizeof(buff), 0);
		if (ret > 0) {
			buff[ret] = '\0';
		}
		strcpy(message, buff);
		cout << "�����û���" << cn->userName << "�����͵���Ϣ�ɹ�" << endl;
		//ת��
		send(tmp, message, strlen(message), 0);
		cout << "�������ʵ����Ϣ��" << cn->userName << "��" << p->userName << "��ת��" << endl;
		cout << message << endl;
	}

	closesocket(connfd);
	//	free(cn);
	return 0;
}