#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define RSA_Public_Key "This is the public key of RSA"
#define AES_Key "This is the key of AES"
#define AES_Key_Successfully_Accepted "The AES key is accepted successfully. You can send messages"
#define PORT 55313
#define PORT_CA 55319
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<iostream>
#include<cstring>
#include<bitset>
#include<string>
#include<vector>
#include<thread>
#include<ctime>
#include<fstream>
#include<regex>
#include<windows.h>
#include<memory>

#define THREAD_NUM 10

#include"encrypt.h"
#include"decrypt.h"
#include"ThreadPool.h"
#include"md5.h"
#include"BigInt.h"
#include"Rsa.h"

using namespace std;

Rsa rsa;
bool isAESCommunication = false;
mutex mtx;
Byte key[16];
word w[4 * (Nr + 1)];
void myEncrypt(Byte m[], word w[]) {
	encrypt(m, w);//����
}
void myDecrypt(Byte e[], int L, int num, string* m_v, word w[]) {
	decrypt(e, w);//����
	int k = 0;
	int count = 16;
	if (num == (L - 1)) {
		count -= e[15].to_ulong();
	}
	while (k < count)
	{
		*(m_v) += ((char)e[k].to_ulong());
		k++;
	}
}
void init_createPQ(Rsa& rsa, int n)
{//��ʼ��
	cout << "RSA��Կ��ʼ��(ͨ���㷨�õ�����)...." << endl;
	long t1 = clock();
	rsa.init(n);
	long t2 = clock();
	cout << "��ʼ�����." << endl;
	cout << rsa << endl;
	cout << "RSA��Կ��ʼ����ʱ:" << (t2 - t1) / 1000 << "s." << endl << endl;
}
void init_inputPQ(Rsa& rsa)
{//��ʼ��
	cout << "RSA��Կ��ʼ��(���ļ���ȡ����)...." << endl;
	long t1 = clock();
	fstream file;
	file.open("twoPrime768.txt", ios::in);
	string buf_P, buf_Q;
	getline(file, buf_P);
	getline(file, buf_Q);
	file.close();
	rsa.init(buf_P, buf_Q);
	long t2 = clock();
	cout << "��ʼ�����." << endl;
	cout << rsa << endl;
	cout << "RSA��Կ��ʼ����ʱ:" << (t2 - t1) / 1000 << "s." << endl << endl;
}
bool islegal(const string& str)
{//�ж������Ƿ�Ϸ�
	for (string::const_iterator it = str.begin();it != str.end();++it)
		if (!isalnum(*it))//������ĸ����
			return false;
	return true;
}

void RSAdecode(Rsa& rsa, BigInt str, string& m)
{//RSA����
	cout << "RSA����AES��Key:" << endl;
	BigInt c(str);
	long t1 = clock();
	m = rsa.decodeByPr(c).toHexString();
	long t2 = clock();
	cout << "����:" << c << endl
		<< "����:" << m << endl;
	cout << "RSA������ʱ:" << (t2 - t1) << "ms." << endl << endl;
}

void RSAencry(Rsa& rsa, BigInt& c, string str)
{//RSA����
	cout << "RSA����AES��Key:" << endl;
	BigInt m(str);
	long t1 = clock();
	c = rsa.encryptByPu(m);
	long t2 = clock();
	cout << "����:" << m << endl
		<< "����:" << c << endl;
	cout << "RSA������ʱ:" << (t2 - t1) << "ms." << endl << endl;
}
string byte2string(Byte key[16]) {
	stringstream Hex_s;
	for (int i = 0;i < 16;i++) {
		Hex_s << setw(2) << setfill('0') << hex << key[i].to_ulong();
	}
	return Hex_s.str();
}
void string2byte(string key_str, Byte key_m[16]) {
	for (int i = 0; i < 16; ++i) {
		string byteString = key_str.substr(i * 2, 2);
		key_m[i] = std::bitset<8>(std::stoi(byteString, nullptr, 16));
	}
}

//�̺߳���
DWORD WINAPI clientReceiverThread(LPVOID lpParamter);
char target_username[100];
char username[100];
string p, g, y, r, s;

int main() {
	char buff[32768];
	char message[32768];

	//��ʼ������
	WSADATA data;
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret) {
		cout << "��ʼ���������" << endl;
		WSACleanup();
		return -1;
	}

	//�����׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "�����׽���ʧ�ܣ�" << endl;
		WSACleanup();
		return -1;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
	addr.sin_port = htons(PORT);


	//����
	ret = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		WSACleanup();
		cout << "���ӷ�����ʧ��" << endl;
		return -1;
	}

	//�����û�����server
	memset(username, 0, sizeof(username));
	cout << "1�������û�����";
	cin >> username;
	getchar();
	memset(buff, 0, sizeof(buff));
	strcpy(buff, username);
	send(sock, buff, strlen(buff), 0);
	//����Ŀ���û���
	memset(target_username, 0, sizeof(target_username));
	cout << "3������Ŀ���û�����";
	cin >> target_username;
	getchar();
	memset(buff, 0, sizeof(buff));
	strcpy(buff, target_username);
	send(sock, buff, strlen(buff), 0);


	//����һ���߳����ڲ�ͣ���շ���˷�������Ϣ
	HANDLE thread = CreateThread(NULL, 0, clientReceiverThread, reinterpret_cast<LPVOID>(&sock), 0, NULL);
	Byte** m = NULL;
	int num = 0;
	while (1) {
		unique_lock<mutex> lock(mtx);
		if (isAESCommunication == false)
			continue;
		//���������
		cout << endl << "-------------------------------����AES���ܵ���Ϣ-----------------------------------" << endl;
		string M;
		cout << "--�����뷢�͵����ģ������ģ�:" << endl;//��ȡһ��
		getline(cin, M);
		cout << endl;
		string md5Str = md5(M);
		cout << "--���ĵ�MD5Ϊ:" << endl << md5Str << endl << endl;//��ȡһ��
		M += md5Str;
		//pkcs7���뷨
		int is = M.size() % 16;
		{
			string tmp;
			tmp.assign((16 - M.size() % 16), (char)(16 - M.size() % 16));
			M += tmp;
		}
		int L = M.size() / 16;

		num = 0;
		m = new Byte * [L];
		string E = "";
		{
			ThreadPool myEncryptPool(THREAD_NUM);
			while (num < L) {
				m[num] = new Byte[16];
				for (size_t i = 0; i < 16; i++)
				{
					m[num][i] = M[num * 16 + i];
				}
				myEncryptPool.enqueue(myEncrypt, m[num], w);
				num++;
			}
		}
		for (size_t i = 0; i < L; i++)
		{
			E += (byte2string(m[i]));
		}
		cout << "--����ͨ��AES-128���ܺ��������:" << endl;
		cout << E << endl;
		cout << endl;
		for (size_t i = 0; i < L; i++)
		{
			delete[] m[i];
		}
		delete[] m;
		m = NULL;
		//������Ϣ
		memset(buff, 0, sizeof(buff));
		strcpy(buff, E.data());
		send(sock, buff, strlen(buff), 0);
		cout << "������Ϣ�ɹ�" << endl;
	}
	closesocket(sock);
	WSACleanup();
}

DWORD WINAPI clientReceiverThread(LPVOID lpParamter) {
	SOCKET* connfd0 = reinterpret_cast<SOCKET*>(lpParamter);
	SOCKET connfd = *(connfd0);
	char buff[32768];
	string** m_v = NULL;
	int num = 0;
	Byte** e = NULL;


	while (1) {
		memset(buff, 0, sizeof(buff));
		int ret = recv(connfd, buff, sizeof(buff) - 1, 0);
		Sleep(500);
		if (ret > 0) {
			buff[ret] = '\0';
			string substr(buff);
			if (strcmp(buff, RSA_Public_Key) == 0) {
				memset(buff, 0, sizeof(buff));
				int ret = recv(connfd, buff, sizeof(buff) - 1, 0);
				buff[ret] = '\0';
				string str(buff);
				cout <<endl<< "-----------------------------------�յ�RSA��Կ��Ϣ-------------------------------------" << endl;
				regex reg(",");
				sregex_token_iterator iter(str.begin(), str.end(), reg, -1);
				rsa.e = iter->str();
				cout << "RSA.e��" << rsa.e << endl;
				iter++;
				rsa.N = iter->str();
				cout << "RSA.N��" << rsa.N << endl;
				{
					cout << endl << "------------------------------��Trent(CA)��ȡ�Է�������ǩ��--------------------------" << endl;
					//��ʼ������
					WSADATA data_ca;
					int ret_ca = WSAStartup(MAKEWORD(2, 2), &data_ca);
					if (ret_ca) {
						cout << "��ʼ���������" << endl;
						WSACleanup();
						return -1;
					}

					//����������ǩ��serverͨ�ŵ��׽���
					SOCKET sock_ca = socket(AF_INET, SOCK_STREAM, 0);
					if (sock_ca == INVALID_SOCKET) {
						cerr << "�����׽���ʧ�ܣ�" << endl;
						WSACleanup();
						return -1;
					}
					sockaddr_in addr_ca;
					addr_ca.sin_family = AF_INET;
					addr_ca.sin_addr.S_un.S_addr = inet_addr("172.31.118.155");
					addr_ca.sin_port = htons(PORT_CA);

					ret_ca = connect(sock_ca, (sockaddr*)&addr_ca, sizeof(addr_ca));
					if (ret_ca == -1) {
						WSACleanup();
						cout << "���ӷ�����ʧ��" << endl;
						return -1;
					}
					cout << "--��������ǩ��Server Trent(CA)�ɹ�" << endl;
					// �����û�����Trent
					// ���Ӻ����̷����û�������UserID(�������)
					memset(buff, 0, sizeof(buff));
					strcpy(buff, username);
					send(sock_ca, buff, strlen(buff), 0);
					Sleep(500);

					// ������ʾ��Ϣ
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						cout << buff << "\n\n";
					}

					// ��������֤��
					cout << "--��ȡRSA��Կ����֤��:\n";
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						cout << buff << "\n\n";
					}

					// ����ElGamal��Կ
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						p = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						g = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						y = buff;
					}
					Sleep(100);
					// ����ǩ��
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						r = buff;
					}
					Sleep(100);
					memset(buff, 0, sizeof(buff));
					ret_ca = recv(sock_ca, buff, sizeof(buff), 0);
					if (ret_ca > 0) {
						buff[ret_ca] = 0;
						s = buff;
					}
					cout << "--����ǩ��Server Trent(CA)��Bob�������ѹر�����\n";
					closesocket(sock_ca);
				}
				// ��֤ǩ��
				cout <<endl<< "-----------------------------------��������ǩ����֤-------------------------------------" << endl;
				// v1 = pow(y, r, p) * pow(r, s, p) % p
				// v2 = pow(g, m, p)
				string M = str + target_username;  // �û�id��Ϊ��ֵ
				string md5_M = md5(M);
				// cout << p << endl << g << endl << y << endl << r << endl << s << endl << md5_M<< endl;
				BigInt pp(p), gg(g), yy(y), rr(r), ss(s), MM(md5_M);
				BigInt v1 = (yy.moden(rr, pp) * rr.moden(ss, pp)) % pp;  //moden(exp, mod)ģ������
				BigInt v2 = gg.moden(MM, pp);
				cout << "--����õ�V1��V2���ж��Ƿ����" << endl;
				cout << "v1: \n" << v1 << endl;
				cout << "v2: \n" << v2 << endl;
				if (v1 == v2)
					cout << "\n����ǩ����֤�ɹ�\n";
				else
					cout << "\n����ǩ����֤ʧ��\n";
				cout << endl << "------------------------------------AES��Կ��ʼ��-----------------------------------" << endl;
				cout << "--������AES_128��������Կ��16���ƣ���:139A81AC71296CE688112233AACDFF10��" << endl;
				string KEY_STRING;
				cin >> KEY_STRING;
				getchar();
				string2byte(KEY_STRING, key);
				cout << endl << "--------------------------------����RSA���ܵ�AES��Կ--------------------------------" << endl;
				cout << "--RSA����ǰ��AES������Կ��:" << endl;
				for (int i = 0;i < 16;i++) {
					cout << hex << key[i].to_ulong() << "  ";
					if ((i + 1) % 4 == 0)
						cout << endl;
				}
				cout << endl;
				BigInt key_c_str;
				KeyExpansion(key, w); // ��Կ��չ
				RSAencry(rsa, key_c_str, byte2string(key));
				memset(buff, 0, sizeof(buff));
				strcpy(buff, AES_Key);
				send(connfd, buff, strlen(buff), 0);
				Sleep(1000);
				memset(buff, 0, sizeof(buff));
				strcpy(buff, key_c_str.toHexString().data());
				send(connfd, buff, strlen(buff), 0);
				Sleep(1000);
			}
			else if (strcmp(buff, AES_Key_Successfully_Accepted) == 0) {
				cout << endl << "----------------------------�յ�AES��Կ��ȷ�Ͻ��ܵ���Ϣ----------------------------" << endl;
				memset(buff, 0, sizeof(buff));
				strcpy(buff, AES_Key_Successfully_Accepted);
				send(connfd, buff, strlen(buff), 0);
				cout << "--���ʹ���Ϣ��ȷ����Ϣ" << endl;
				unique_lock<mutex> lock(mtx);
				isAESCommunication = true;
				cout << "--���Խ���AES����ͨ����" << endl;
			}
			else {
				cout << endl << "---------------------------------�յ�AES���ܵ���Ϣ-------------------------------------" << endl;
				string str_E(buff);
				cout << "--�յ��ı�AES���ܺ������Ϊ:" << endl << str_E << endl << endl;
				int L = str_E.size() / 32;
				string M_Tmp = "";
				m_v = new string * [L];
				num = 0;
				e = new Byte * [L];
				{
					ThreadPool myDecryptPool(THREAD_NUM);
					while (num < L)
					{
						e[num] = new Byte[16];
						string2byte(str_E.substr(num * 32, 32), e[num]);
						m_v[num] = new string("");
						myDecryptPool.enqueue(myDecrypt, e[num], L, num, m_v[num], w);
						num++;
					}
				}
				for (size_t i = 0; i < L; i++)
				{
					M_Tmp += *(m_v[i]);
				}
				int m_len = M_Tmp.length() - 32;
				cout << "--����AES���ܺ��������:" << endl;
				cout << M_Tmp << endl << endl;
				string M_Tmp_Del_Md5 = M_Tmp.substr(0, m_len);
				string New_Md5 = M_Tmp.substr(m_len, 32);
				cout << "--����(������MD5)��:" << endl;
				cout << M_Tmp_Del_Md5 << endl << endl;
				cout << "--���ļ���õ���MD5��:" << endl;
				string MD5 = md5(M_Tmp_Del_Md5);
				cout << MD5 << endl << endl;
				cout << "--������Ϣ��֤" << endl;
				if (MD5.compare(New_Md5) == 0)
					cout << "--md5��֤�ɹ�\n\n";
				else
					cout << "--md5��֤ʧ��\n\n";

				for (size_t i = 0; i < L; i++)
				{
					delete[] e[i];
				}
				delete[] m_v;
				delete[] e;
				m_v = NULL;
				e = NULL;
			}
		}
		else if (ret == 0) {
			cout << "�������ѹر�����" << endl;
			closesocket(connfd);
			return 0;
		}
		else {
			cout << "�������ѹر�����" << endl;
			closesocket(connfd);
			WSACleanup();
			return 0;
		}
	}

	closesocket(connfd);
	return 0;
}
