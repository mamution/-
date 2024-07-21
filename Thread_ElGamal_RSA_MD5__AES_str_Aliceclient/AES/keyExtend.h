#ifndef KEYEXTEND_H
#define KEYEXTEND_H
//��Կ��չ
#include<bitset>

using namespace std;

typedef bitset<8> Byte;//�ֽڣ�8bit��
typedef bitset<32> word;//�֣�32bit��=4�ֽ�

extern Byte S[256];//S��
extern Byte S_[256];//S'��
extern word Rcon[10];
extern Byte C[4 * 4];
extern Byte C_[4 * 4];

//����������Կ�����Լ���չ����
const int Nr = 10;//���� 
const int Nk = 4; //������Կ���ĸ��� 

//��Կ��չ ��غ���
// 1 �ĸ��ֽ�ת����һ���� 
word GetWord(Byte b0, Byte b1, Byte b2, Byte b3);
// 2 ����λ 
word WordMove(word rw);
// 3 S���滻 
word SubWord(word sw);
// 4 ��Կ��չ 
void KeyExpansion(Byte key[4 * Nk], word w[4 * (Nr + 1)]);
// ����ʹ�õ��л������


#endif