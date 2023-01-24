#define _CRT_SECURE_NO_WARNINGS
using namespace std; // C++ �߰����
#include <string>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <Windows.h> // GUI�� �����ϴ� ���
#define PI 3.141592
#include <malloc.h> //�޸� ���� �Լ��� ����Ҷ��� maolloc.h�� �����ؾ� �Ѵ�. 
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <io.h>

/////////////////////////
// ���� ����
/////////////////////////
int inH, inW, outH, outW;
unsigned char** inImage = NULL, ** outImage = NULL;


HWND  hwnd; // ���� ȭ��(�ܺ�, ������)
HDC hdc;    // ���� ȭ�� (����, ����)
FILE* rfp;
FILE* wfp;
char fileName[200];

/////////////////////////
// �Լ� ����� (�Լ��� �̸��� �غ�)
/////////////////////////
void openImage();  void saveImage();
void allocInMemory(); void allocOutMemory(); void freeInMemory(); void freeOutMemory();
////////ȭ���� ó��////////
void equalImage(); int getIntValue(); float getfloatValue(); void addImage();  void subImage(); void mulImage(); void divImage();
void reverseImage(); void bwImage(); void bwAvgImage(); void MidValuebwImage();
void paraCAPImage(); void paraCUPImage(); void gammaImage();
////////������ ó��////////
void leftright_mirror_Image(); void updown_mirror_Image(); void updown_leftright_mirror_Image();
void rotateImage1(); void rotateImage2();
void zoominImage(); void zoominImage2(); void zoomoutImage(); void moveImage();

/////////////////////////
// ���� �ڵ��
/////////////////////////
void printMenu() {
	puts("\n\n============= GrayScale Image Procssing (Version 1) =============\n");
	puts("0.���� 1.���� 9.����\n");
	puts("===========================ȭ���� ó��===========================\n");
	puts("A.���� B.���(����) C.��Ӱ�(����) D.���(����) E.��Ӱ�(������)\n");//ȭ���� ó��
	puts("F.���� G.��� H.���(��հ�) I.���(������)\n");//ȭ���� ó��
	puts("J.CAP(���� �� ��ü) K.CUP(��ο� �� ��ü) L.����\n");//ȭ���� ó��
	puts("===========================������ ó��===========================\n");
	puts("M.�¿� �̷���, N.���� �̷���, O.�����¿� �̷���\n");//������ ó��
	puts("P.ȸ��(������,������ �߾� X), Q.ȸ��(�����, ������ �߾� O)\n");//������ ó��
	puts("R.Ȯ��(������), S.Ȯ��(�����), T.���, U.�̵�");//������ ó��

}
int main() {
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);

	char inKey = 0;
	while (inKey != '9') {
		printMenu();
		inKey = _getch();
		system("cls");

		switch (inKey) {
		case '0': openImage(); break;
		case '1': saveImage(); break;
		case '9': break;
		case 'a':
		case 'A': equalImage(); break;
		case 'b':
		case 'B': addImage(); break;
		case 'c':
		case 'C': subImage(); break;
		case 'd':
		case 'D': mulImage(); break;
		case 'e':
		case 'E': divImage(); break;
		case 'f':
		case 'F': reverseImage(); break;
		case 'g':
		case 'G': bwImage(); break;
		case 'h':
		case 'H': bwAvgImage(); break;
		case 'i':
		case 'I': MidValuebwImage(); break;
		case 'j':
		case 'J': paraCAPImage(); break;
		case 'k':
		case 'K': paraCUPImage(); break;
		case 'l':
		case 'L': gammaImage(); break;
		case 'm':
		case 'M':leftright_mirror_Image(); break;
		case 'n':
		case 'N': updown_mirror_Image(); break;
		case 'o':
		case 'O': updown_leftright_mirror_Image(); break;
		case 'p':
		case 'P': rotateImage1(); break;
		case 'q':
		case 'Q': rotateImage2(); break;
		case 'r':
		case 'R': zoominImage(); break;
		case 's':
		case 'S': zoominImage2(); break;
		case 't':
		case 'T': zoomoutImage(); break;
		case 'u':
		case 'U':moveImage(); break;



		default: printf("�Է��� �߸��Ǿ����ϴ�. �ٽ� �Է����ּ���.");

		}
	}
	freeInMemory();
	freeOutMemory();
	printf("���α׷��� �����մϴ�.\n");

}

/////////////////////////
// �Լ� ���Ǻ� (�Լ��� ������ �ڵ�)
/////////////////////////

// ���� �Լ��� (����, ����, ���÷���)

void openImage() {
	string path = "C:\\images\\*.*";
	struct _finddata_t fd;
	intptr_t handle;
	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
		cout << "No file in directory!" << endl;
	do
	{
		cout << fd.name << endl;

	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);

	char fullName[200] = "C:\\images\\";
	char tmpName[50];
	puts("\n==========================\n");
	printf("���ϸ�-->"); //LENA256, flower512.....
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	// (�߿�!) �̹����� ���� ���̸� �˾Ƴ��� ��.
	rfp = fopen(fileName, "rb");

	if (rfp == NULL) {
		printf("���ϸ��� ��ġ���� �ʽ��ϴ�. �ٽ� �Է��� �ּ���.");
		return;
	}//������ �б� ���� ���� ���� rfp�� null�̸� return �ٽ� ó������ --> ��ȭ 1 ���ϸ� Ʋ�� �� �߻��Ǵ� ���� �ذ�


	fseek(rfp, 0L, SEEK_END); // ������ ������ �̵�.
	unsigned long long fsize = ftell(rfp); // �� ����? 262,144 
	fclose(rfp);
	freeInMemory();
	// ����, �� ���ϱ�
	inH = inW = (int)sqrt(fsize); // 512
	// �޸� �Ҵ�(�迭 �����ϱ�)
	allocInMemory();

	// ���� --> �޸� (�� �྿)
	rfp = fopen(fileName, "rb");
	for (int i = 0; i < inH; i++)
		fread(inImage[i], sizeof(unsigned char), inW, rfp);
	fclose(rfp);

	equalImage();
}

void saveImage() {
	char fullName[200] = "C:\\images\\";
	char tmpName[50];
	printf("���� ���ϸ�-->"); //cat50
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	wfp = fopen(fileName, "wb");
	// �޸� --> ���� (���྿)
	for (int i = 0; i < outH; i++)
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	fclose(wfp);

	MessageBox(hwnd, L"���� �Ϸ�", L"���� â", NULL);
}

void displayImage() {
	system("cls");
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int px = outImage[i][k];
			SetPixel(hdc, k + 1000, i + 200, RGB(px, px, px));
		}
	}

}

void allocInMemory() {
	inImage = (unsigned char**)malloc(sizeof(unsigned char*) * inH);
	for (int i = 0; i < inH; i++)
		inImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * inW);
}

void freeInMemory() {
	if (inImage == NULL)
		return;
	for (int i = 0; i < inH; i++)
		free(inImage[i]);
	free(inImage);
	inImage = NULL;
}

void allocOutMemory() {
	outImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		outImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);


	//��� �̹��� �ʱ�ȭ
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = 0;
		}
	}
}

void freeOutMemory() {
	if (outImage == NULL)
		return;
	for (int i = 0; i < outH; i++)
		free(outImage[i]);
	free(outImage);
	outImage = NULL;
}


// ����ó�� �Լ���(*****)

void equalImage() {//���� --> case 'a'
	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {

			outImage[i][k] = inImage[i][k];
		}
	}
	displayImage();
}

int getIntValue() {
	int retValue = 0;
	printf("���� �� : ");
	scanf("%d", &retValue);

	return retValue;
}

float getfloatValue() {


	float retValue = 0;
	printf("�Ǽ� �� : ");
	scanf("%f", &retValue);

	return retValue;
}
///////////////////////////
///////ȭ���� ó��/////////
///////////////////////////

void addImage() {//��� --> case 'b'

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�


	//��¥ �˰���
	int v = getIntValue();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] + v > 255) {
				outImage[i][k] = 255;
			}
			else {
				outImage[i][k] = inImage[i][k] + v;
			}

		}
	}
	displayImage();

}

void subImage() {//��Ӱ� --> case 'c'

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	int v = getIntValue();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] - v < 0) {
				outImage[i][k] = 0;
			}
			else {
				outImage[i][k] = inImage[i][k] - v;
			}

		}
	}
	displayImage();

}

void mulImage() {//��� (����) --> case 'd'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	float v = getfloatValue();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] * v > 255) {
				outImage[i][k] = 255;
			}
			else {
				outImage[i][k] = inImage[i][k] * v;
			}

		}
	}
	displayImage();

}

void divImage() {//��Ӱ� (������)--> case 'e'��
	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	float v = getfloatValue();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] / v < 0) {
				outImage[i][k] = 0;
			}
			else {
				outImage[i][k] = inImage[i][k] / v;
			}

		}
	}
	displayImage();

}

void reverseImage() {//���� --> case ' f'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = 255 - inImage[i][k];
		}
	}
	displayImage();

}

void bwImage() {//������ --> case 'g'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] <= 127)
				outImage[i][k] = 0;
			else
				outImage[i][k] = 255;
		}
	}
	displayImage();
}

void bwAvgImage() {//��հ� ��� ��� --> case 'h'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	// ��� ���ϱ�
	long long hap = 0;
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			hap += inImage[i][k];
	float avg = (float)hap / (inH * inW);

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] <= avg)
				outImage[i][k] = 0;
			else
				outImage[i][k] = 255;
		}
	}
	displayImage();
}

void MidValuebwImage() {//��ķ� ���������� ����--> case 'i'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	unsigned char** tmpImage;
	tmpImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		tmpImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {

			tmpImage[i][k] = inImage[i][k];
		}
	}

	int temp = 0;
	int midVal = 0;


	//��¥ �˰���

	for (int i = 0; i < inH; i++)//�� ����
	{
		for (int k = 1; k < inH; k++)
		{
			for (int j = 0; j < inH - k; j++)
			{
				if (tmpImage[i][j] > tmpImage[i][j + 1])
				{
					temp = tmpImage[i][j + 1];
					tmpImage[i][j + 1] = tmpImage[i][j];
					tmpImage[i][j] = temp;

				}
			}
		}
	}

	for (int i = 0; i < inW; i++)//�� ����
	{
		for (int k = 1; k < inW; k++)
		{
			for (int j = 0; j < inW - k; j++)
			{
				if (tmpImage[j][i] > tmpImage[j + 1][i])
				{
					temp = tmpImage[j + 1][i];
					tmpImage[j + 1][i] = tmpImage[j][i];
					tmpImage[j][i] = temp;

				}
			}
		}
	}

	midVal = tmpImage[inH / 2][inW / 2];//���������� �迭 ������ ��� �� ����

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] <= midVal)
				outImage[i][k] = 0;
			else
				outImage[i][k] = 255;
		}
	}

	displayImage();
}

void paraCAPImage()//�Ķ󺼶� ���� �� ��ü��(CAP)--> case 'j'��
{
	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�


	//��¥ �˰���
	double value = 0;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			value = 255.0 - 255.0 * pow((inImage[i][k] / 128.0 - 1.0), 2);
			if (value > 255) {
				value = 255;
			}
			else if (value < 0) {
				value = 0;

			}
			outImage[i][k] += value;
		}
	}
	displayImage();

}

void paraCUPImage()//�Ķ󺼶� ��ο� �� ��ü��(CUP)--> case 'k'��
{
	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	double value = 0;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			value = 255.0 * pow((inImage[i][k] / 128.0 - 1.0), 2);
			if (value > 255) {
				value = 255;
			}
			else if (value < 0) {
				value = 0;

			}
			outImage[i][k] += value;
		}
	}
	displayImage();

}

void gammaImage() {//���� ���� --> case 'l'��
	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	float gamma = 0;
	printf("���� �������� �Է� �Ͻÿ�(�Ǽ���) : ");
	scanf("%f", &gamma);

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			double m = inImage[i][k];
			outImage[i][k] = (unsigned int)255.0 * pow(m / 255.0, gamma);

		}
	}
	displayImage();
}

///////////////////////////
///////������ ó��/////////
///////////////////////////

void leftright_mirror_Image() {//�¿� �̷��� ó�� --> case 'm'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[i][inW - k - 1];

		}
	}
	displayImage();
}

void updown_mirror_Image() {//���� �̷��� ó�� --> case 'n'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[inH - i - 1][k];

		}
	}
	displayImage();
}

void updown_leftright_mirror_Image() {//�����¿� �̷��� ó�� --> case 'o'��

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[inH - i - 1][inW - k - 1];

		}
	}
	displayImage();
}

void rotateImage1() {//ȸ��(������) --> case 'p'��
	//������ : ȸ���� �������� ����� �ƴ�, hole������ ����(ȸ���ϸ鼭 �� ū ������ �ʿ�� �ϱ� ������)
	int angle = 0;
	printf("ȸ����ų ������ �Է��ϼ���. : ");
	scanf("%d", &angle);

	freeOutMemory();//�޸� �ʱ�ȭ
	outH = inH;
	outW = inW;
	allocOutMemory();//�޸� �Ҵ�

	int newI, newK = 0;
	//��¥ �˰���
	//xd = cos*i - sin*k
	//yd = sin*i + cos*k

	double radian = 0;//radian = degree*PI/180.0
	radian = -angle * PI / 180.0;

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {

			newI = cos(radian) * i - sin(radian) * k;
			newK = sin(radian) * i + cos(radian) * k;

			if ((0 <= newI && newI < outH) && (0 <= newK && newK < outH)) {
				outImage[newI][newK] = inImage[i][k];
			}
		}
	}
	displayImage();
}

void rotateImage2() {// ȸ��(������) --> case 'q'��
	//ȸ�� ������ �߾�����, hole���� �ذ� 
	//�߰����� ������ : ȸ���� �ϸ� ũ�Ⱑ ���������� ������ �ʱ� ������ �𼭸��� �߸��� ���� �߻� ==> ȸ���� ���� ���������� ũ�Ⱑ �����ǵ��� ����
	//��� �ʱ�ȭ �ȵ�...
	freeOutMemory();
	int angle = 0;
	printf("ȸ����ų ������ �Է��ϼ���. : ");
	scanf("%d", &angle);

	//��� ������ ũ�⸦ ���ϴ� ����
	double seta = angle % 90 * PI / 180.0;
	double seta_90 = (90 - angle % 90) * PI / 180.0;

	outH = (int)(inH * cos(seta_90) + inW * cos(seta));
	outW = (int)(inH * cos(seta) + inW * cos(seta_90));

	double radian = 0;//radian = degree*PI/180.0
	radian = angle * PI / 180.0;
	//// �޸� �Ҵ�(�迭 �����ϱ�)
	allocOutMemory();

	//// �ӽ� �޸� �Ҵ�
	unsigned char** tmpImage;
	tmpImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		tmpImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);

	//�ӽ� ��� �޸� �̹��� �ʱ�ȭ
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			tmpImage[i][k] = 0;
		}
	}

	int dx = (outH - inH) / 2;
	int dy = (outW - inW) / 2;

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			tmpImage[i + dx][k + dy] = inImage[i][k];
		}
	}


	//�߾� ��ġ ���ϱ�
	int oldI, oldK = 0;
	int cX = outH / 2;
	int cY = outW / 2;

	//��¥ �˰���
	//xd = cos*i - sin*k
	//yd = sin*i + cos*k

	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {

			oldI = cos(radian) * (i - cX) - sin(radian) * (k - cY) + cX;
			oldK = sin(radian) * (i - cX) + cos(radian) * (k - cY) + cY;


			if ((0 <= oldI && oldI < outH) && (0 <= oldK && oldK < outW))
				outImage[i][k] = tmpImage[oldI][oldK];
		}
	}

	for (int i = 0; i < outH; i++)
		free(tmpImage[i]);
	free(tmpImage);


	displayImage();
}

void zoominImage() {//Ȯ�� �˰���(������) --> case'r'��  --> ��ȭ2

	freeOutMemory();

	int s_Num = 0;//���� �Է¹��� ����
	printf("Ȯ���Ͻ� ������ �����ּ���. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH * s_Num;
	outW = inW * s_Num;

	allocOutMemory();

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i * s_Num][k * s_Num] = inImage[i][k];
		}
	}
	displayImage();
}

void zoominImage2() {//Ȯ�� �˰���(������) --> case's'��  --> ��ȭ2

	freeOutMemory();

	int s_Num = 0;//���� �Է¹��� ����
	printf("Ȯ���Ͻ� ������ �����ּ���. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH * s_Num;
	outW = inW * s_Num;

	allocOutMemory();

	//��¥ �˰���
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i / s_Num][k / s_Num];
		}
	}

	displayImage();
}

void zoomoutImage() {//��� �˰��� --> case't'�� --> ��ȭ2

	freeOutMemory();//�޸� �ʱ�ȭ

	int s_Num = 0;//���� �Է¹��� ����
	printf("����Ͻ� ������ �����ּ���. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH / s_Num;
	outW = inW / s_Num;

	//�޸� �Ҵ�(�迭 �����ϱ�)
	allocOutMemory();

	//��¥ �˰���
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i * s_Num][k * s_Num];
		}
	}

	displayImage();
}

void moveImage() {//�̵� �˰��� --> case 'u'��

	freeOutMemory();

	int x_Val, y_Val = 0;//�̵� x,y ����
	printf("���� : ");
	scanf("%d", &y_Val);
	printf("���� : ");
	scanf("%d", &x_Val);

	outH = inH;
	outW = inW;

	//�޸� �Ҵ�(�迭 �����ϱ�)
	allocOutMemory();

	//��¥ �˰���
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if ((0 <= i + x_Val && i + x_Val < inH) && (0 <= k + y_Val && k + y_Val < inH))
				outImage[i + x_Val][k + y_Val] = inImage[i][k];

		}
	}

	displayImage();
}