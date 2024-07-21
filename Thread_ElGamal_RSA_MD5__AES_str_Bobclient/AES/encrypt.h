#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "keyExtend.h"

using namespace std;

// ���G��2^8���������ϵĳ˷����� 
Byte GF_Mul(Byte a, Byte b);


// 1 ����Կ��
void Cyc_Key_Add(Byte m[4 * 4], word w[4]);

// 2 �ֽڴ���
void SubBytes(Byte m[4 * 4]);

// 3 ����λ
void ShiftRow(Byte m[4 * 4]);

// 4 �л�� 
void MixColumns(Byte m[4 * 4], Byte s[4 * 4]);

// 5 ���ܺ���
void encrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]);

#endif