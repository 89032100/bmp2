#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <windows.h>
#include <fstream>
using namespace std;
typedef struct
{
	BYTE b;
	BYTE g;
	BYTE r;
}RGB;

class CBitmap {
public:
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	ifstream pfin;
	ofstream pfout;
	int offset;
	RGB **img;

	void load(const char* path) {
		pfin.open(path, ios::binary);
		pfin.read((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
		pfin.read((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
		offset = (infoHeader.biWidth * 3) % 4;
		if (offset != 0) {
			offset = 4 - offset;
		}
		img = new RGB*[infoHeader.biHeight];
		for (long i = infoHeader.biHeight - 1; i >= 0; i--) {
			img[i] = new RGB[infoHeader.biWidth];
			for (long j = 0; j < infoHeader.biWidth; j++) {
				pfin.read((char*)(&img[i][j]), sizeof(RGB));
			}
			if (offset != 0) {
				char ignore;
				for (int k = 0; k < offset; k++) {
					pfin.read(&ignore, sizeof(char));
				}
			}
		}
		pfin.close();
	}

	void write(const char* path) {
		pfout.open(path, ios::binary);
		pfout.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
		pfout.write((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
		for (long i = infoHeader.biHeight - 1; i >= 0; i--) {
			for (long j = 0; j < infoHeader.biWidth; j++) {
				pfout.write((char*)(&img[i][j]), sizeof(RGB));
			}
			if (offset != 0) {
				char ignore=0;
				for (int k = 0; k < offset; k++) {
					pfout.write(&ignore, sizeof(char));
				}
			}
		}
		pfout.close();
	}
	string fileHeaderToString() {
		string temp = "bfOffBits:" + to_string(fileHeader.bfOffBits) + " bfSize:" + to_string(fileHeader.bfSize) + " bfType:" + to_string(fileHeader.bfType);
		return temp;
	}
	string inforHeaderToString() {
		string temp = "biSize:" + to_string(infoHeader.biSize) + " biWidth:" + to_string(infoHeader.biWidth) + " biHeight:" + to_string(infoHeader.biHeight) +
			" biBitCount:" + to_string(infoHeader.biBitCount) + " biCompression:" + to_string(infoHeader.biCompression) + " biSizeImage:" +
			to_string(infoHeader.biSizeImage) + " biClrUsed:" + to_string(infoHeader.biClrUsed) + " biClrImportant:" + to_string(infoHeader.biClrImportant);
		return temp;
	}
	void toMirror() {
		RGB temp;
		for (long i = 0; i < infoHeader.biHeight; i++) {
			for (long j = 0, k = infoHeader.biWidth - 1; j < k; j++, k--) {
				temp.r = img[i][j].r;
				img[i][j].r = img[i][k].r;
				img[i][k].r = temp.r;

				temp.g = img[i][j].g;
				img[i][j].g = img[i][k].g;
				img[i][k].g = temp.g;

				temp.b = img[i][j].b;
				img[i][j].b = img[i][k].b;
				img[i][k].b = temp.b;
			}
		}
	}
	void toRotate90() {
		

		int temp = infoHeader.biHeight;
		infoHeader.biHeight = infoHeader.biWidth;
		infoHeader.biWidth = temp;
		infoHeader.biSizeImage = (infoHeader.biWidth * 24 + 31) / 32 * 4 * infoHeader.biHeight;
		fileHeader.bfSize = 54 + infoHeader.biSizeImage;

		RGB** img2;
		img2 = new RGB * [infoHeader.biHeight];
		for (long i = 0; i < infoHeader.biHeight; i++) {
			img2[i] = new RGB[infoHeader.biWidth];
			for (long j = 0; j < infoHeader.biWidth; j++)
				img2[i][j] = img[j][infoHeader.biHeight - 1 - i];
		}

		offset = (infoHeader.biWidth * 3) % 4;
		if (offset != 0) {
			offset = 4 - offset;
		}

		for (int i = 0; i < infoHeader.biWidth; i++) {
			delete[]img[i];
		}

		img = img2;

	}
	~CBitmap() {
		for (int i = 0; i < infoHeader.biHeight; i++) {
			delete[]img[i];
		}
		delete[]img;
	}
};
int main()
{
	CBitmap bmp;
	bmp.load("a.bmp");
	cout << bmp.fileHeaderToString() << endl;
	cout << bmp.inforHeaderToString() << endl;
	bmp.toMirror();
	bmp.write("a1.bmp");
	bmp.toRotate90();
	bmp.write("a2.bmp");
	system("a.bmp");
	system("a1.bmp");
	system("a2.bmp");

}
