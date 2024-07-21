#ifndef MD5_H
#define MD5_H
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<iomanip>
using namespace std;

//����һЩ����
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define CLS(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) { a += F(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define GG(a,b,c,d,x,s,ac) { a += G(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define HH(a,b,c,d,x,s,ac) { a += H(b, c, d) + x + ac; a = CLS(a, s); a += b; }

#define II(a,b,c,d,x,s,ac) { a += I(b, c, d) + x + ac; a = CLS(a, s); a += b; }

//��亯��
//mΪԭʼ���ģ���������ݳ���Ϊ512*n bit�������������output�б���
void mExtend(vector<unsigned char>& date, int& n, string m);

//���ɵ�n���X���飬��С��->��ˣ���С��ת��
void get_X(unsigned int X[], vector<unsigned char> m, int n);

void hex2hexstring(stringstream& ss, unsigned int tmp);

string md5(string inputStr);

#endif