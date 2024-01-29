#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct point {
	double x = 0;
	double y = 0;
	double z = 0;
}Point;

typedef struct voxel {
	int edgePassNum = 0;		//�������� edge�� mesh�� ������ ����
	int edgePassSum = 0;		//�������� edge�� mesh�� ������ ��ġ�� 0~11���� �Ͽ� 2������ ���� �� 10������ ǥ��
	int b = 0;					//������ �� �� = row
	int c = 0;					//������ �� �� = col
	int h = 0;					//������ ���� �� = height
	int vertexInNum = 0;		//�������� mesh�� ���ԵǴ� vertex�� ����
	int vertexInSum = 0;		//�������� mesh�� ���ԵǴ� vertex�� ��ġ�� 2������ ���� �� 10������ ǥ��
	int voxelCase = 0;			//�ش��� ������ ���� Case���� ��Ÿ���� ��
	int voxelIndex = 0;			//�ش� ������ index(��ȣ)
	int vError = 0;				//�ش� ������ Skinny Triangle ����
	int vErrorFaceNum = 0;		//�ش� ������ Skinny Triangle ����
	int vFaceNum = 0;			//�ش� ������ face ����
	int startIndex = -1;		//faceList���� n��° index���� faceNum ������ŭ������ �ش� ������ ����
}Voxel;

typedef struct vertexset {
	Point P;
	int index = 0;
	int use = 1;
}vertexSet;

typedef struct faceset {
	int v1 = 0;
	int v2 = 0;
	int v3 = 0;
	int voxelIndex = 0;
	int isErrorFace = 0;
	int use = 1;
}faceSet;


//FILE* fpEdgePassCheckFile1 = fopen("1step_emd_1003_EdgePassCheck1.txt", "r");//Data ����
FILE* fpEdgePassCheckFile1;//Data ����
//���� ���� 7���� data
//edgePassNum, edgePassSum, b, c, h, vertexInNum, vertexInSum
//edgePassNum	:	�������� edge�� mesh�� ������ ����
//edgePassSum	:	�������� edge�� mesh�� ������ ��ġ�� 0~11���� �Ͽ� 2������ ���� �� 10������ ǥ��
//b,c,h			:	������ ��, ��, ��ġ ���� = row, col, height
//vertexInNum	:	�������� mesh�� ���ԵǴ� vertex�� ����
//vertexInSum	:	�������� mesh�� ���ԵǴ� vertex�� ��ġ�� 2������ ���� �� 10������ ǥ��
//voxelIndex	:	�ش� ������ index(��ȣ)

//FILE* fpVOFF = fopen("V_1step_emd_1003.OFF", "r");
FILE* fpVOFF;
//vertexNum FaceNum 0
//x, y, z...
//3 v1 v2 v3 voxelNum isErrorFace
//voxelIndex: ��ġ�ϴ� ������ index(0~)

//FILE* fpErrorFace = fopen("ErrorFace.txt", "w");
FILE* fpVoxelCase = fopen("VoxelCase.txt", "w");
FILE* fpDebug = fopen("Debug.txt", "w");
//FILE* fpErrorCase = fopen("ErrorCase.txt", "w");
FILE* fpErrorCase;

//FILE* fpNewVOFF = fopen("New.OFF", "w");
FILE* fpNewVOFF;

Point findNearIndex(faceSet Fs[], vertexSet Vs[], int n);
void setCloseCorner(Point p1, Point* newp1, Voxel Vo[], int i);
Point getCloseCorner(Point p1, Voxel Vo[], int i);
//void setCenterNotCorner(Point p1, Point* newp1, Voxel* Vo, int i);

int IsSamePoint(Point p1, Point p2);
int IsNearFace(int A, int B, faceSet Fs[]);


//double cl = 82.8
//double half_cl = cl / 2;
//double cl;
//ouble half_cl;
//conotur_level �ʿ� ���� ����

double stepSize = 1;
double half_stepSize = stepSize / 2;


int main(void)
{
	int edgePassNum = 0, edgePassSum = 0, valVoxelNum = 0;
	int b = 0, c = 0, h = 0;
	int vertexNum, faceNum, zero, three;
	int newVertexNum, newFaceNum;
	int errorIndex = 0;								//�߰��Ǵ� vertex�� ����
	int errorNum = 0;
	int n1 = 1;
	int n2 = 3;
	int i = 0;

	int errorCase[15] = { 0 };
	//int AllErrorFace = 0;
	int AllnowErrorFace = 0;
	int AllErrorCase = 0;
	int AllnowErrorCase = 0;
	int nowErrorCase[15] = { 0 };
	int nowErrorCaseTemp[15] = { 0 };

	char OFFName[40] = "";
	char EdgePassName[40] = "";
	char ErrorCaseName[40] = "";
	char EMDNum[40] = "";
	char newOFFName[40] = "";

	/*
	printf("��Īť�긦 Ȱ���Ͽ� ���� OFF���ϰ� ������ ������ �ִ� ���� 2���� �ʿ��մϴ�.\n");
	printf("OFF������ �̸��� �Է��ϼ���\n");

	scanf("%s", OFFName);

	//ErrorCase ���� ���� ����
	strcat(ErrorCaseName, OFFName);
	strcat(ErrorCaseName, "_ErrorCase.txt");
	fpErrorCase = fopen(ErrorCaseName, "w");

	printf("EdgePass ������ �� ������ �̸��� �Է��ϼ���\n");
	scanf("%s", EdgePassName);
	strcat(OFFName, ".OFF");
	strcat(EdgePassName, ".txt");
	*/

	printf("Ȱ���� EMD �ѹ��� �Է��ϼ���.\n");
	scanf("%s", EMDNum);

	strcat(OFFName, "V_1step_emd_");	//OFFName = V_1step_emd_
	strcat(OFFName, EMDNum);			//OFFName = V_1step_emd_****
	strcat(OFFName, ".OFF");			//OFFName = V_1step_emd_****.OFF

	strcat(EdgePassName, "1step_emd_");				//EdgePassName = 1step_emd_
	strcat(EdgePassName, EMDNum);					//EdgePassName = 1step_emd_****
	strcat(EdgePassName, "_EdgePassCheck1.txt");	//EdgePassName = 1step_emd_****_EdgePassCheck1.txt

	strcat(ErrorCaseName, EMDNum);
	strcat(ErrorCaseName, "_ErrorCase.txt");
	fpErrorCase = fopen(ErrorCaseName, "w");

	strcat(newOFFName, "new");
	strcat(newOFFName, EMDNum);
	strcat(newOFFName, ".OFF");

	fpNewVOFF = fopen(newOFFName, "w");

	fpVOFF = fopen(OFFName, "r");
	if (fpVOFF == NULL)
	{
		printf("���� : �ش� OFF������ ���������ʽ��ϴ�.\n");
		return 0;
	}
	fpEdgePassCheckFile1 = fopen(EdgePassName, "r");
	if (fpEdgePassCheckFile1 == NULL)
	{
		printf("���� : �ش� ���� ���� ������ ���������ʽ��ϴ�.\n");
		return 0;
	}

	/*
	printf("Contour Level�� �Է��ϼ���.\n");
	scanf("%lf", &cl);

	half_cl = cl / 2;
	*/

	fscanf(fpVOFF, "%d %d %d %d", &vertexNum, &faceNum, &zero, &valVoxelNum);

	Voxel* Vo = new Voxel[valVoxelNum];
	vertexSet* Vs = new vertexSet[vertexNum];
	faceSet* Fs = new faceSet[faceNum];
	vertexSet* newVs = new vertexSet[vertexNum];
	faceSet* newFs = new faceSet[faceNum];

	newVertexNum = vertexNum;
	newFaceNum = faceNum;

	//EdgePassCheckFile1 : edgePassNum, edgePassSum, b, c, h, vertexInNum, vertexInSum
	for (int i = 0; i < valVoxelNum; i++)
	{
		fscanf(fpEdgePassCheckFile1, "%d %d %d %d %d %d %d", &Vo[i].edgePassNum, &Vo[i].edgePassSum, &Vo[i].b, &Vo[i].c, &Vo[i].h, &Vo[i].vertexInNum, &Vo[i].vertexInSum);
		Vo[i].voxelIndex = i;
	}

	for (int i = 0; i < vertexNum; i++)
	{
		fscanf(fpVOFF, "%lf %lf %lf", &Vs[i].P.x, &Vs[i].P.y, &Vs[i].P.z);
		Vs[i].index = i;

		newVs[i].P.x = Vs[i].P.x;
		newVs[i].P.y = Vs[i].P.y;
		newVs[i].P.z = Vs[i].P.z;
		newVs[i].index = i;
	}

	for (int i = 0; i < faceNum; i++)
	{
		fscanf(fpVOFF, "%d %d %d %d %d %d", &three, &Fs[i].v1, &Fs[i].v2, &Fs[i].v3, &Fs[i].voxelIndex, &Fs[i].isErrorFace);
		if (Fs[i].isErrorFace == 1)
		{
			Vo[Fs[i].voxelIndex].vErrorFaceNum++;
			Vo[Fs[i].voxelIndex].vError = 1;
		}
		if (Vo[Fs[i].voxelIndex].startIndex == -1)
		{
			Vo[Fs[i].voxelIndex].startIndex = i;
		}
		Vo[Fs[i].voxelIndex].vFaceNum++;

		newFs[i].v1 = Fs[i].v1;
		newFs[i].v2 = Fs[i].v2;
		newFs[i].v3 = Fs[i].v3;
		newFs[i].voxelIndex = Fs[i].voxelIndex;
		newFs[i].isErrorFace = Fs[i].isErrorFace;
	}

	//�з� ����
	for (int i = 0; i < valVoxelNum; i++)
	{
		if (Vo[i].edgePassNum == 3)
		{
			Vo[i].voxelCase = 1;
			//printf("%d\n", Vo[i].voxelIndex);
			if (Vo[i].vError == 1)
				errorCase[1]++;
		}
		else if (Vo[i].edgePassNum == 5)
		{
			Vo[i].voxelCase = 4;
			if (Vo[i].vError == 1)
				errorCase[4]++;
		}
		else if (Vo[i].edgePassNum == 7)
		{
			Vo[i].voxelCase = 11;
			if (Vo[i].vError == 1)
			{
				errorCase[11]++;
			}

		}
		else if (Vo[i].edgePassNum == 9)
		{
			Vo[i].voxelCase = 12;
			if (Vo[i].vError == 1)
				errorCase[12]++;
		}
		else if (Vo[i].edgePassNum == 12)
		{
			Vo[i].voxelCase = 7;
			if (Vo[i].vError == 1)
				errorCase[7]++;
		}
		else if (Vo[i].edgePassNum == 4)
		{
			if (Vo[i].edgePassSum == 85 || Vo[i].edgePassSum == 170 || Vo[i].edgePassSum == 3840)
			{
				Vo[i].voxelCase = 5;
				if (Vo[i].vError == 1)
					errorCase[5]++;
			}
			else
			{
				Vo[i].voxelCase = 2;
				if (Vo[i].vError == 1)
					errorCase[2]++;
			}
		}
		else if (Vo[i].edgePassNum == 8)//case
		{
			if (Vo[i].edgePassSum == 255 || Vo[i].edgePassSum == 3925 || Vo[i].edgePassSum == 4010)
			{
				Vo[i].voxelCase = 13;
				if (Vo[i].vError == 1)
				{
					errorCase[13]++;
				}

			}
			else//�߰��Ǹ� Ȯ���� ����
			{
				Vo[i].voxelCase = 6;
				if (Vo[i].vError == 1)
					errorCase[6]++;
			}
		}
		else if (Vo[i].edgePassNum == 6)//case 3, 10 / 8, 9, 14
		{
			if (Vo[i].vertexInNum == 2 || Vo[i].vertexInNum == 6)//case 3, 10  (index 2�� �Ǵ� reverse�� 6��)
			{
				if (Vo[i].vertexInSum == 20 || Vo[i].vertexInSum == 40 || Vo[i].vertexInSum == 65 || Vo[i].vertexInSum == 125\
					|| Vo[i].vertexInSum == 130 || Vo[i].vertexInSum == 190 || Vo[i].vertexInSum == 215 || Vo[i].vertexInSum == 235)
				{
					Vo[i].voxelCase = 3;
					if (Vo[i].vError == 1)
						errorCase[3]++;
				}
				else//�߰��Ǹ� Ȯ���� ����
				{
					Vo[i].voxelCase = 10;
					if (Vo[i].vError == 1)
						errorCase[10]++;
				}
			}
			else if (Vo[i].vertexInNum == 4)
			{
				if (Vo[i].vertexInSum == 27 || Vo[i].vertexInSum == 39 || Vo[i].vertexInSum == 78 || Vo[i].vertexInSum == 114\
					|| Vo[i].vertexInSum == 141 || Vo[i].vertexInSum == 177 || Vo[i].vertexInSum == 216 || Vo[i].vertexInSum == 228)
				{
					Vo[i].voxelCase = 8;
					if (Vo[i].vError == 1)
						errorCase[8]++;
				}
				else if (Vo[i].vertexInSum == 23 || Vo[i].vertexInSum == 46 || Vo[i].vertexInSum == 57 || Vo[i].vertexInSum == 77\
					|| Vo[i].vertexInSum == 99 || Vo[i].vertexInSum == 116 || Vo[i].vertexInSum == 139 || Vo[i].vertexInSum == 156\
					|| Vo[i].vertexInSum == 178 || Vo[i].vertexInSum == 198 || Vo[i].vertexInSum == 209 || Vo[i].vertexInSum == 232)
				{
					Vo[i].voxelCase = 9;
					if (Vo[i].vError == 1)
						errorCase[9]++;
				}
				else//�߰��Ǹ� Ȯ���� ����
				{
					Vo[i].voxelCase = 14;
					if (Vo[i].vError == 1)
						errorCase[14]++;
				}
			}
		}
		else
		{
			printf("edgePassNum ���� Ȯ�ε�\n");
		}

		//���� Ȯ�� �� ���
		if (Vo[i].voxelCase == 0)
		{
			printf("voxelCase �̺з� Ȯ�ε�\n");
		}
		if (Vo[i].edgePassNum < 3 || Vo[i].edgePassNum > 9 && Vo[i].edgePassNum != 12)
		{
			printf("edgePassNum ���� Ȯ�ε� : edgePassNum : %d, edgePassSum : %d, Case : %d\n", Vo[i].edgePassNum, Vo[i].edgePassSum, Vo[i].voxelCase);
		}
		if (Vo[i].edgePassSum < 7 || Vo[i].edgePassSum > 4095)//000000000111 ~ 111111111111 ����
		{
			printf("edgePassSum ���� Ȯ�ε� : edgePassNum : %d, edgePassSum : %d, Case : %d\n", Vo[i].edgePassNum, Vo[i].edgePassSum, Vo[i].voxelCase);
		}
		if (Vo[i].vertexInNum < 1 || Vo[i].vertexInNum > 7)//1~7 ����
		{
			printf("vertexInNum ���� Ȯ�ε� : vertexInNum : %d, vertexInSum : %d, Case : %d\n", Vo[i].vertexInNum, Vo[i].vertexInSum, Vo[i].voxelCase);
		}
		if (Vo[i].vertexInSum < 1 || Vo[i].vertexInSum > 254)//00000001 ~ 11111110 ����
		{
			printf("vertexInNum ���� Ȯ�ε� : vertexInNum : %d, vertexInSum : %d, Case : %d\n", Vo[i].vertexInNum, Vo[i].vertexInSum, Vo[i].voxelCase);
		}
		if (Vo[i].vError == 1)
		{
			//fprintf(fpErrorFace, "ErrorFace�� ������ %6d��° Case %2d ������ ��ġ %3d %3d %3d, face %2d���� Case %2d \n", Vo[i].voxelIndex, Vo[i].voxelCase, Vo[i].b, Vo[i].c, Vo[i].h, Vo[i].voxelCase, Vo[i].vFaceNum);
		}
		fprintf(fpVoxelCase, "Index %3d : Case %2d\n", Vo[i].voxelIndex, Vo[i].voxelCase);
	}



	//�Է½� ��¿�
	/*
	//���� index sample ������ �ش� ���� ���� Face ���
	int indexSample = 0;
	int sample = 0;

	printf("���° ������ ������ �˰� �ͽ��ϱ�?\n");
	scanf("%d", &sample);

	printf("index%d ������ Case%2d�Դϴ�.\n", sample, Vo[sample].voxelCase);
	printf("��������\n��ġ : b = %d, c = %d, h = %d\nedgePassNum = %d, edgePassSum = %d, vertexInNum = %d, vertexInSum = %d\n", Vo[sample].b, Vo[sample].c, Vo[sample].h, Vo[sample].edgePassNum, Vo[sample].edgePassSum, Vo[sample].vertexInNum, Vo[sample].vertexInSum);
	//printf("vError = %d, vfaceNum = %d\n", Vo[sample].vError, Vo[sample].vFaceNum);
	printf("\n�ش� ������ ���Ե� face�� ����\n");
	printf("���� face index = %d\n", Vo[sample].startIndex);
	indexSample = Vo[sample].startIndex;
	for (int i = 0; i < Vo[sample].vFaceNum; i++)
	{
		printf("%d��° face index %d %d %d\n", indexSample, Fs[indexSample].v1, Fs[indexSample].v2, Fs[indexSample].v3);
		printf("��ǥ1 : %lf %lf %lf\n", Vs[Fs[indexSample].v1].P.x, Vs[Fs[indexSample].v1].P.y, Vs[Fs[indexSample].v1].P.z);
		printf("��ǥ2 : %lf %lf %lf\n", Vs[Fs[indexSample].v2].P.x, Vs[Fs[indexSample].v2].P.y, Vs[Fs[indexSample].v2].P.z);
		printf("��ǥ3 : %lf %lf %lf\n", Vs[Fs[indexSample].v3].P.x, Vs[Fs[indexSample].v3].P.y, Vs[Fs[indexSample].v3].P.z);
		indexSample
	}
	printf("\n");
	*/

	/*
	//findNearIndex ����
	int n5 = 0;
	Point sample;
	sample = findNearIndex(Fs, Vs, n5);

	printf("nearPoint : %lf %lf %lf\n\n", sample.x, sample.y, sample.z);
	*/

	printf("���� Case �з� �Ϸ�, ���� �ذ� ����\n");
	//���� �ذ�
	int tempStart = 0;

	printf("������1\n");
	//��� ������ Ž��
	for (i = 0; i < valVoxelNum; i++)
	{

		if (Vo[i].vError == 1)				//������ ������ �ִ�. ������ ������ �ִ� face�� �ִ�.
		{
			//Case1

			if (Vo[i].voxelCase == 1)		//������ Case�� 1�̴�. face�� 1��
			{
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;

				//printf("%d index ���� Case1\n", Vo[i].voxelIndex);
				tempStart = Vo[i].startIndex;	//������ face ���� index �� �ֱ�
				//1��° face (tempStart, fIndex01,02,03 , nearPoint0)
				if (Fs[tempStart].isErrorFace == 1) // 1�� Face�� ���� Face
				{
					//printf("Case1 Error Face �߻�\n");
					fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
					fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
					fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3
					nearPoint0 = findNearIndex(Fs, Vs, tempStart);   //�ش� face�� ����� ���� ������ ã��

					newVs[fIndex01].P = nearPoint0; // ����� �������� �ֱ� 1
					newVs[fIndex02].P = nearPoint0; // ����� �������� �ֱ� 2
					newVs[fIndex03].P = nearPoint0; // ����� �������� �ֱ� 3

					Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
					newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
					newFaceNum--;							// face ���� �Ѱ� ����
					nowErrorCase[1]++;
					for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
					{
						if (newFs[j].v1 == fIndex02 || newFs[j].v1 == fIndex03)
						{
							newFs[j].v1 = fIndex01;
						}
						if (newFs[j].v2 == fIndex02 || newFs[j].v2 == fIndex03)
						{
							newFs[j].v2 = fIndex01;
						}
						if (newFs[j].v3 == fIndex02 || newFs[j].v3 == fIndex03)
						{
							newFs[j].v3 = fIndex01;
						}
					}
				}
			}
			//Case1 ���� �Ϸ�

			//Case3,10

			if (Vo[i].voxelCase == 3 || Vo[i].voxelCase == 10)
			{
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;

				//printf("%d index ���� Case3, 10\n", Vo[i].voxelIndex);
				tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�
				//printf("���̽� ���� %d\n", Vo[i].vFaceNum);
				nowErrorCaseTemp[3] = 0;
				nowErrorCaseTemp[10] = 0;

				if (Vo[i].vFaceNum == 2)// face�� 2���� �ƴ� ���� ���°� �⺻�� �ٸ��Ƿ� �ٸ� ����� �����ؾ� �Ѵ�.
				{
					//1��° face (tempStart, fIndex01,02,03 , nearPoint0)
					if (Fs[tempStart].isErrorFace == 1) // 1�� Face�� ���� Face
					{
						//printf("Case3, 10 Error Face �߻�\n");
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3
						nearPoint0 = findNearIndex(Fs, Vs, tempStart);   //�ش� face�� ����� ���� ������ ã��

						//printf("%lf, %lf, %lf\n", nearPoint0.x, nearPoint0.y, nearPoint0.z);

						newVs[fIndex01].P = nearPoint0; // ����� �������� �ֱ� 1
						newVs[fIndex02].P = nearPoint0; // ����� �������� �ֱ� 2
						newVs[fIndex03].P = nearPoint0; // ����� �������� �ֱ� 3

						Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����	

						if (Vo[i].voxelCase == 3)
						{
							nowErrorCaseTemp[3] = 1;
							nowErrorCase[3]++;
						}
						if (Vo[i].voxelCase == 10)
						{
							nowErrorCaseTemp[10] = 1;
							nowErrorCase[10]++;
						}

						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex02 || newFs[j].v1 == fIndex03)
							{
								newFs[j].v1 = fIndex01;
							}
							if (newFs[j].v2 == fIndex02 || newFs[j].v2 == fIndex03)
							{
								newFs[j].v2 = fIndex01;
							}
							if (newFs[j].v3 == fIndex02 || newFs[j].v3 == fIndex03)
							{
								newFs[j].v3 = fIndex01;
							}
						}
					}

					//2��° face (tempStart + 1 , fIndex11,12,13 , nearPoint1)
					if (Fs[tempStart + 1].isErrorFace == 1) // 2�� Face�� ���� Face
					{
						//printf("Case3, 10 Error Face �߻�\n");
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3
						nearPoint1 = findNearIndex(Fs, Vs, tempStart + 1);   //�ش� face�� ����� ���� ������ ã��

						//printf("%lf, %lf, %lf\n", nearPoint1.x, nearPoint1.y, nearPoint1.z);

						newVs[fIndex11].P = nearPoint1; // ����� �������� �ֱ� 1
						newVs[fIndex12].P = nearPoint1; // ����� �������� �ֱ� 2
						newVs[fIndex13].P = nearPoint1; // ����� �������� �ֱ� 3

						if (Vo[i].voxelCase == 3 && nowErrorCaseTemp[3] == 0)
						{
							nowErrorCase[3]++;
						}
						if (Vo[i].voxelCase == 10 && nowErrorCaseTemp[10] == 0)
						{
							nowErrorCase[10]++;
						}

						Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����

						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex12 || newFs[j].v1 == fIndex13)
							{
								newFs[j].v1 = fIndex11;
							}
							if (newFs[j].v2 == fIndex12 || newFs[j].v2 == fIndex13)
							{
								newFs[j].v2 = fIndex11;
							}
							if (newFs[j].v3 == fIndex12 || newFs[j].v3 == fIndex13)
							{
								newFs[j].v3 = fIndex11;
							}
						}
					}
				}
				else
				{
					if (Vo[i].voxelCase == 3)
					{
						printf("!!Case3 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
					}
					if (Vo[i].voxelCase == 10)
					{
						//if (Vo[i].vFaceNum != 4)
						printf("!!Case10 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
						//Case10�� ��� face 4���� ���� �� �ִ�.
					}
				}

			}
			//Case3,10 ���� �Ϸ�

			//Case12

			if (Vo[i].voxelCase == 12)
			{
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[12] = 0;


				//printf("%d index ���� Case12\n", Vo[i].voxelIndex);
				tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�


				//printf("���̽� ���� %d\n", Vo[i].vFaceNum);

				if (Vo[i].vFaceNum == 3)
				{

					//1��° face (tempStart, fIndex01,02,03 , nearPoint0)
					if (Fs[tempStart].isErrorFace == 1) // 1�� Face�� ���� Face
					{
						//printf("Case12 Error Face �߻�\n");
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3
						nearPoint0 = findNearIndex(Fs, Vs, tempStart);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex01].P = nearPoint0; // ����� �������� �ֱ� 1
						newVs[fIndex02].P = nearPoint0; // ����� �������� �ֱ� 2
						newVs[fIndex03].P = nearPoint0; // ����� �������� �ֱ� 3

						Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����
						nowErrorCase[12]++;
						nowErrorCaseTemp[12] = 1;

						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex02 || newFs[j].v1 == fIndex03)
							{
								newFs[j].v1 = fIndex01;
							}
							if (newFs[j].v2 == fIndex02 || newFs[j].v2 == fIndex03)
							{
								newFs[j].v2 = fIndex01;
							}
							if (newFs[j].v3 == fIndex02 || newFs[j].v3 == fIndex03)
							{
								newFs[j].v3 = fIndex01;
							}
						}
					}


					//2��° face (tempStart + 1 , fIndex11,12,13 , nearPoint1)
					if (Fs[tempStart + 1].isErrorFace == 1) // 2�� Face�� ���� Face
					{
						//printf("Case12 Error Face �߻�\n");
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3
						nearPoint1 = findNearIndex(Fs, Vs, tempStart + 1);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex11].P = nearPoint1; // ����� �������� �ֱ� 1
						newVs[fIndex12].P = nearPoint1; // ����� �������� �ֱ� 2
						newVs[fIndex13].P = nearPoint1; // ����� �������� �ֱ� 3

						Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����

						if (nowErrorCaseTemp[12] == 0)
						{
							nowErrorCase[12]++;
							nowErrorCaseTemp[12] = 1;
						}


						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex12 || newFs[j].v1 == fIndex13)
							{
								newFs[j].v1 = fIndex11;
							}
							if (newFs[j].v2 == fIndex12 || newFs[j].v2 == fIndex13)
							{
								newFs[j].v2 = fIndex11;
							}
							if (newFs[j].v3 == fIndex12 || newFs[j].v3 == fIndex13)
							{
								newFs[j].v3 = fIndex11;
							}
						}
					}

					//3��° face (tempStart + 2 , findex21,22,23 , nearPoint2)
					if (Fs[tempStart + 2].isErrorFace == 1) // 3�� Face�� ���� Face
					{
						//printf("Case12 Error Face �߻�\n");
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3
						nearPoint2 = findNearIndex(Fs, Vs, tempStart + 2);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex21].P = nearPoint2; // ����� �������� �ֱ� 1
						newVs[fIndex22].P = nearPoint2; // ����� �������� �ֱ� 2
						newVs[fIndex23].P = nearPoint2; // ����� �������� �ֱ� 3

						Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����

						if (nowErrorCaseTemp[12] == 0)
						{
							nowErrorCase[12]++;
							nowErrorCaseTemp[12] = 1;
						}

						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex22 || newFs[j].v1 == fIndex23)
							{
								newFs[j].v1 = fIndex21;
							}
							if (newFs[j].v2 == fIndex22 || newFs[j].v2 == fIndex23)
							{
								newFs[j].v2 = fIndex21;
							}
							if (newFs[j].v3 == fIndex22 || newFs[j].v3 == fIndex23)
							{
								newFs[j].v3 = fIndex21;
							}
						}
					}

				}
				else
				{
					//if (Vo[i].vFaceNum != 5)
					printf("!!Case12 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
					//Case12�� ��� face 5���� ���� �� �ִ�.
				}
			}
			//Case12 ���� �Ϸ�

			//Case7

			if (Vo[i].voxelCase == 7)	// �ﰢ�� ���� 4��
			{
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[7] = 0;

				//printf("%d index ���� Case7\n", Vo[i].voxelIndex);
				tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�

				if (Vo[i].vFaceNum == 4)
				{
					//1��° face (tempStart, fIndex01,02,03 , nearPoint0)
					if (Fs[tempStart].isErrorFace == 1) // 1�� Face�� ���� Face
					{
						//printf("Case7 Error Face �߻�\n");
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3
						nearPoint0 = findNearIndex(Fs, Vs, tempStart);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex01].P = nearPoint0; // ����� �������� �ֱ� 1
						newVs[fIndex02].P = nearPoint0; // ����� �������� �ֱ� 2
						newVs[fIndex03].P = nearPoint0; // ����� �������� �ֱ� 3

						Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;							// face ���� �Ѱ� ����
						nowErrorCase[7]++;
						nowErrorCaseTemp[7] = 1;

						for (int j = 0; j < faceNum; j++)		// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex02 || newFs[j].v1 == fIndex03)
							{
								newFs[j].v1 = fIndex01;
							}
							if (newFs[j].v2 == fIndex02 || newFs[j].v2 == fIndex03)
							{
								newFs[j].v2 = fIndex01;
							}
							if (newFs[j].v3 == fIndex02 || newFs[j].v3 == fIndex03)
							{
								newFs[j].v3 = fIndex01;
							}
						}
					}

					//2��° face (tempStart + 1 , fIndex11,12,13 , nearPoint1)
					if (Fs[tempStart + 1].isErrorFace == 1) // 2�� Face�� ���� Face
					{
						//printf("Case7 Error Face �߻�\n");
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3
						nearPoint1 = findNearIndex(Fs, Vs, tempStart + 1);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex11].P = nearPoint1; // ����� �������� �ֱ� 1
						newVs[fIndex12].P = nearPoint1; // ����� �������� �ֱ� 2
						newVs[fIndex13].P = nearPoint1; // ����� �������� �ֱ� 3

						Fs[tempStart + 1].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;								// face ���� �Ѱ� ����
						if (nowErrorCaseTemp[7] == 0)
						{
							nowErrorCaseTemp[7] = 1;
							nowErrorCase[7]++;
						}

						for (int j = 0; j < faceNum; j++)			// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex12 || newFs[j].v1 == fIndex13)
							{
								newFs[j].v1 = fIndex11;
							}
							if (newFs[j].v2 == fIndex12 || newFs[j].v2 == fIndex13)
							{
								newFs[j].v2 = fIndex11;
							}
							if (newFs[j].v3 == fIndex12 || newFs[j].v3 == fIndex13)
							{
								newFs[j].v3 = fIndex11;
							}
						}
					}

					//3��° face (tempStart + 2 , findex21,22,23 , nearPoint2)
					if (Fs[tempStart + 2].isErrorFace == 1) // 3�� Face�� ���� Face
					{
						//printf("Case7 Error Face �߻�\n");
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3
						nearPoint2 = findNearIndex(Fs, Vs, tempStart + 2);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex21].P = nearPoint2; // ����� �������� �ֱ� 1
						newVs[fIndex22].P = nearPoint2; // ����� �������� �ֱ� 2
						newVs[fIndex23].P = nearPoint2; // ����� �������� �ֱ� 3

						Fs[tempStart + 2].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;								// face ���� �Ѱ� ����

						if (nowErrorCaseTemp[7] == 0)
						{
							nowErrorCaseTemp[7] = 1;
							nowErrorCase[7]++;
						}

						for (int j = 0; j < faceNum; j++)			// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex22 || newFs[j].v1 == fIndex23)
							{
								newFs[j].v1 = fIndex21;
							}
							if (newFs[j].v2 == fIndex22 || newFs[j].v2 == fIndex23)
							{
								newFs[j].v2 = fIndex21;
							}
							if (newFs[j].v3 == fIndex22 || newFs[j].v3 == fIndex23)
							{
								newFs[j].v3 = fIndex21;
							}
						}
					}

					//4��° face (tempStart + 3 , findex31,32,33 , nearPoint3)
					if (Fs[tempStart + 3].isErrorFace == 1) // 4�� Face�� ���� Face
					{
						//printf("Case7 Error Face �߻�\n");
						fIndex31 = Fs[tempStart + 3].v1;	//face index �� �ֱ� 1
						fIndex32 = Fs[tempStart + 3].v2;	//face index �� �ֱ� 2
						fIndex33 = Fs[tempStart + 3].v3;	//face index �� �ֱ� 3
						nearPoint3 = findNearIndex(Fs, Vs, tempStart + 3);   //�ش� face�� ����� ���� ������ ã��

						newVs[fIndex31].P = nearPoint3; // ����� �������� �ֱ� 1
						newVs[fIndex32].P = nearPoint3; // ����� �������� �ֱ� 2
						newVs[fIndex33].P = nearPoint3; // ����� �������� �ֱ� 3

						Fs[tempStart + 3].use = 0;					// �� face list�� ���� �ʰ� ǥ��
						newFs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
						newFaceNum--;								// face ���� �Ѱ� ����

						if (nowErrorCaseTemp[7] == 0)
						{
							nowErrorCaseTemp[7] = 1;
							nowErrorCase[7]++;
						}

						for (int j = 0; j < faceNum; j++)			// face 1�� ������ 2,3�� �� �ٲٱ�
						{
							if (newFs[j].v1 == fIndex32 || newFs[j].v1 == fIndex33)
							{
								newFs[j].v1 = fIndex31;
							}
							if (newFs[j].v2 == fIndex32 || newFs[j].v2 == fIndex33)
							{
								newFs[j].v2 = fIndex31;
							}
							if (newFs[j].v3 == fIndex32 || newFs[j].v3 == fIndex33)
							{
								newFs[j].v3 = fIndex31;
							}
						}
					}
				}
				else
				{
					printf("!!Case7 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case7 ���� �Ϸ�

			//Case 1, 3, 7, 10, 12 �Ϸ�

			//���� �ﰢ�� 2�� ���� Case2, 11 ,13

			//Case2

			if (Vo[i].voxelCase == 2)//����纯��
			{
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[2] = 0;

				//printf("%d index ���� Case2\n", Vo[i].voxelIndex);
				//�Ѱ��� ó�� �ι�
				if (Vo[i].vErrorFaceNum > 0 && Vo[i].vFaceNum == 2)
				{
					tempStart = Vo[i].startIndex;
					//1�� Face�� ����
					if (Fs[tempStart].isErrorFace == 1)
					{
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3

						//01-02 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex02].P, Vo, i)))
						{
							//printf("Case2 ErrorFace 01-02�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex02)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex02)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex02)
									newFs[j].v3 = fIndex01;
							}
						}
						//01-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case2 ErrorFace 01-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex01;
							}
						}
						//02-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex02].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case2 ErrorFace 02-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex02].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex02].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex02;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex02;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex02;
							}
						}
						else
						{
							printf("!!Case2 ErrorFace 1�� face ���� ���� ���� �� ���� �߻�\n");
						}

					}
					//2�� Face�� ����
					if (Fs[tempStart + 1].isErrorFace == 1)
					{
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3

						//11-12 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex12].P, Vo, i)))
						{
							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex12)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex12)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex12)
									newFs[j].v3 = fIndex11;
							}
						}
						//11-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case2 ErrorFace 11-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex11;
							}
						}
						//12-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex12].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case2 ErrorFace 12-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex12].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex12].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[2] == 0)
							{
								nowErrorCase[2]++;
								nowErrorCaseTemp[2] = 1;
							}
							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex12;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex12;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex12;
							}
						}
						else
						{
							printf("!!Case2 ErrorFace 2�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
				}
				else
				{
					printf("!!Case2 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case2 ���� �Ϸ�

			//Case11

			if (Vo[i].voxelCase == 11)//����纯��
			{
				//Case11��
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int NearIndex01 = 0, NearIndex02 = 0, OutIndex01 = 0, Case11Val = 1;
				int fIndexNear01 = 0, fIndexNear02 = 0, fIndexNear03 = 0;
				int fIndexNear11 = 0, fIndexNear12 = 0, fIndexNear13 = 0;
				int fIndexOut01 = 0, fIndexOut02 = 0, fIndexOut03 = 0;
				nowErrorCaseTemp[11] = 0;

				if (Vo[i].vFaceNum == 3)
				{
					//printf("%d index ���� Case11\n", Vo[i].voxelIndex);
					Case11Val = 1;					// �ʱ�ȭ
					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�

					//printf("%lf %lf %lf\n", newVs[Fs[tempStart].v1].P.x, newVs[Fs[tempStart].v1].P.y, newVs[Fs[tempStart].v1].P.z);

					//���� 3���� face �� �پ��ִ� 2���� face�� ���� Face���� ���� �˾Ƴ���
					//nearIndex 1,2�� ������ face, OutIndex01 ���� �ִ� face

					//tempStart, tempStart + 1 Face�� ����纯���� �̷�ٸ�
					if (IsNearFace(tempStart, tempStart + 1, Fs) == 1)
					{
						NearIndex01 = tempStart;
						NearIndex02 = tempStart + 1;
						OutIndex01 = tempStart + 2;
					}
					//tempStart+1, tempStart + 2 Face�� ����纯���� �̷�ٸ�
					else if (IsNearFace(tempStart + 1, tempStart + 2, Fs) == 1)
					{
						NearIndex01 = tempStart + 1;
						NearIndex02 = tempStart + 2;
						OutIndex01 = tempStart;
					}
					//tempStart, tempStart + 2 Face�� ����纯���� �̷�ٸ�
					else if (IsNearFace(tempStart, tempStart + 2, Fs) == 1)
					{
						NearIndex01 = tempStart;
						NearIndex02 = tempStart + 2;
						OutIndex01 = tempStart + 1;
					}
					else
					{
						Case11Val = 0; // �˰��� ������
						printf("!!Case11 ���� ������ Face ����");
					}
					if (IsNearFace(tempStart, tempStart + 1, Fs) == 1 && IsNearFace(tempStart + 1, tempStart + 2, Fs) == 1 && IsNearFace(tempStart, tempStart + 2, Fs) == 1)
					{
						Case11Val = 0; // �˰��� ������
						printf("!!Case11 ���� Face 3�� ��� ������");
					}

					fIndexOut01 = Fs[OutIndex01].v1;	//OutIndex01 face index �� �ֱ� 1
					fIndexOut02 = Fs[OutIndex01].v2;	//OutIndex01 face index �� �ֱ� 2
					fIndexOut03 = Fs[OutIndex01].v3;	//OutIndex01 face index �� �ֱ� 3

					fIndexNear01 = Fs[NearIndex01].v1;	//nearIndex1 face index �� �ֱ� 1
					fIndexNear02 = Fs[NearIndex01].v2;	//nearIndex1 face index �� �ֱ� 2
					fIndexNear03 = Fs[NearIndex01].v3;	//nearIndex1face index �� �ֱ� 3

					fIndexNear11 = Fs[NearIndex02].v1;	//nearIndex2 face index �� �ֱ� 1
					fIndexNear12 = Fs[NearIndex02].v2;	//nearIndex2 face index �� �ֱ� 2
					fIndexNear13 = Fs[NearIndex02].v3;	//nearIndex2 face index �� �ֱ� 3
					//������� �ؼ� tempStart ~ tempStart+2�� neaerIndex1,2, OutIndex01�� ����.

					//skinny triangle ���� ����
					//Case11 skinny triangle�� 1���̻��̰� �⺻ Case �����̴�.
					if (Vo[i].vErrorFaceNum > 0 && Case11Val != 0)
					{
						// ȥ�� ������ �ִ� OutIndex01 Face�� skinny triangle
						if (Fs[OutIndex01].isErrorFace == 1)
						{
							//printf("Case11 ErrorFace, OutIndex01�� skinny triangle\n");
							//���� ����� ���� ������ ã��
							nearPoint0 = findNearIndex(Fs, Vs, OutIndex01);
							//3���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndexOut01].P = nearPoint0;
							newVs[fIndexOut02].P = nearPoint0;
							newVs[fIndexOut03].P = nearPoint0;
							//printf("%lf, %lf, %lf\n", nearPoint0.x, nearPoint0.y, nearPoint0.z);
							Fs[OutIndex01].use = 0;				// OutIndex01�� face list�� ���� �ʰ� ǥ��
							newFs[OutIndex01].use = 0;			// OutIndex01�� face list�� ���� �ʰ� ǥ��
							newFaceNum--;						// face ���� �Ѱ� ����
							nowErrorCase[11]++;
							nowErrorCaseTemp[11] = 1;

							for (int j = 0; j < faceNum; j++)	// fIndexOut02,03 ���� fIndexOut01 ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndexOut02 || newFs[j].v1 == fIndexOut03)
								{
									newFs[j].v1 = fIndexOut01;
								}
								if (newFs[j].v2 == fIndexOut02 || newFs[j].v2 == fIndexOut03)
								{
									newFs[j].v2 = fIndexOut01;
								}
								if (newFs[j].v3 == fIndexOut02 || newFs[j].v3 == fIndexOut03)
								{
									newFs[j].v3 = fIndexOut01;
								}
							}
						}
						// ������ Face �� �� NearIndex01 face�� skinny triangle �̶��
						if (Fs[NearIndex01].isErrorFace == 1)
						{
							//NearIndex01 fIndexNear01-02�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNear01].P, Vo, i), getCloseCorner(Vs[fIndexNear02].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 01-02�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear01].P = getCloseCorner(Vs[fIndexNear01].P, Vo, i);
								newVs[fIndexNear02].P = getCloseCorner(Vs[fIndexNear02].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear02�� ���� fIndexNear01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear02)
										newFs[j].v1 = fIndexNear01;
									if (newFs[j].v2 == fIndexNear02)
										newFs[j].v2 = fIndexNear01;
									if (newFs[j].v3 == fIndexNear02)
										newFs[j].v3 = fIndexNear01;
								}
							}
							//NearIndex01 fIndexNear01-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear01].P, Vo, i), getCloseCorner(Vs[fIndexNear03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 01-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear01].P = getCloseCorner(Vs[fIndexNear01].P, Vo, i);
								newVs[fIndexNear03].P = getCloseCorner(Vs[fIndexNear03].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear03�� ���� fIndexNear01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear03)
										newFs[j].v1 = fIndexNear01;
									if (newFs[j].v2 == fIndexNear03)
										newFs[j].v2 = fIndexNear01;
									if (newFs[j].v3 == fIndexNear03)
										newFs[j].v3 = fIndexNear01;
								}
							}
							//NearIndex01 fIndexNear02-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear02].P, Vo, i), getCloseCorner(Vs[fIndexNear03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 02-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear02].P = getCloseCorner(Vs[fIndexNear02].P, Vo, i);
								newVs[fIndexNear03].P = getCloseCorner(Vs[fIndexNear03].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear03�� ���� fIndexNear02�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear03)
										newFs[j].v1 = fIndexNear02;
									if (newFs[j].v2 == fIndexNear03)
										newFs[j].v2 = fIndexNear02;
									if (newFs[j].v3 == fIndexNear03)
										newFs[j].v3 = fIndexNear02;
								}
							}
							else
							{
								printf("!!Case11 NearIndex01 face ���� ����\n");
							}
						}
						// ������ Face �� �� NearIndex02 face�� skinny triangle �̶��
						if (Fs[NearIndex02].isErrorFace == 1)
						{
							//NearIndex02 fIndexNear11-12�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNear11].P, Vo, i), getCloseCorner(Vs[fIndexNear12].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 11-12�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear11].P = getCloseCorner(Vs[fIndexNear11].P, Vo, i);
								newVs[fIndexNear12].P = getCloseCorner(Vs[fIndexNear12].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear12�� ���� fIndexNear11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear12)
										newFs[j].v1 = fIndexNear11;
									if (newFs[j].v2 == fIndexNear12)
										newFs[j].v2 = fIndexNear11;
									if (newFs[j].v3 == fIndexNear12)
										newFs[j].v3 = fIndexNear11;
								}
							}
							//NearIndex02 fIndexNear11-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear11].P, Vo, i), getCloseCorner(Vs[fIndexNear13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 11-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear11].P = getCloseCorner(Vs[fIndexNear11].P, Vo, i);
								newVs[fIndexNear13].P = getCloseCorner(Vs[fIndexNear13].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear13�� ���� fIndexNear11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear13)
										newFs[j].v1 = fIndexNear11;
									if (newFs[j].v2 == fIndexNear13)
										newFs[j].v2 = fIndexNear11;
									if (newFs[j].v3 == fIndexNear13)
										newFs[j].v3 = fIndexNear11;
								}
							}
							//NearIndex02 fIndexNear12-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear12].P, Vo, i), getCloseCorner(Vs[fIndexNear13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 12-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear12].P = getCloseCorner(Vs[fIndexNear12].P, Vo, i);
								newVs[fIndexNear13].P = getCloseCorner(Vs[fIndexNear13].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[11] == 0)
								{
									nowErrorCase[11]++;
									nowErrorCaseTemp[11] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear13�� ���� fIndexNear12�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear13)
										newFs[j].v1 = fIndexNear12;
									if (newFs[j].v2 == fIndexNear13)
										newFs[j].v2 = fIndexNear12;
									if (newFs[j].v3 == fIndexNear13)
										newFs[j].v3 = fIndexNear12;
								}
							}
							else
							{
								printf("!!Case11 NearIndex02 face ���� ����\n");
							}
						}
					}
					else
					{
						printf("!!Case11 �� face ������ 3�� �϶� ���� + �⺻���¿� �ٸ�\n");
					}
				}
				else //Case11���� face�� 3���� �ƴ� ���
				{
					//if (Vo[i].vFaceNum != 5)
					printf("!!Case11 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
					//Case11�� ��� face 5���� ���� �� �ִ�.
				}
			}
			//Case11 ���� �Ϸ�

			//Case13

			if (Vo[i].voxelCase == 13)//����纯�� 2��
			{
				//Case13��
				Point nearPoint0, nearPoint1, nearPoint2, nearPoint3;
				int NearIndexA01 = 0, NearIndexA02 = 0, NearIndexB01 = 0, NearIndexB02 = 0, Case13Val = 1;
				int fIndexNearA01 = 0, fIndexNearA02 = 0, fIndexNearA03 = 0;
				int fIndexNearA11 = 0, fIndexNearA12 = 0, fIndexNearA13 = 0;
				int fIndexNearB01 = 0, fIndexNearB02 = 0, fIndexNearB03 = 0;
				int fIndexNearB11 = 0, fIndexNearB12 = 0, fIndexNearB13 = 0;
				nowErrorCaseTemp[13] = 0;

				if (Vo[i].vFaceNum == 4)
				{
					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�
					Case13Val = 1;					//�ʱ�ȭ

					//���� 4���� face �� �پ��ִ� 2���� face�� �������� ���� �˾Ƴ���
					//NearIndexA01, NearIndexA02 ������ 2���� face, NearIndexB01, NearIndexB02 ������ 2���� face

					//tempStart�� tempStart+1�� �����ϸ� �翬�� tempStart+2, tempStart+3�� �����ؾ���
					if (IsNearFace(tempStart, tempStart + 1, Fs) == 1)
					{
						if (IsNearFace(tempStart + 2, tempStart + 3, Fs) == 1)
						{
							NearIndexA01 = tempStart;
							NearIndexA02 = tempStart + 1;
							NearIndexB01 = tempStart + 2;
							NearIndexB02 = tempStart + 3;
						}
						else
						{
							Case13Val = 0;
							printf("!!Case13 temp 2-3 ���� �߻����� ����\n");
						}
					}
					//tempStart�� tempStart+2�� �����ϸ� �翬�� tempStart+1, tempStart+3�� �����ؾ���
					else if (IsNearFace(tempStart, tempStart + 2, Fs) == 1)
					{
						if (IsNearFace(tempStart + 1, tempStart + 3, Fs) == 1)
						{
							NearIndexA01 = tempStart;
							NearIndexA02 = tempStart + 2;
							NearIndexB01 = tempStart + 1;
							NearIndexB02 = tempStart + 3;
						}
						else
						{
							Case13Val = 0;
							printf("!!Case13 temp 1-3 ���� �߻����� ����\n");
						}
					}
					//tempStart�� tempStart+3�� �����ϸ� �翬�� tempStart+1, tempStart+2�� �����ؾ���
					else if (IsNearFace(tempStart, tempStart + 3, Fs) == 1)
					{
						if (IsNearFace(tempStart + 1, tempStart + 2, Fs) == 1)
						{
							NearIndexA01 = tempStart;
							NearIndexA02 = tempStart + 3;
							NearIndexB01 = tempStart + 1;
							NearIndexB02 = tempStart + 2;
						}
						else
						{
							Case13Val = 0;
							printf("!!Case13 temp 1-2 ���� �߻����� ����\n");
						}
					}
					else
					{
						Case13Val = 0;
						printf("!!Case13 temp0�� �����ϴ� face ���� ����\n");
					}

					//�ش� Index�ֱ�
					fIndexNearA01 = Fs[NearIndexA01].v1;	//NearIndexA01 face index �� �ֱ� 1
					fIndexNearA02 = Fs[NearIndexA01].v2;	//NearIndexA01 face index �� �ֱ� 2
					fIndexNearA03 = Fs[NearIndexA01].v3;	//NearIndexA01 face index �� �ֱ� 3

					fIndexNearA11 = Fs[NearIndexA02].v1;	//NearIndexA02 face index �� �ֱ� 1
					fIndexNearA12 = Fs[NearIndexA02].v2;	//NearIndexA02 face index �� �ֱ� 2
					fIndexNearA13 = Fs[NearIndexA02].v3;	//NearIndexA02 face index �� �ֱ� 3

					fIndexNearB01 = Fs[NearIndexB01].v1;	//NearIndexB01 face index �� �ֱ� 1
					fIndexNearB02 = Fs[NearIndexB01].v2;	//NearIndexB01 face index �� �ֱ� 2
					fIndexNearB03 = Fs[NearIndexB01].v3;	//NearIndexB01 face index �� �ֱ� 3

					fIndexNearB11 = Fs[NearIndexB02].v1;	//NearIndexB02 face index �� �ֱ� 1
					fIndexNearB12 = Fs[NearIndexB02].v2;	//NearIndexB02 face index �� �ֱ� 2
					fIndexNearB13 = Fs[NearIndexB02].v3;	//NearIndexB02 face index �� �ֱ� 3
					//������� Case13 ������ Index ���� �Ϸ�


					//skinny triangle ���� ����
					//Case13 skinny triangle�� 1���̰� �⺻ Case �����̴�.
					if (Vo[i].vErrorFaceNum > 0 && Case13Val != 0)
					{
						//NearIndexA01�� skinny triangle
						if (Fs[NearIndexA01].isErrorFace == 1)
						{
							//NearIndexA01 fIndexNearA01-02�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNearA01].P, Vo, i), getCloseCorner(Vs[fIndexNearA02].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA01 01-02�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA01].P = getCloseCorner(Vs[fIndexNearA01].P, Vo, i);
								newVs[fIndexNearA02].P = getCloseCorner(Vs[fIndexNearA02].P, Vo, i);

								Fs[NearIndexA01].use = 0;			//NearIndexA01�� face list�� ���� �ʱ�
								newFs[NearIndexA01].use = 0;		//NearIndexA01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA02�� ���� fIndexNearA01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA02)
										newFs[j].v1 = fIndexNearA01;
									if (newFs[j].v2 == fIndexNearA02)
										newFs[j].v2 = fIndexNearA01;
									if (newFs[j].v3 == fIndexNearA02)
										newFs[j].v3 = fIndexNearA01;
								}
							}
							//NearIndexA01 fIndexNearA01-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearA01].P, Vo, i), getCloseCorner(Vs[fIndexNearA03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA01 01-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA01].P = getCloseCorner(Vs[fIndexNearA01].P, Vo, i);
								newVs[fIndexNearA03].P = getCloseCorner(Vs[fIndexNearA03].P, Vo, i);

								Fs[NearIndexA01].use = 0;			//NearIndexA01�� face list�� ���� �ʱ�
								newFs[NearIndexA01].use = 0;		//NearIndexA01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA03�� ���� fIndexNearA01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA03)
										newFs[j].v1 = fIndexNearA01;
									if (newFs[j].v2 == fIndexNearA03)
										newFs[j].v2 = fIndexNearA01;
									if (newFs[j].v3 == fIndexNearA03)
										newFs[j].v3 = fIndexNearA01;
								}
							}
							//NearIndexA01 fIndexNearA02-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearA02].P, Vo, i), getCloseCorner(Vs[fIndexNearA03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA01 02-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA02].P = getCloseCorner(Vs[fIndexNearA02].P, Vo, i);
								newVs[fIndexNearA03].P = getCloseCorner(Vs[fIndexNearA03].P, Vo, i);

								Fs[NearIndexA01].use = 0;			//NearIndexA01�� face list�� ���� �ʱ�
								newFs[NearIndexA01].use = 0;			//NearIndexA01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA03�� ���� fIndexNearA02�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA03)
										newFs[j].v1 = fIndexNearA02;
									if (newFs[j].v2 == fIndexNearA03)
										newFs[j].v2 = fIndexNearA02;
									if (newFs[j].v3 == fIndexNearA03)
										newFs[j].v3 = fIndexNearA02;
								}
							}
							else
							{
								printf("!!Case13 skinny triangle NearIndexA01 face ���� ����\n");
							}
						}
						//NearIndexA02�� skinny triangle
						if (Fs[NearIndexA02].isErrorFace == 1)
						{
							//NearIndexA02 fIndexNearA11-12�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNearA11].P, Vo, i), getCloseCorner(Vs[fIndexNearA12].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA02 11-12�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA11].P = getCloseCorner(Vs[fIndexNearA11].P, Vo, i);
								newVs[fIndexNearA12].P = getCloseCorner(Vs[fIndexNearA12].P, Vo, i);

								Fs[NearIndexA02].use = 0;			//NearIndexA02�� face list�� ���� �ʱ�
								newFs[NearIndexA02].use = 0;		//NearIndexA02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA12�� ���� fIndexNearA11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA12)
										newFs[j].v1 = fIndexNearA11;
									if (newFs[j].v2 == fIndexNearA12)
										newFs[j].v2 = fIndexNearA11;
									if (newFs[j].v3 == fIndexNearA12)
										newFs[j].v3 = fIndexNearA11;
								}
							}
							//NearIndexA02 fIndexNearA11-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearA11].P, Vo, i), getCloseCorner(Vs[fIndexNearA13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA02 11-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA11].P = getCloseCorner(Vs[fIndexNearA11].P, Vo, i);
								newVs[fIndexNearA13].P = getCloseCorner(Vs[fIndexNearA13].P, Vo, i);

								Fs[NearIndexA02].use = 0;			//NearIndexA02�� face list�� ���� �ʱ�
								newFs[NearIndexA02].use = 0;		//NearIndexA02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA13�� ���� fIndexNearA11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA13)
										newFs[j].v1 = fIndexNearA11;
									if (newFs[j].v2 == fIndexNearA13)
										newFs[j].v2 = fIndexNearA11;
									if (newFs[j].v3 == fIndexNearA13)
										newFs[j].v3 = fIndexNearA11;
								}
							}
							//NearIndexA02 fIndexNearA12-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearA12].P, Vo, i), getCloseCorner(Vs[fIndexNearA13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexA02 12-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearA12].P = getCloseCorner(Vs[fIndexNearA12].P, Vo, i);
								newVs[fIndexNearA13].P = getCloseCorner(Vs[fIndexNearA13].P, Vo, i);

								Fs[NearIndexA02].use = 0;			//NearIndexA02�� face list�� ���� �ʱ�
								newFs[NearIndexA02].use = 0;			//NearIndexA02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearA13�� ���� fIndexNearA12�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearA13)
										newFs[j].v1 = fIndexNearA12;
									if (newFs[j].v2 == fIndexNearA13)
										newFs[j].v2 = fIndexNearA12;
									if (newFs[j].v3 == fIndexNearA13)
										newFs[j].v3 = fIndexNearA12;
								}
							}
							else
							{
								printf("!!Case13 skinny triangle NearIndexA02 face ���� ����\n");
							}
						}
						//NearIndexB01�� skinny triangle
						if (Fs[NearIndexB01].isErrorFace == 1)
						{
							//NearIndexB01 fIndexNearB01-02�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNearB01].P, Vo, i), getCloseCorner(Vs[fIndexNearB02].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB01 01-02�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB01].P = getCloseCorner(Vs[fIndexNearB01].P, Vo, i);
								newVs[fIndexNearB02].P = getCloseCorner(Vs[fIndexNearB02].P, Vo, i);

								Fs[NearIndexB01].use = 0;			//NearIndexB01�� face list�� ���� �ʱ�
								newFs[NearIndexB01].use = 0;		//NearIndexB01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB02�� ���� fIndexNearB01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB02)
										newFs[j].v1 = fIndexNearB01;
									if (newFs[j].v2 == fIndexNearB02)
										newFs[j].v2 = fIndexNearB01;
									if (newFs[j].v3 == fIndexNearB02)
										newFs[j].v3 = fIndexNearB01;
								}
							}
							//NearIndexB01 fIndexNearB01-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearB01].P, Vo, i), getCloseCorner(Vs[fIndexNearB03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB01 01-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB01].P = getCloseCorner(Vs[fIndexNearB01].P, Vo, i);
								newVs[fIndexNearB03].P = getCloseCorner(Vs[fIndexNearB03].P, Vo, i);

								Fs[NearIndexB01].use = 0;			//NearIndexB01�� face list�� ���� �ʱ�
								newFs[NearIndexB01].use = 0;		//NearIndexB01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB03�� ���� fIndexNearB01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB03)
										newFs[j].v1 = fIndexNearB01;
									if (newFs[j].v2 == fIndexNearB03)
										newFs[j].v2 = fIndexNearB01;
									if (newFs[j].v3 == fIndexNearB03)
										newFs[j].v3 = fIndexNearB01;
								}
							}
							//NearIndexB01 fIndexNearB02-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearB02].P, Vo, i), getCloseCorner(Vs[fIndexNearB03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB01 02-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB02].P = getCloseCorner(Vs[fIndexNearB02].P, Vo, i);
								newVs[fIndexNearB03].P = getCloseCorner(Vs[fIndexNearB03].P, Vo, i);

								Fs[NearIndexB01].use = 0;			//NearIndexB01�� face list�� ���� �ʱ�
								newFs[NearIndexB01].use = 0;			//NearIndexB01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB03�� ���� fIndexNearB02�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB03)
										newFs[j].v1 = fIndexNearB02;
									if (newFs[j].v2 == fIndexNearB03)
										newFs[j].v2 = fIndexNearB02;
									if (newFs[j].v3 == fIndexNearB03)
										newFs[j].v3 = fIndexNearB02;
								}
							}
							else
							{
								printf("!!Case13 skinny triangle NearIndexB01 face ���� ����\n");
							}
						}
						//NearIndexB02�� skinny triangle
						if (Fs[NearIndexB02].isErrorFace == 1)
						{
							//NearIndexB02 fIndexNearB11-12�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNearB11].P, Vo, i), getCloseCorner(Vs[fIndexNearB12].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB02 11-12�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB11].P = getCloseCorner(Vs[fIndexNearB11].P, Vo, i);
								newVs[fIndexNearB12].P = getCloseCorner(Vs[fIndexNearB12].P, Vo, i);

								Fs[NearIndexB02].use = 0;			//NearIndexB02�� face list�� ���� �ʱ�
								newFs[NearIndexB02].use = 0;		//NearIndexB02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB12�� ���� fIndexNearB11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB12)
										newFs[j].v1 = fIndexNearB11;
									if (newFs[j].v2 == fIndexNearB12)
										newFs[j].v2 = fIndexNearB11;
									if (newFs[j].v3 == fIndexNearB12)
										newFs[j].v3 = fIndexNearB11;
								}
							}
							//NearIndexB02 fIndexNearB11-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearB11].P, Vo, i), getCloseCorner(Vs[fIndexNearB13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB02 11-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB11].P = getCloseCorner(Vs[fIndexNearB11].P, Vo, i);
								newVs[fIndexNearB13].P = getCloseCorner(Vs[fIndexNearB13].P, Vo, i);

								Fs[NearIndexB02].use = 0;			//NearIndexB02�� face list�� ���� �ʱ�
								newFs[NearIndexB02].use = 0;		//NearIndexB02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB13�� ���� fIndexNearB11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB13)
										newFs[j].v1 = fIndexNearB11;
									if (newFs[j].v2 == fIndexNearB13)
										newFs[j].v2 = fIndexNearB11;
									if (newFs[j].v3 == fIndexNearB13)
										newFs[j].v3 = fIndexNearB11;
								}
							}
							//NearIndexB02 fIndexNearB12-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNearB12].P, Vo, i), getCloseCorner(Vs[fIndexNearB13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndexB02 12-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNearB12].P = getCloseCorner(Vs[fIndexNearB12].P, Vo, i);
								newVs[fIndexNearB13].P = getCloseCorner(Vs[fIndexNearB13].P, Vo, i);

								Fs[NearIndexB02].use = 0;			//NearIndexB02�� face list�� ���� �ʱ�
								newFs[NearIndexB02].use = 0;			//NearIndexB02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[13] == 0)
								{
									nowErrorCase[13]++;
									nowErrorCaseTemp[13] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNearB13�� ���� fIndexNearB12�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNearB13)
										newFs[j].v1 = fIndexNearB12;
									if (newFs[j].v2 == fIndexNearB13)
										newFs[j].v2 = fIndexNearB12;
									if (newFs[j].v3 == fIndexNearB13)
										newFs[j].v3 = fIndexNearB12;
								}
							}
							else
							{
								printf("!!Case13 skinny triangle NearIndexB02 face ���� ����\n");
							}
						}
					}
					else
					{
						printf("!!Case13 �� face ������ 4�� �϶� ���� + �⺻���¿� �ٸ�\n");
					}

				}
				else
				{
					printf("!!Case13 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case13 ���� �Ϸ�

			//���� �ﰢ�� 3�� ���� Case4, 6

			//Case4

			if (Vo[i].voxelCase == 4)//�ﰢ�� 3�� ����
			{
				Point nearPoint0;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				nowErrorCaseTemp[4] = 0;

				//�Ѱ��� ó�� ����
				if (Vo[i].vErrorFaceNum > 0 && Vo[i].vFaceNum == 3)
				{
					tempStart = Vo[i].startIndex;
					//1�� Face�� ����
					if (Fs[tempStart].isErrorFace == 1)
					{
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3

						//01-02 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex02].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 01-02�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex02)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex02)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex02)
									newFs[j].v3 = fIndex01;
							}
						}
						//01-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 01-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex01;
							}
						}
						//02-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex02].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 02-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex02].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex02].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex02;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex02;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex02;
							}
						}
						else
						{
							printf("!!Case4 ErrorFace 1�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//2�� Face�� ����
					if (Fs[tempStart + 1].isErrorFace == 1)
					{
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3

						//11-12 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex12].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 11-12�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex12)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex12)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex12)
									newFs[j].v3 = fIndex11;
							}
						}
						//11-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 11-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex11;
							}
						}
						//12-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex12].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 12-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex12].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex12].P, Vo, i);


							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex12;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex12;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex12;
							}
						}
						else
						{
							printf("!!Case4 ErrorFace 2�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//3�� Face�� ����
					if (Fs[tempStart + 2].isErrorFace == 1)
					{
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3

						//21-22 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex22].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex22)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex22)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex22)
									newFs[j].v3 = fIndex21;
							}
						}
						//21-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex21;
							}
						}
						//22-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex22].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case4 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex22].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex22].P, Vo, i);


							Fs[tempStart + 2].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[4] == 0)
							{
								nowErrorCase[4]++;
								nowErrorCaseTemp[4] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex22;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex22;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex22;
							}
						}
						else
						{
							printf("!!Case4 ErrorFace 3�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
				}
				else
				{
					printf("!!Case4 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}


			}
			//Case4 ���� �Ϸ�

			//Case6

			if (Vo[i].voxelCase == 6)
			{
				Point nearPoint0;
				int NearIndex01 = 0, NearIndex02 = 0, NearIndex03 = 0, OutIndex01 = 0, Case6Val = 1;
				int fIndexNear01 = 0, fIndexNear02 = 0, fIndexNear03 = 0;
				int fIndexNear11 = 0, fIndexNear12 = 0, fIndexNear13 = 0;
				int fIndexNear21 = 0, fIndexNear22 = 0, fIndexNear23 = 0;
				int fIndexOut01 = 0, fIndexOut02 = 0, fIndexOut03 = 0;
				nowErrorCaseTemp[6] = 0;

				if (Vo[i].vFaceNum == 4)
				{
					//printf("%d index ���� Case6\n", Vo[i].voxelIndex);
					Case6Val = 1;					// �ʱ�ȭ
					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�

					//printf("%lf %lf %lf\n", newVs[Fs[tempStart].v1].P.x, newVs[Fs[tempStart].v1].P.y, newVs[Fs[tempStart].v1].P.z);

					//���� 4���� face �� �پ��ִ� 3���� face�� ���� Face���� ���� �˾Ƴ���
					//nearIndex 1,2,3�� ������ face, OutIndex01�� ���� �ִ� face

					//���� tempStart�� tesmpStart + 1�� �����Ѵٸ�
					if (IsNearFace(tempStart, tempStart + 1, Fs) == 1)
					{
						//tempStart�� tempStart +2�� ���� �Ǵ� tempStart + 1�� tempStart + 2�� ���� == tempStart + 3 �� ������ ����
						if ((IsNearFace(tempStart, tempStart + 2, Fs) == 1) || (IsNearFace(tempStart + 1, tempStart + 2, Fs) == 1))
						{
							NearIndex01 = tempStart;
							NearIndex02 = tempStart + 1;
							NearIndex03 = tempStart + 2;
							OutIndex01 = tempStart + 3;
						}
						//tempStart�� tempStart +3�� ���� �Ǵ� tempStart + 1�� tempStart + 3�� ���� == tempStart + 2 �� ������ ����
						else if ((IsNearFace(tempStart, tempStart + 3, Fs) == 1) || (IsNearFace(tempStart + 1, tempStart + 3, Fs) == 1))
						{
							NearIndex01 = tempStart;
							NearIndex02 = tempStart + 1;
							NearIndex03 = tempStart + 3;
							OutIndex01 = tempStart + 2;
						}
						//tempStart�� tempStart+1�� ���� Case6�� �⺻ ���°� �ƴ�
						else
						{
							Case6Val = 0;
							printf("!!Case6 tempStart�� tempStart+1�� �����ϴ� ����\n");
						}
					}
					//���� tempStart�� tesmpStart + 1�� ���������ʴ´�. (���������ʾƵ� ���� 3���� �� ���� ���� �ִ�.)
					else if (IsNearFace(tempStart, tempStart + 1, Fs) == 0)
					{
						//tempStart�� tempStart + 2�� tempStart + 3 �� �� �������� �ʴ´�. == tempStart �� ������ ����
						if ((IsNearFace(tempStart, tempStart + 2, Fs) == 0) && (IsNearFace(tempStart, tempStart + 3, Fs) == 0))
						{
							NearIndex01 = tempStart + 1;
							NearIndex02 = tempStart + 2;
							NearIndex03 = tempStart + 3;
							OutIndex01 = tempStart;
						}
						//tempStart + 1�� tempStart + 2�� tempStart + 3 �� �� �������� �ʴ´�. == tempStart + 1 �� ������ ����
						else if ((IsNearFace(tempStart + 1, tempStart + 2, Fs) == 0) && (IsNearFace(tempStart + 1, tempStart + 3, Fs) == 0))
						{
							NearIndex01 = tempStart;
							NearIndex02 = tempStart + 2;
							NearIndex03 = tempStart + 3;
							OutIndex01 = tempStart + 1;
						}
						//tempStart�� tempStart + 1 �� ������ 3���� face �� �� ���� ����Ҷ�
						//tempStart + 2 �� �� �����϶� == tempStart�� tempStart + 2�� �����ϰ�, tempStart + 1�� tempStart +2�� ���� == tempStart + 3 �� ������ ����
						else if ((IsNearFace(tempStart, tempStart + 2, Fs) == 1) && (IsNearFace(tempStart + 1, tempStart + 2, Fs) == 1))
						{
							NearIndex01 = tempStart;
							NearIndex02 = tempStart + 2;
							NearIndex03 = tempStart + 1;
							OutIndex01 = tempStart + 3;
						}
						//tempStart + 3 �� �� �����϶� == tempStart�� tempStart + 3�� �����ϰ�, tempStart + 1�� tempStart +3�� ���� == tempStart + 2 �� ������ ����
						else if ((IsNearFace(tempStart, tempStart + 3, Fs) == 1) && (IsNearFace(tempStart + 3, tempStart + 2, Fs) == 1))
						{
							NearIndex01 = tempStart;
							NearIndex02 = tempStart + 3;
							NearIndex03 = tempStart + 1;
							OutIndex01 = tempStart + 2;
						}
						//�����ؾ��ϴ� ������ ���ڶ�, Case6�� �⺻ ���°� �ƴ�
						else
						{
							Case6Val = 0;
							printf("!!Case6 �����ؾ��ϴ� face ������ ���ڶ�� ����\n");
						}
					}
					//������ ���� ���ų� �ٸ� ����
					else
					{
						Case6Val = 0;
						printf("!!Case6 ���� ó���� ������ ���ų� �ٸ� ����ġ ���� ����\n");
					}

					fIndexOut01 = Fs[OutIndex01].v1;	//OutIndex01 face index �� �ֱ� 1
					fIndexOut02 = Fs[OutIndex01].v2;	//OutIndex01 face index �� �ֱ� 2
					fIndexOut03 = Fs[OutIndex01].v3;	//OutIndex01 face index �� �ֱ� 3

					fIndexNear01 = Fs[NearIndex01].v1;	//nearIndex1 face index �� �ֱ� 1
					fIndexNear02 = Fs[NearIndex01].v2;	//nearIndex1 face index �� �ֱ� 2
					fIndexNear03 = Fs[NearIndex01].v3;	//nearIndex1face index �� �ֱ� 3

					fIndexNear11 = Fs[NearIndex02].v1;	//nearIndex2 face index �� �ֱ� 1
					fIndexNear12 = Fs[NearIndex02].v2;	//nearIndex2 face index �� �ֱ� 2
					fIndexNear13 = Fs[NearIndex02].v3;	//nearIndex2 face index �� �ֱ� 3

					fIndexNear21 = Fs[NearIndex03].v1;	//nearIndex2 face index �� �ֱ� 1
					fIndexNear22 = Fs[NearIndex03].v2;	//nearIndex2 face index �� �ֱ� 2
					fIndexNear23 = Fs[NearIndex03].v3;	//nearIndex2 face index �� �ֱ� 3
					//������� �ؼ� tempStart ~ tempStart+3�� neaerIndex01,02,03, OutIndex01�� ����.

					//SkinnyTriangle ���� ����
					//Case6 skinny triangle�� 1�� �̻��̰� �⺻ Case �����̴�.
					if (Vo[i].vErrorFaceNum > 0 && Case6Val != 0)
					{
						// ȥ�� ������ �ִ� OutIndex01 Face�� skinny triangle
						if (Fs[OutIndex01].isErrorFace == 1)
						{
							//printf("Case11 ErrorFace, OutIndex01�� skinny triangle\n");
							//���� ����� ���� ������ ã��
							nearPoint0 = findNearIndex(Fs, Vs, OutIndex01);
							//3���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndexOut01].P = nearPoint0;
							newVs[fIndexOut02].P = nearPoint0;
							newVs[fIndexOut03].P = nearPoint0;
							//printf("%lf, %lf, %lf\n", nearPoint0.x, nearPoint0.y, nearPoint0.z);
							Fs[OutIndex01].use = 0;				// OutIndex01�� face list�� ���� �ʰ� ǥ��
							newFs[OutIndex01].use = 0;			// OutIndex01�� face list�� ���� �ʰ� ǥ��
							newFaceNum--;						// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[6] == 0)
							{
								nowErrorCase[6]++;
								nowErrorCaseTemp[6] = 1;
							}

							for (int j = 0; j < faceNum; j++)	// fIndexOut02,03 ���� fIndexOut01 ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndexOut02 || newFs[j].v1 == fIndexOut03)
								{
									newFs[j].v1 = fIndexOut01;
								}
								if (newFs[j].v2 == fIndexOut02 || newFs[j].v2 == fIndexOut03)
								{
									newFs[j].v2 = fIndexOut01;
								}
								if (newFs[j].v3 == fIndexOut02 || newFs[j].v3 == fIndexOut03)
								{
									newFs[j].v3 = fIndexOut01;
								}
							}
						}
						// ������ Face �� �� NearIndex01 face�� skinny triangle �̶��
						if (Fs[NearIndex01].isErrorFace == 1)
						{
							//NearIndex01 fIndexNear01-02�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNear01].P, Vo, i), getCloseCorner(Vs[fIndexNear02].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 01-02�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear01].P = getCloseCorner(Vs[fIndexNear01].P, Vo, i);
								newVs[fIndexNear02].P = getCloseCorner(Vs[fIndexNear02].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear02�� ���� fIndexNear01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear02)
										newFs[j].v1 = fIndexNear01;
									if (newFs[j].v2 == fIndexNear02)
										newFs[j].v2 = fIndexNear01;
									if (newFs[j].v3 == fIndexNear02)
										newFs[j].v3 = fIndexNear01;
								}
							}
							//NearIndex01 fIndexNear01-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear01].P, Vo, i), getCloseCorner(Vs[fIndexNear03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 01-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear01].P = getCloseCorner(Vs[fIndexNear01].P, Vo, i);
								newVs[fIndexNear03].P = getCloseCorner(Vs[fIndexNear03].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear03�� ���� fIndexNear01�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear03)
										newFs[j].v1 = fIndexNear01;
									if (newFs[j].v2 == fIndexNear03)
										newFs[j].v2 = fIndexNear01;
									if (newFs[j].v3 == fIndexNear03)
										newFs[j].v3 = fIndexNear01;
								}
							}
							//NearIndex01 fIndexNear02-03�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear02].P, Vo, i), getCloseCorner(Vs[fIndexNear03].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex01 02-03�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear02].P = getCloseCorner(Vs[fIndexNear02].P, Vo, i);
								newVs[fIndexNear03].P = getCloseCorner(Vs[fIndexNear03].P, Vo, i);

								Fs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFs[NearIndex01].use = 0;			//NearIndex01�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear03�� ���� fIndexNear02�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear03)
										newFs[j].v1 = fIndexNear02;
									if (newFs[j].v2 == fIndexNear03)
										newFs[j].v2 = fIndexNear02;
									if (newFs[j].v3 == fIndexNear03)
										newFs[j].v3 = fIndexNear02;
								}
							}
							else
							{
								printf("!!Case6 NearIndex01 face ���� ����\n");
							}
						}
						// ������ Face �� �� NearIndex02 face�� skinny triangle �̶��
						if (Fs[NearIndex02].isErrorFace == 1)
						{
							//NearIndex02 fIndexNear11-12�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNear11].P, Vo, i), getCloseCorner(Vs[fIndexNear12].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 11-12�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear11].P = getCloseCorner(Vs[fIndexNear11].P, Vo, i);
								newVs[fIndexNear12].P = getCloseCorner(Vs[fIndexNear12].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear12�� ���� fIndexNear11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear12)
										newFs[j].v1 = fIndexNear11;
									if (newFs[j].v2 == fIndexNear12)
										newFs[j].v2 = fIndexNear11;
									if (newFs[j].v3 == fIndexNear12)
										newFs[j].v3 = fIndexNear11;
								}
							}
							//NearIndex02 fIndexNear11-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear11].P, Vo, i), getCloseCorner(Vs[fIndexNear13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 11-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear11].P = getCloseCorner(Vs[fIndexNear11].P, Vo, i);
								newVs[fIndexNear13].P = getCloseCorner(Vs[fIndexNear13].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear13�� ���� fIndexNear11�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear13)
										newFs[j].v1 = fIndexNear11;
									if (newFs[j].v2 == fIndexNear13)
										newFs[j].v2 = fIndexNear11;
									if (newFs[j].v3 == fIndexNear13)
										newFs[j].v3 = fIndexNear11;
								}
							}
							//NearIndex02 fIndexNear12-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear12].P, Vo, i), getCloseCorner(Vs[fIndexNear13].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex02 12-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear12].P = getCloseCorner(Vs[fIndexNear12].P, Vo, i);
								newVs[fIndexNear13].P = getCloseCorner(Vs[fIndexNear13].P, Vo, i);

								Fs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFs[NearIndex02].use = 0;			//NearIndex02�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear13�� ���� fIndexNear12�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear13)
										newFs[j].v1 = fIndexNear12;
									if (newFs[j].v2 == fIndexNear13)
										newFs[j].v2 = fIndexNear12;
									if (newFs[j].v3 == fIndexNear13)
										newFs[j].v3 = fIndexNear12;
								}
							}
							else
							{
								printf("!!Case 11 NearIndex02 face ���� ����\n");
							}
						}
						// ������ Face �� �� NearIndex03 face�� skinny triangle �̶��
						if (Fs[NearIndex03].isErrorFace == 1)
						{
							//NearIndex03 fIndexNear21-22�� ª�� �Ÿ�
							if (IsSamePoint(getCloseCorner(Vs[fIndexNear21].P, Vo, i), getCloseCorner(Vs[fIndexNear22].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex03 21-22�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear21].P = getCloseCorner(Vs[fIndexNear21].P, Vo, i);
								newVs[fIndexNear22].P = getCloseCorner(Vs[fIndexNear22].P, Vo, i);

								Fs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear22�� ���� fIndexNear21�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear22)
										newFs[j].v1 = fIndexNear21;
									if (newFs[j].v2 == fIndexNear22)
										newFs[j].v2 = fIndexNear21;
									if (newFs[j].v3 == fIndexNear22)
										newFs[j].v3 = fIndexNear21;
								}
							}
							//NearIndex03 fIndexNear21-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear21].P, Vo, i), getCloseCorner(Vs[fIndexNear23].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex03 11-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear21].P = getCloseCorner(Vs[fIndexNear21].P, Vo, i);
								newVs[fIndexNear23].P = getCloseCorner(Vs[fIndexNear23].P, Vo, i);

								Fs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear23�� ���� fIndexNear21�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear23)
										newFs[j].v1 = fIndexNear21;
									if (newFs[j].v2 == fIndexNear23)
										newFs[j].v2 = fIndexNear21;
									if (newFs[j].v3 == fIndexNear23)
										newFs[j].v3 = fIndexNear21;
								}
							}
							//NearIndex03 fIndexNear22-13�� ª�� �Ÿ�
							else if (IsSamePoint(getCloseCorner(Vs[fIndexNear22].P, Vo, i), getCloseCorner(Vs[fIndexNear23].P, Vo, i)))
							{
								//printf("Case11 ErrorFace, NearIndex03 12-13�� index edge�� ª��\n");

								//���� ª�� edge�� ����� ������ ���������� �̵��Ѵ�.
								newVs[fIndexNear22].P = getCloseCorner(Vs[fIndexNear22].P, Vo, i);
								newVs[fIndexNear23].P = getCloseCorner(Vs[fIndexNear23].P, Vo, i);

								Fs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFs[NearIndex03].use = 0;			//NearIndex03�� face list�� ���� �ʱ�
								newFaceNum--;
								if (nowErrorCaseTemp[6] == 0)
								{
									nowErrorCase[6]++;
									nowErrorCaseTemp[6] = 1;
								}

								for (int j = 0; j < faceNum; j++)	// face list���� fIndexNear23�� ���� fIndexNear22�� ������ �ٲٱ�
								{
									if (newFs[j].v1 == fIndexNear23)
										newFs[j].v1 = fIndexNear22;
									if (newFs[j].v2 == fIndexNear23)
										newFs[j].v2 = fIndexNear22;
									if (newFs[j].v3 == fIndexNear23)
										newFs[j].v3 = fIndexNear22;
								}
							}
							else
							{
								printf("!!Case 11 NearIndex03 face ���� ����\n");
							}
						}
					}
					else
					{
						printf("!!Case6 �� face ������ 4�� �϶� ���� + �⺻���¿� �ٸ�\n");
					}
				}
				else
				{
					printf("!!Case6 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
					//Case6�� ��� face ?���� ���� �� �ִ�.
				}
			}
			//Case6 ���� �Ϸ�


			//���� �ﰢ�� 4�� ���� Case8, 9 ,14

			//Case8

			if (Vo[i].voxelCase == 8)
			{
				Point nearPoint0;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[8] = 0;

				//�Ѱ��� ó�� 4��
				if (Vo[i].vErrorFaceNum > 0 && Vo[i].vFaceNum == 4)
				{
					//printf("%d index ���� Case8\n", Vo[i].voxelIndex);

					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�
					//printf("%lf %lf %lf\n", newVs[Fs[tempStart].v1].P.x, newVs[Fs[tempStart].v1].P.y, newVs[Fs[tempStart].v1].P.z);

					//1�� Face�� ����
					if (Fs[tempStart].isErrorFace == 1)
					{
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3

						//01-02 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex02].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 01-02�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex02)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex02)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex02)
									newFs[j].v3 = fIndex01;
							}
						}
						//01-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 01-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex01;
							}
						}
						//02-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex02].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 02-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex02].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex02].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex02;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex02;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex02;
							}
						}
						else
						{
							printf("!!Case8 ErrorFace 1�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//2�� Face�� ����
					if (Fs[tempStart + 1].isErrorFace == 1)
					{
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3

						//11-12 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex12].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 11-12�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex12)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex12)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex12)
									newFs[j].v3 = fIndex11;
							}
						}
						//11-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 11-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex11;
							}
						}
						//12-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex12].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 12-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex12].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex12].P, Vo, i);


							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex12;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex12;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex12;
							}
						}
						else
						{
							printf("!!Case8 ErrorFace 2�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//3�� Face�� ����
					if (Fs[tempStart + 2].isErrorFace == 1)
					{
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3

						//21-22 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex22].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex22)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex22)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex22)
									newFs[j].v3 = fIndex21;
							}
						}
						//21-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex21;
							}
						}
						//22-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex22].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex22].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex22].P, Vo, i);


							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex22;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex22;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex22;
							}
						}
						else
						{
							printf("!!Case8 ErrorFace 3�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//4�� Face�� ����
					if (Fs[tempStart + 3].isErrorFace == 1)
					{
						fIndex31 = Fs[tempStart + 3].v1;	//face index �� �ֱ� 1
						fIndex32 = Fs[tempStart + 3].v2;	//face index �� �ֱ� 2
						fIndex33 = Fs[tempStart + 3].v3;	//face index �� �ֱ� 3

						//31-32 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex32].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex32)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex32)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex32)
									newFs[j].v3 = fIndex31;
							}
						}
						//31-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex31;
							}
						}
						//32-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex32].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case8 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex32].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex32].P, Vo, i);


							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[8] == 0)
							{
								nowErrorCase[8]++;
								nowErrorCaseTemp[8] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex32;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex32;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex32;
							}
						}
						else
						{
							printf("!!Case8 ErrorFace 4�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
				}
				else
				{
					printf("!!Case8 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case8 ���� �Ϸ�

			//Case9

			if (Vo[i].voxelCase == 9)
			{
				Point nearPoint0;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[9] = 0;

				//�Ѱ��� ó�� 4��
				if (Vo[i].vErrorFaceNum > 0 && Vo[i].vFaceNum == 4)
				{
					//printf("%d index ���� Case9\n", Vo[i].voxelIndex);

					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�
					//printf("%lf %lf %lf\n", newVs[Fs[tempStart].v1].P.x, newVs[Fs[tempStart].v1].P.y, newVs[Fs[tempStart].v1].P.z);

					//1�� Face�� ����
					if (Fs[tempStart].isErrorFace == 1)
					{
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3

						//01-02 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex02].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 01-02�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex02)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex02)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex02)
									newFs[j].v3 = fIndex01;
							}
						}
						//01-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 01-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex01;
							}
						}
						//02-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex02].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 02-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex02].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex02].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex02;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex02;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex02;
							}
						}
						else
						{
							printf("!!Case9 ErrorFace 1�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//2�� Face�� ����
					if (Fs[tempStart + 1].isErrorFace == 1)
					{
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3

						//11-12 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex12].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 11-12�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex12)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex12)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex12)
									newFs[j].v3 = fIndex11;
							}
						}
						//11-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 11-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex11;
							}
						}
						//12-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex12].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 12-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex12].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex12].P, Vo, i);


							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex12;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex12;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex12;
							}
						}
						else
						{
							printf("!!Case9 ErrorFace 2�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//3�� Face�� ����
					if (Fs[tempStart + 2].isErrorFace == 1)
					{
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3

						//21-22 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex22].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex22)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex22)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex22)
									newFs[j].v3 = fIndex21;
							}
						}
						//21-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex21;
							}
						}
						//22-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex22].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex22].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex22].P, Vo, i);


							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex22;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex22;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex22;
							}
						}
						else
						{
							printf("!!Case9 ErrorFace 3�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//4�� Face�� ����
					if (Fs[tempStart + 3].isErrorFace == 1)
					{
						fIndex31 = Fs[tempStart + 3].v1;	//face index �� �ֱ� 1
						fIndex32 = Fs[tempStart + 3].v2;	//face index �� �ֱ� 2
						fIndex33 = Fs[tempStart + 3].v3;	//face index �� �ֱ� 3

						//31-32 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex32].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex32)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex32)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex32)
									newFs[j].v3 = fIndex31;
							}
						}
						//31-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex31;
							}
						}
						//32-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex32].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case9 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex32].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex32].P, Vo, i);


							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[9] == 0)
							{
								nowErrorCase[9]++;
								nowErrorCaseTemp[9] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex32;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex32;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex32;
							}
						}
						else
						{
							printf("!!Case9 ErrorFace 4�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
				}
				else
				{
					printf("!!Case9 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case9 ���� �Ϸ�

			//Case14

			if (Vo[i].voxelCase == 14)
			{
				Point nearPoint0;
				int fIndex01 = 0, fIndex02 = 0, fIndex03 = 0;
				int fIndex11 = 0, fIndex12 = 0, fIndex13 = 0;
				int fIndex21 = 0, fIndex22 = 0, fIndex23 = 0;
				int fIndex31 = 0, fIndex32 = 0, fIndex33 = 0;
				nowErrorCaseTemp[14] = 0;

				//�Ѱ��� ó�� 4��
				if (Vo[i].vErrorFaceNum > 0 && Vo[i].vFaceNum == 4)
				{
					//printf("%d index ���� Case14\n", Vo[i].voxelIndex);

					tempStart = Vo[i].startIndex;	//������ face ���� ���� index �� �ֱ�
					//printf("%lf %lf %lf\n", newVs[Fs[tempStart].v1].P.x, newVs[Fs[tempStart].v1].P.y, newVs[Fs[tempStart].v1].P.z);

					//1�� Face�� ����
					if (Fs[tempStart].isErrorFace == 1)
					{
						fIndex01 = Fs[tempStart].v1;	//face index �� �ֱ� 1
						fIndex02 = Fs[tempStart].v2;	//face index �� �ֱ� 2
						fIndex03 = Fs[tempStart].v3;	//face index �� �ֱ� 3

						//01-02 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex02].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 01-02�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex02)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex02)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex02)
									newFs[j].v3 = fIndex01;
							}
						}
						//01-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex01].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 01-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex01].P = getCloseCorner(Vs[fIndex01].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex01].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex01;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex01;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex01;
							}
						}
						//02-03 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex02].P, Vo, i), getCloseCorner(Vs[fIndex03].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 02-03�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex02].P = getCloseCorner(Vs[fIndex02].P, Vo, i);
							newVs[fIndex03].P = getCloseCorner(Vs[fIndex02].P, Vo, i);

							Fs[tempStart].use = 0;					// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex03)
									newFs[j].v1 = fIndex02;
								if (newFs[j].v2 == fIndex03)
									newFs[j].v2 = fIndex02;
								if (newFs[j].v3 == fIndex03)
									newFs[j].v3 = fIndex02;
							}
						}
						else
						{
							printf("!!Case14 ErrorFace 1�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//2�� Face�� ����
					if (Fs[tempStart + 1].isErrorFace == 1)
					{
						fIndex11 = Fs[tempStart + 1].v1;	//face index �� �ֱ� 1
						fIndex12 = Fs[tempStart + 1].v2;	//face index �� �ֱ� 2
						fIndex13 = Fs[tempStart + 1].v3;	//face index �� �ֱ� 3

						//11-12 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex12].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 11-12�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex12)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex12)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex12)
									newFs[j].v3 = fIndex11;
							}
						}
						//11-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex11].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 11-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex11].P = getCloseCorner(Vs[fIndex11].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex11].P, Vo, i);

							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex11;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex11;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex11;
							}
						}
						//12-13 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex12].P, Vo, i), getCloseCorner(Vs[fIndex13].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 12-13�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex12].P = getCloseCorner(Vs[fIndex12].P, Vo, i);
							newVs[fIndex13].P = getCloseCorner(Vs[fIndex12].P, Vo, i);


							Fs[tempStart + 1].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 1].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex13)
									newFs[j].v1 = fIndex12;
								if (newFs[j].v2 == fIndex13)
									newFs[j].v2 = fIndex12;
								if (newFs[j].v3 == fIndex13)
									newFs[j].v3 = fIndex12;
							}
						}
						else
						{
							printf("!!Case14 ErrorFace 2�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//3�� Face�� ����
					if (Fs[tempStart + 2].isErrorFace == 1)
					{
						fIndex21 = Fs[tempStart + 2].v1;	//face index �� �ֱ� 1
						fIndex22 = Fs[tempStart + 2].v2;	//face index �� �ֱ� 2
						fIndex23 = Fs[tempStart + 2].v3;	//face index �� �ֱ� 3

						//21-22 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex22].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex22)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex22)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex22)
									newFs[j].v3 = fIndex21;
							}
						}
						//21-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex21].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex21].P = getCloseCorner(Vs[fIndex21].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex21].P, Vo, i);

							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex21;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex21;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex21;
							}
						}
						//22-23 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex22].P, Vo, i), getCloseCorner(Vs[fIndex23].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex22].P = getCloseCorner(Vs[fIndex22].P, Vo, i);
							newVs[fIndex23].P = getCloseCorner(Vs[fIndex22].P, Vo, i);


							Fs[tempStart + 2].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 2].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex23)
									newFs[j].v1 = fIndex22;
								if (newFs[j].v2 == fIndex23)
									newFs[j].v2 = fIndex22;
								if (newFs[j].v3 == fIndex23)
									newFs[j].v3 = fIndex22;
							}
						}
						else
						{
							printf("!!Case14 ErrorFace 3�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
					//4�� Face�� ����
					if (Fs[tempStart + 3].isErrorFace == 1)
					{
						fIndex31 = Fs[tempStart + 3].v1;	//face index �� �ֱ� 1
						fIndex32 = Fs[tempStart + 3].v2;	//face index �� �ֱ� 2
						fIndex33 = Fs[tempStart + 3].v3;	//face index �� �ֱ� 3

						//31-32 ª�� �Ÿ�
						if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex32].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 21-22�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 2�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex32)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex32)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex32)
									newFs[j].v3 = fIndex31;
							}
						}
						//31-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex31].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 21-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex31].P = getCloseCorner(Vs[fIndex31].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex31].P, Vo, i);

							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 1�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex31;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex31;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex31;
							}
						}
						//32-33 ª�� �Ÿ�
						else if (IsSamePoint(getCloseCorner(Vs[fIndex32].P, Vo, i), getCloseCorner(Vs[fIndex33].P, Vo, i)))
						{
							//printf("Case14 ErrorFace 22-23�� index edge�� ª��\n");

							//���� ª�� edge ���� ��ġ�� ���� ����� ���������� �̵��Ѵ�.
							newVs[fIndex32].P = getCloseCorner(Vs[fIndex32].P, Vo, i);
							newVs[fIndex33].P = getCloseCorner(Vs[fIndex32].P, Vo, i);


							Fs[tempStart + 3].use = 0;				// �� face list�� ���� �ʰ� ǥ��
							newFs[tempStart + 3].use = 0;			// �� face list�� ���� �ʰ� ǥ��
							newFaceNum--;							// face ���� �Ѱ� ����
							if (nowErrorCaseTemp[14] == 0)
							{
								nowErrorCase[14]++;
								nowErrorCaseTemp[14] = 1;
							}

							for (int j = 0; j < faceNum; j++)		// face list���� 3�� ���� 2�� ������ �ٲٱ�
							{
								if (newFs[j].v1 == fIndex33)
									newFs[j].v1 = fIndex32;
								if (newFs[j].v2 == fIndex33)
									newFs[j].v2 = fIndex32;
								if (newFs[j].v3 == fIndex33)
									newFs[j].v3 = fIndex32;
							}
						}
						else
						{
							printf("!!Case14 ErrorFace 4�� face ���� ���� ���� �� ���� �߻�\n");
						}
					}
				}
				else
				{
					printf("!!Case14 face %d�� �����ʿ�\n", Vo[i].vFaceNum);
				}
			}
			//Case14 ���� �Ϸ�

			//������ Case 5������ �⺻ Case 5�� Error Face�� ���� �� ���� ����
		}
	}


	//p1.x =  Vs

	printf("������2\n");

	int plusErrorFace = 0;			//���� �� ���� ���� �־ �߰��� ���ŵǴ� Face
	int AlgoErrorFace = 0;			//�˰��� �ڵ�� ���� ���ŵǴ� Face

	for (i = 0; i < faceNum; i++)
	{
		if (newFs[i].use != 0)
		{
			if (newFs[i].v1 == newFs[i].v2 || newFs[i].v1 == newFs[i].v3 || newFs[i].v2 == newFs[i].v3) // face�� �����ʴ� ���
			{
				plusErrorFace++;
			}
		}
	}
	printf("newFaceNum : %d\nplusErrorFace : %d\n", newFaceNum, plusErrorFace);
	AlgoErrorFace = faceNum - newFaceNum;
	newFaceNum -= plusErrorFace;		//�˰������� ���� ���ŵ� Face �������� ���� ������ ���� ���ŵ� Face�� ����



	fprintf(fpNewVOFF, "OFF\n\n");
	fprintf(fpNewVOFF, "%d %d 0\n", vertexNum, newFaceNum);
	for (i = 0; i < vertexNum; i++)
	{
		fprintf(fpNewVOFF, "%lf %lf %lf\n", newVs[i].P.x, newVs[i].P.y, newVs[i].P.z);
	}
	for (i = 0; i < faceNum; i++)
	{
		if (newFs[i].use != 0)
		{
			if (newFs[i].v1 != newFs[i].v2 && newFs[i].v1 != newFs[i].v3 && newFs[i].v2 != newFs[i].v3) // face�� �����ʴ� ��� ����(�� �Ǵ� ��)
			{
				fprintf(fpNewVOFF, "3 %d %d %d\n", newFs[i].v1, newFs[i].v2, newFs[i].v3);
			}
		}
	}

	delete[] Vo;
	delete[] Vs;
	delete[] Fs;

	fprintf(fpErrorCase, "�� Case �� Skinny triangle�� �����ϴ� Voxel�� ��\n");
	for (i = 1; i <= 14; i++)
	{
		printf("%6d", i);
		fprintf(fpErrorCase, "%8d", i);
	}
	printf("\n");
	fprintf(fpErrorCase, "\n");
	for (i = 1; i <= 14; i++)
	{
		printf("%6d", errorCase[i]);
		fprintf(fpErrorCase, "%8d", errorCase[i]);
		AllErrorCase += errorCase[i];
	}
	printf("\n");
	fprintf(fpErrorCase, "\n");
	for (i = 1; i <= 14; i++)
	{
		printf("%6d", nowErrorCase[i]);
		fprintf(fpErrorCase, "%8d", nowErrorCase[i]);
		AllnowErrorCase += nowErrorCase[i];
	}

	fprintf(fpErrorCase, "\n���� ���� Case (1, 3, 7, 10, 12), (2, 11, 13), (4 ,6), (8,9,14)\n");
	fprintf(fpErrorCase, "(����)��ü Error Case : %d��\n", AllErrorCase);
	fprintf(fpErrorCase, "(����)���� ���� Error Case : %d��\n", AllnowErrorCase);
	fprintf(fpErrorCase, "(����)���� ���� ���� %lf\n", (double)AllnowErrorCase / valVoxelNum);
	fprintf(fpErrorCase, "(�ﰢ��)�˰��� ���� ���ŵ� Face : %d��\n", AlgoErrorFace);
	fprintf(fpErrorCase, "(�ﰢ��)�߰��� ���ŵ� Face : %d��\n", plusErrorFace);
	fprintf(fpErrorCase, "(�ﰢ��)��ü Face %d�� ���� �� %d�� ���ŵǼ� face�� �� %d�� �̴�.\n", faceNum, AlgoErrorFace + plusErrorFace, newFaceNum);



	fclose(fpErrorCase);
	fclose(fpVOFF);
	fclose(fpEdgePassCheckFile1);
	//fclose(fpErrorFace);
	fclose(fpVoxelCase);
	fclose(fpNewVOFF);

	return 0;
}

// Face�� ����� ������ ��ġ ���ϱ� (Case 1��)
Point findNearIndex(faceSet Fs[], vertexSet Vs[], int n)
{
	Point nearPoint, p1, p2, p3;
	nearPoint.x = 0;
	nearPoint.y = 0;
	nearPoint.z = 0;

	p1 = Vs[Fs[n].v1].P;
	p2 = Vs[Fs[n].v2].P;
	p3 = Vs[Fs[n].v3].P;

	if (p1.x == p2.x || p1.x == p3.x)
	{
		nearPoint.x = p1.x;
	}
	else if (p2.x == p3.x)
	{
		nearPoint.x = p2.x;
	}
	else
	{
		printf("findNearIndex X ����\n");
	}

	if (p1.y == p2.y || p1.y == p3.y)
	{
		nearPoint.y = p1.y;
	}
	else if (p2.y == p3.y)
	{
		nearPoint.y = p2.y;
	}
	else
	{
		printf("findNearIndex Y ����\n");
	}

	if (p1.z == p2.z || p1.z == p3.z)
	{
		nearPoint.z = p1.z;
	}
	else if (p2.z == p3.z)
	{
		nearPoint.z = p2.z;
	}
	else
	{
		printf("findNearIndex Z ����\n");
	}

	return nearPoint;
}

/*
//���� ������ �糡 �������� �ƴϸ� edge�� �߰� ���� �ֱ� (�̻��)
void setCenterNotCorner(Point p1, Point* newp1, Voxel* Vo, int i)
{
	if (p1.x != (Vo[i].b * stepSize) && p1.x != ((Vo[i].b * stepSize) + stepSize))	//x���� ������ �糡 �������� �ƴϸ� �߰� �� �ֱ�
	{
		(*newp1).x = (Vo[i].b * stepSize) + half_stepSize;
		//printf("ü����x ");
	}
	if (p1.y != (Vo[i].c * stepSize) && p1.y != ((Vo[i].c * stepSize) + stepSize))	//y���� ������ �糡 �������� �ƴϸ� �߰� �� �ֱ�
	{
		(*newp1).y = (Vo[i].c * stepSize) + half_stepSize;
		//printf("ü����y ");
	}
	if (p1.z != (Vo[i].h * stepSize) && p1.z != ((Vo[i].h * stepSize) + stepSize))	//z���� ������ �糡 �������� �ƴϸ� �߰� �� �ֱ�
	{
		(*newp1).z = (Vo[i].h * stepSize) + half_stepSize;
		//printf("ü����z ");
	}
	//printf("\n");
}
*/
//������ ���� ����� �ڳʸ� ã���� �ش� ������ �ٲٱ�
void setCloseCorner(Point p1, Point* newp1, Voxel Vo[], int i)
{
	double xBound = 0, yBound = 0, zBound = 0;
	xBound = (Vo[i].b * stepSize) + half_stepSize;
	yBound = (Vo[i].c * stepSize) + half_stepSize;
	zBound = (Vo[i].h * stepSize) + half_stepSize;

	//x�� ����� �� ã��
	if (p1.x >= xBound)
		(*newp1).x = (Vo[i].b * stepSize) + stepSize;
	else
		(*newp1).x = (Vo[i].b * stepSize);
	//y�� ����� �� ã��
	if (p1.y >= yBound)
		(*newp1).y = (Vo[i].c * stepSize) + stepSize;
	else
		(*newp1).y = (Vo[i].c * stepSize);
	//z�� ����� �� ã��
	if (p1.z >= zBound)
		(*newp1).z = (Vo[i].h * stepSize) + stepSize;
	else
		(*newp1).z = (Vo[i].h * stepSize);
}

//������ ���� ����� �ڳ� ã��
Point getCloseCorner(Point p1, Voxel Vo[], int i)
{
	Point result;
	double xBound = 0, yBound = 0, zBound = 0;
	xBound = (Vo[i].b * stepSize) + half_stepSize;
	yBound = (Vo[i].c * stepSize) + half_stepSize;
	zBound = (Vo[i].h * stepSize) + half_stepSize;

	//x�� ����� �� ã��
	if (p1.x >= xBound)
		result.x = (Vo[i].b * stepSize) + stepSize;
	else
		result.x = (Vo[i].b * stepSize);
	//y�� ����� �� ã��
	if (p1.y >= yBound)
		result.y = (Vo[i].c * stepSize) + stepSize;
	else
		result.y = (Vo[i].c * stepSize);
	//z�� ����� �� ã��
	if (p1.z >= zBound)
		result.z = (Vo[i].h * stepSize) + stepSize;
	else
		result.z = (Vo[i].h * stepSize);

	return result;
}

//�� ���� ���� ��ġ �ΰ�?
int IsSamePoint(Point p1, Point p2)
{
	if (p1.x == p2.x)
		if (p1.y == p2.y)
			if (p1.z == p2.z)
				return 1;

	return 0;
}

//�� Face�� ������ Face�ΰ�? (�� edge�� ����)
int IsNearFace(int A, int B, faceSet Fs[])
{
	int inA1, inA2, inA3, inB1, inB2, inB3;

	inA1 = Fs[A].v1;
	inA2 = Fs[A].v2;
	inA3 = Fs[A].v3;

	inB1 = Fs[B].v1;
	inB2 = Fs[B].v2;
	inB3 = Fs[B].v3;

	if (inA1 == inB1 || inA1 == inB2 || inA1 == inB3) // A1�� ���� �� ����
	{
		if (inA2 == inB1 || inA2 == inB2 || inA2 == inB3) // A1, A2�� ���� �� ����
		{
			if (inA3 == inB1 || inA3 == inB2 || inA3 == inB3) // A1, A2, A3 ���� �� ����
			{
				printf("!!�� face ���� �񱳽� ���� Face�� ������ ���� �߻�\n");
				return 3;
			}
			else // A1, A2�� ���� �� ����
			{
				return 1;
			}
		}
		else if (inA3 == inB1 || inA3 == inB2 || inA3 == inB3) //A1, A3 ���� �� ����
		{
			return 1;
		}
		else //A1�� ���� �� ���� = �� ���� ����
		{
			printf("!!�� face ���� �񱳽� ������ ���� ��ġ �߻�\n");
			return 2;
		}
	}
	else // A1�� ���� ���� ����
	{
		if (inA2 == inB1 || inA2 == inB2 || inA2 == inB3) // A2�� ���� �� ����
		{
			if (inA3 == inB1 || inA3 == inB2 || inA3 == inB3) // A2, A3 ���� �� ����
			{
				return 1;
			}
			else // A2�� ���� �� ����
			{
				printf("!!�� face ���� �񱳽� ������ ���� ��ġ �߻�\n");
				return 2;
			}
		}
		else if (inA3 == inB1 || inA3 == inB2 || inA3 == inB3) //A3 �� ���� �� ����
		{
			printf("!!�� face ���� �񱳽� ������ ���� ��ġ �߻�\n");
			return 2;
		}
		else //���� ���� ���� �������� ����
		{
			return 0;
		}
	}
}


//���� ���� ����(Ȯ��)
//1. �⺻ Case�� �ٸ� face ������ ����
//Case 10 face 4�� ����(�⺻�� 2��)
//Case 12 face 5�� ����(�⺻�� 3��)
//Case 11 face 5�� ����(�⺻�� 3��)
//���� if ������ ������� ���ѳ���