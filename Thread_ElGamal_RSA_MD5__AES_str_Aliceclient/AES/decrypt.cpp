#include<iostream>
#include<string>
#include"decrypt.h"


using namespace std;

// 1 ���б任  ѭ������
void ShiftRow_(Byte m[4 * 4]) {
	Byte b_tmp[4];
	for (int i = 0;i < 4;i++) {

		//���� ��ֹ������
		for (int j = 0;j < i;j++) {
			b_tmp[j] = m[i * 4 + 3 - j];
		}
		// �����ᷢ���±�����Ľ��и�ֵ 
		for (int j = 0;j < 4 - i;j++)
		{
			m[i * 4 + 3 - j] = m[i * 4 + 3 - j - i];
		}
		// ���ݴ�����Ż�״̬���� ���� 
		for (int k = 0;k < i;k++) {
			m[i * 4 + k] = b_tmp[i - k - 1];
		}
	}

}
// 2 ��S�б任
void SubBytes_(Byte m[4 * 4]) {
	// ��16���ֽ����ν��д���
	for (int i = 0;i < 16;i++) {
		int row = m[i][7] * 8 + m[i][6] * 4 + m[i][5] * 2 + m[i][4];
		int col = m[i][3] * 8 + m[i][2] * 4 + m[i][1] * 2 + m[i][0];
		m[i] = S_[row * 16 + col];
	}
}

// 3 ���б任   û�仯û���� 
void MixColumns_(Byte m[4 * 4], Byte C_[4 * 4]) {
	Byte matr[4];
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 4;j++)
			matr[j] = m[i + j * 4];

		m[i] = GF_Mul(C_[0], matr[0]) ^ GF_Mul(C_[1], matr[1]) ^ GF_Mul(C_[2], matr[2]) ^ GF_Mul(C_[3], matr[3]);
		m[i + 4] = GF_Mul(C_[4], matr[0]) ^ GF_Mul(C_[5], matr[1]) ^ GF_Mul(C_[6], matr[2]) ^ GF_Mul(C_[7], matr[3]);
		m[i + 8] = GF_Mul(C_[8], matr[0]) ^ GF_Mul(C_[9], matr[1]) ^ GF_Mul(C_[10], matr[2]) ^ GF_Mul(C_[11], matr[3]);
		m[i + 12] = GF_Mul(C_[12], matr[0]) ^ GF_Mul(C_[13], matr[1]) ^ GF_Mul(C_[14], matr[2]) ^ GF_Mul(C_[15], matr[3]);
	}
}

// 4 ���ܺ��� 
void decrypt(Byte m[4 * 4], word w[4 * (Nr + 1)]) {
	word key[4];
	for (int i = 0; i < 4; i++)
		key[i] = w[4 * Nr + i];
	//�Ƚ���һ������Կ�� 
	Cyc_Key_Add(m, key);

	//ǰ���ֲ���  ������λ   ��S��   ����Կ��  ���л�� 
	for (int r = Nr - 1; r > 0; r--)
	{
		ShiftRow_(m);
		SubBytes_(m);
		for (int i = 0; i < 4; i++)
			key[i] = w[4 * r + i];
		Cyc_Key_Add(m, key);
		MixColumns_(m, C_);
	}
	//��ʮ��   ������λ   ��S�� ����Կ�� 
	ShiftRow_(m);
	SubBytes_(m);
	for (int i = 0; i < 4; i++)
		key[i] = w[i];
	Cyc_Key_Add(m, key);
}
