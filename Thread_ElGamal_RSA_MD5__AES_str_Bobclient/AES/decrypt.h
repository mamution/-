#ifndef DECRYPT_H
#define DECRYPT_H
#include"encrypt.h"


using namespace std;

// 1 ���б任
void ShiftRow_(Byte m[4 * 4]);

// 2 ��S�б任
void SubBytes_(Byte m[4 * 4]);

// 3 ���б任
void MixColumns_(Byte m[4 * 4]);

// 4 ���ܺ���
void decrypt(Byte in[4 * 4], word w[4 * (Nr + 1)]);

#endif