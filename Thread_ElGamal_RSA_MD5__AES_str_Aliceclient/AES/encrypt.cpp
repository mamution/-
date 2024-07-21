#include<iostream>
#include<string>
#include"encrypt.h"

using namespace std;


//�������ϵĳ˷� GF(2^8)
//�ο�xtime()--x�˷�
Byte GF_Mul(Byte a, Byte b) {
	Byte b_tmp = 0;
	for (int i = 0;i < 8;i++) {
		//b�ĵͽ�λΪ1��ֱ�Ӹ�ֵ\��Ϊ1�ͽ������������ֵ��ֱ�ӳ�x
		if (b[0] == 1)
			b_tmp ^= a;
		//���a�ĸ߽�λ����1����������x^8
		int h = a[7];
		a <<= 1;
		//�����ü���û�ж�ʧ���ݵ����ƺ�a mod m(x)  �ȼ��ڶ�ʧ���ݺ�a ^= 0x1b
		if (h == 1)
			a ^= 0x1b;
		b >>= 1;

	}
	return b_tmp;
}


// 1 ����Կ��:��״̬�����һ�е��ĸ��ֽں�����Կ�Ķ�Ӧ�ֽڽ������   
void Cyc_Key_Add(Byte m[4 * 4], word w[4]) {
	for (int i = 0;i < 4;i++) {
		//��w���л�ȡ�ĸ��ֽ�
		word k0 = w[i] >> 24;
		word k1 = (w[i] << 8) >> 24;
		word k2 = (w[i] << 16) >> 24;
		word k3 = (w[i] << 24) >> 24;

		m[i] = m[i] ^ Byte(k0.to_ulong());
		m[i + 4] = m[i + 4] ^ Byte(k1.to_ulong());
		m[i + 8] = m[i + 8] ^ Byte(k2.to_ulong());
		m[i + 12] = m[i + 12] ^ Byte(k3.to_ulong());
	}

}

// 2 �ֽڴ���
void SubBytes(Byte m[4 * 4]) {
	// ��16���ֽ����ν��д���
	for (int i = 0;i < 16;i++) {
		int row = m[i][7] * 8 + m[i][6] * 4 + m[i][5] * 2 + m[i][4] * 1;
		int col = m[i][3] * 8 + m[i][2] * 4 + m[i][1] * 2 + m[i][0] * 1;
		m[i] = S[row * 16 + col];
	}
}

// 3 ����λ : ���н����ֽ���λ
void ShiftRow(Byte m[4 * 4]) {
	Byte b_tmp[4];
	for (int i = 0;i < 4;i++) {
		//����Ҫ�ƶ����ֽڴ洢����
		for (int j = 0;j < i;j++) {
			b_tmp[j] = m[i * 4 + j];
		}
		//����Ҫ��ǰ�ƶ��ı���
		for (int j = 0;j < 4 - i;j++) {
			m[i * 4 + j] = m[i * 4 + j + i];
		}
		//�ݴ�����Ż�״̬�������� 
		for (int k = 4 - i;k < 4;k++) {
			m[i * 4 + k] = b_tmp[k + i - 4];
		}
	}
}

// 4 �л��
void MixColumns(Byte m[4 * 4], Byte C[4 * 4]) {
	Byte matr[4];
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 4;j++)
			matr[j] = m[i + j * 4];//��m�е�����ȡ����
		//������г˷����㣬�ٴ洢��ԭ��������
		m[i] = GF_Mul(C[0], matr[0]) ^ GF_Mul(C[1], matr[1]) ^ GF_Mul(C[2], matr[2]) ^ GF_Mul(C[3], matr[3]);
		m[i + 4] = GF_Mul(C[4], matr[0]) ^ GF_Mul(C[5], matr[1]) ^ GF_Mul(C[6], matr[2]) ^ GF_Mul(C[7], matr[3]);
		m[i + 8] = GF_Mul(C[8], matr[0]) ^ GF_Mul(C[9], matr[1]) ^ GF_Mul(C[10], matr[2]) ^ GF_Mul(C[11], matr[3]);
		m[i + 12] = GF_Mul(C[12], matr[0]) ^ GF_Mul(C[13], matr[1]) ^ GF_Mul(C[14], matr[2]) ^ GF_Mul(C[15], matr[3]);
	}
}

// 5 ���ܺ���
void encrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]) {
	word key[4];
	for (int i = 0; i < 4; i++)
		key[i] = w[i];
	//�Ƚ���һ������Կ�� 
	Cyc_Key_Add(m, key);

	//ǰ���֣�   S��  ����λ  �л��  ����Կ�� 
	for (int r = 1; r < Nr; r++)
	{
		SubBytes(m);
		ShiftRow(m);
		MixColumns(m, C);
		for (int i = 0; i < 4; i++)
			key[i] = w[4 * r + i];
		Cyc_Key_Add(m, key);
  
	}
	//��ʮ��   S��  ����λ  ����Կ�� 
	SubBytes(m);
	ShiftRow(m);
	for (int i = 0; i < 4; ++i)
		key[i] = w[4 * Nr + i];
	Cyc_Key_Add(m, key);
}