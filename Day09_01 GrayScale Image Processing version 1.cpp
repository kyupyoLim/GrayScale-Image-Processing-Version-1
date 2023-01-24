#define _CRT_SECURE_NO_WARNINGS
using namespace std; // C++ 추가기능
#include <string>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <Windows.h> // GUI를 지원하는 기능
#define PI 3.141592
#include <malloc.h> //메모리 관련 함수를 사용할때는 maolloc.h를 포함해야 한다. 
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <io.h>

/////////////////////////
// 전역 변수
/////////////////////////
int inH, inW, outH, outW;
unsigned char** inImage = NULL, ** outImage = NULL;


HWND  hwnd; // 윈도 화면(외부, 물리적)
HDC hdc;    // 윈도 화면 (내부, 논리적)
FILE* rfp;
FILE* wfp;
char fileName[200];

/////////////////////////
// 함수 선언부 (함수의 이름만 준비)
/////////////////////////
void openImage();  void saveImage();
void allocInMemory(); void allocOutMemory(); void freeInMemory(); void freeOutMemory();
////////화소점 처리////////
void equalImage(); int getIntValue(); float getfloatValue(); void addImage();  void subImage(); void mulImage(); void divImage();
void reverseImage(); void bwImage(); void bwAvgImage(); void MidValuebwImage();
void paraCAPImage(); void paraCUPImage(); void gammaImage();
////////기하학 처리////////
void leftright_mirror_Image(); void updown_mirror_Image(); void updown_leftright_mirror_Image();
void rotateImage1(); void rotateImage2();
void zoominImage(); void zoominImage2(); void zoomoutImage(); void moveImage();

/////////////////////////
// 메인 코드부
/////////////////////////
void printMenu() {
	puts("\n\n============= GrayScale Image Procssing (Version 1) =============\n");
	puts("0.열기 1.저장 9.종료\n");
	puts("===========================화소점 처리===========================\n");
	puts("A.동일 B.밝게(덧셈) C.어둡게(뺄셈) D.밝게(곱셈) E.어둡게(나눗셈)\n");//화소점 처리
	puts("F.반전 G.흑백 H.흑백(평균값) I.흑백(중위값)\n");//화소점 처리
	puts("J.CAP(밝은 곳 입체) K.CUP(어두운 곳 입체) L.감마\n");//화소점 처리
	puts("===========================기하학 처리===========================\n");
	puts("M.좌우 미러링, N.상하 미러링, O.상하좌우 미러링\n");//기하학 처리
	puts("P.회전(포워딩,기준점 중앙 X), Q.회전(백워딩, 기준점 중앙 O)\n");//기하학 처리
	puts("R.확대(포워딩), S.확대(백워딩), T.축소, U.이동");//기하학 처리

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



		default: printf("입력이 잘못되었습니다. 다시 입력해주세요.");

		}
	}
	freeInMemory();
	freeOutMemory();
	printf("프로그램을 종료합니다.\n");

}

/////////////////////////
// 함수 정의부 (함수의 내용을 코딩)
/////////////////////////

// 공통 함수부 (열기, 저장, 디스플레이)

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
	printf("파일명-->"); //LENA256, flower512.....
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	// (중요!) 이미지의 폭과 높이를 알아내야 함.
	rfp = fopen(fileName, "rb");

	if (rfp == NULL) {
		printf("파일명이 일치하지 않습니다. 다시 입력해 주세요.");
		return;
	}//파일을 읽기 모드로 읽은 다음 rfp가 null이면 return 다시 처음으로 --> 심화 1 파일명 틀릴 떄 발생되는 버그 해결


	fseek(rfp, 0L, SEEK_END); // 파일의 끝까지 이동.
	unsigned long long fsize = ftell(rfp); // 너 어디니? 262,144 
	fclose(rfp);
	freeInMemory();
	// 높이, 폭 구하기
	inH = inW = (int)sqrt(fsize); // 512
	// 메모리 할당(배열 선언하기)
	allocInMemory();

	// 파일 --> 메모리 (한 행씩)
	rfp = fopen(fileName, "rb");
	for (int i = 0; i < inH; i++)
		fread(inImage[i], sizeof(unsigned char), inW, rfp);
	fclose(rfp);

	equalImage();
}

void saveImage() {
	char fullName[200] = "C:\\images\\";
	char tmpName[50];
	printf("저장 파일명-->"); //cat50
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	wfp = fopen(fileName, "wb");
	// 메모리 --> 파일 (한행씩)
	for (int i = 0; i < outH; i++)
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	fclose(wfp);

	MessageBox(hwnd, L"저장 완료", L"저장 창", NULL);
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


	//출력 이미지 초기화
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


// 영상처리 함수부(*****)

void equalImage() {//동일 --> case 'a'
	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {

			outImage[i][k] = inImage[i][k];
		}
	}
	displayImage();
}

int getIntValue() {
	int retValue = 0;
	printf("정수 값 : ");
	scanf("%d", &retValue);

	return retValue;
}

float getfloatValue() {


	float retValue = 0;
	printf("실수 값 : ");
	scanf("%f", &retValue);

	return retValue;
}
///////////////////////////
///////화소점 처리/////////
///////////////////////////

void addImage() {//밝게 --> case 'b'

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당


	//진짜 알고리즘
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

void subImage() {//어둡게 --> case 'c'

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
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

void mulImage() {//밝게 (곱셈) --> case 'd'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
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

void divImage() {//어둡게 (나눗셈)--> case 'e'번
	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
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

void reverseImage() {//반전 --> case ' f'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = 255 - inImage[i][k];
		}
	}
	displayImage();

}

void bwImage() {//흑백출력 --> case 'g'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
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

void bwAvgImage() {//평균값 흑백 출력 --> case 'h'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	// 평균 구하기
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

void MidValuebwImage() {//행렬로 순차적으로 정렬--> case 'i'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

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


	//진짜 알고리즘

	for (int i = 0; i < inH; i++)//행 정렬
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

	for (int i = 0; i < inW; i++)//열 정렬
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

	midVal = tmpImage[inH / 2][inW / 2];//순차적으로 배열 했으니 가운데 값 추출

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

void paraCAPImage()//파라볼라 밝은 곳 입체형(CAP)--> case 'j'번
{
	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당


	//진짜 알고리즘
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

void paraCUPImage()//파라볼라 어두운 곳 입체형(CUP)--> case 'k'번
{
	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
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

void gammaImage() {//감마 조정 --> case 'l'번
	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	float gamma = 0;
	printf("감마 조절값을 입력 하시오(실수값) : ");
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
///////기하학 처리/////////
///////////////////////////

void leftright_mirror_Image() {//좌우 미러링 처리 --> case 'm'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[i][inW - k - 1];

		}
	}
	displayImage();
}

void updown_mirror_Image() {//상하 미러링 처리 --> case 'n'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[inH - i - 1][k];

		}
	}
	displayImage();
}

void updown_leftright_mirror_Image() {//상하좌우 미러링 처리 --> case 'o'번

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[inH - i - 1][inW - k - 1];

		}
	}
	displayImage();
}

void rotateImage1() {//회전(정방향) --> case 'p'번
	//문제점 : 회전의 기준점이 가운데가 아님, hole문제가 생김(회전하면서 더 큰 영역이 필요로 하기 때문에)
	int angle = 0;
	printf("회전시킬 각도를 입력하세요. : ");
	scanf("%d", &angle);

	freeOutMemory();//메모리 초기화
	outH = inH;
	outW = inW;
	allocOutMemory();//메모리 할당

	int newI, newK = 0;
	//진짜 알고리즘
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

void rotateImage2() {// 회전(역방향) --> case 'q'번
	//회전 기준점 중앙으로, hole문제 해결 
	//추가적인 문제점 : 회전을 하면 크기가 가변적으로 변하지 않기 때문에 모서리가 잘리는 현상 발생 ==> 회전한 다음 가변적으로 크기가 조절되도록 설정
	//출력 초기화 안됨...
	freeOutMemory();
	int angle = 0;
	printf("회전시킬 각도를 입력하세요. : ");
	scanf("%d", &angle);

	//출력 영상의 크기를 구하는 공식
	double seta = angle % 90 * PI / 180.0;
	double seta_90 = (90 - angle % 90) * PI / 180.0;

	outH = (int)(inH * cos(seta_90) + inW * cos(seta));
	outW = (int)(inH * cos(seta) + inW * cos(seta_90));

	double radian = 0;//radian = degree*PI/180.0
	radian = angle * PI / 180.0;
	//// 메모리 할당(배열 선언하기)
	allocOutMemory();

	//// 임시 메모리 할당
	unsigned char** tmpImage;
	tmpImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		tmpImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);

	//임시 출력 메모리 이미지 초기화
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


	//중앙 위치 구하기
	int oldI, oldK = 0;
	int cX = outH / 2;
	int cY = outW / 2;

	//진짜 알고리즘
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

void zoominImage() {//확대 알고리즘(정방향) --> case'r'번  --> 심화2

	freeOutMemory();

	int s_Num = 0;//배율 입력받을 변수
	printf("확대하실 배율을 적어주세요. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH * s_Num;
	outW = inW * s_Num;

	allocOutMemory();

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i * s_Num][k * s_Num] = inImage[i][k];
		}
	}
	displayImage();
}

void zoominImage2() {//확대 알고리즘(역방향) --> case's'번  --> 심화2

	freeOutMemory();

	int s_Num = 0;//배율 입력받을 변수
	printf("확대하실 배율을 적어주세요. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH * s_Num;
	outW = inW * s_Num;

	allocOutMemory();

	//진짜 알고리즘
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i / s_Num][k / s_Num];
		}
	}

	displayImage();
}

void zoomoutImage() {//축소 알고리즘 --> case't'번 --> 심화2

	freeOutMemory();//메모리 초기화

	int s_Num = 0;//배율 입력받을 변수
	printf("축소하실 배율을 적어주세요. (2,4,8,16...) : ");
	scanf("%d", &s_Num);

	outH = inH / s_Num;
	outW = inW / s_Num;

	//메모리 할당(배열 선언하기)
	allocOutMemory();

	//진짜 알고리즘
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i * s_Num][k * s_Num];
		}
	}

	displayImage();
}

void moveImage() {//이동 알고리즘 --> case 'u'번

	freeOutMemory();

	int x_Val, y_Val = 0;//이동 x,y 변수
	printf("가로 : ");
	scanf("%d", &y_Val);
	printf("세로 : ");
	scanf("%d", &x_Val);

	outH = inH;
	outW = inW;

	//메모리 할당(배열 선언하기)
	allocOutMemory();

	//진짜 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if ((0 <= i + x_Val && i + x_Val < inH) && (0 <= k + y_Val && k + y_Val < inH))
				outImage[i + x_Val][k + y_Val] = inImage[i][k];

		}
	}

	displayImage();
}