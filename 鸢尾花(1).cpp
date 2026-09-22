#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define bool int
#define false 0
#define true 1
#define K 3               // 簇的数目
#define dimNum 4          // 维数
#define MAX_ROUNDS 100    // 最大允许的迭代次数

// 存储鸢尾花数据的结构
struct Iris {
    double sepalLength;   // 花萼长度
    double sepalWidth;    // 花尊宽度
    double petalLength;   // 花瓣长度
    double petalWidth;    // 花瓣宽度
    int clusterID;        // 用于存放该点所属的簇群编号
};

int isContinue;
struct Iris iris[150]; // 记录鸢尾花数据集
struct Iris clusterCenter[K]; // 存储质心
struct Iris centerCalc[K]; // 存储计算的新质心
int dataNum; // 存储数据集中的数据记录总数
int clusterCenterInitIndex[K]; // 记录每个质心最初使用的数据点的编号
double distanceFromCenter[K]; // 记录一个点到所有质心的距离
int dataSizePerCluster[K]; // 记录每个簇群的样本总数

bool Inputs();
void InitialCluster();
void KMeans();
void CalDistance2OneCenters(int pointID, int centerID);
void CalDistance2AllCenters(int pointID);
void Partition4OnePoint(int pointID);
void Partition4AllPointOneCluster();
void CalClusterCenter();
void CompareNewOldClusterCenter();

int main() {
    system("chcp 65001");
    if (Inputs() == false) {
        return 0;
    }
    
    InitialCluster();
    KMeans();
    
    system("pause");
    return 0;
}

bool Inputs() {
    char fname[256]; // 文件名
    char name[20];
    FILE *fp;
    
    printf("\n 样本数目:\n");
    scanf("%d", &dataNum);
    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        printf("不能打开输入的文件\n");
        return false;
    }
    
    for (int i = 0; i < dataNum; i++) {
        fscanf(fp, "%lf,%lf,%lf,%lf,%s", &iris[i].sepalLength, &iris[i].sepalWidth, &iris[i].petalLength, &iris[i].petalWidth,name);
        iris[i].clusterID = -1; // cluster id初值为-1
    }
    
    fclose(fp);
    return true;
}

void InitialCluster() {
    int random; // 存放随机数的变量
    for (int i=0;i<K;i++){
    	clusterCenterInitIndex[i]=-1;
	}
    // 随机产生 K 个质心编号(不能重复)
    for (int i = 0; i < K; i++) {
        random = rand() % (dataNum-1); // 随机产生0到dataNum-1之间的随机整数
        int j = 0;
        
        // 与前i个质心编号进行比对，判断是否有重复
        while (j < i) {
            if (random == clusterCenterInitIndex[j]) {
                random = rand() % (dataNum-1);
                j = 0;
            } else {
                j++;
            }
        }
        
        clusterCenterInitIndex[i] = random; // 设置第i个质心编号
    }
    
    // 确定K个质心编号后，将样本数组iris中对应编号的数据赋值给质心结构体数组clusterCenter
    for (int i = 0; i < K; i++) {
        clusterCenter[i].sepalLength = iris[clusterCenterInitIndex[i]].sepalLength;
        clusterCenter[i].sepalWidth = iris[clusterCenterInitIndex[i]].sepalWidth;
        clusterCenter[i].petalLength = iris[clusterCenterInitIndex[i]].petalLength;
        clusterCenter[i].petalWidth = iris[clusterCenterInitIndex[i]].petalWidth;
        clusterCenter[i].clusterID = i; // 将该类的编号设置为i
        // 将 iris 中该质心的类编号设置为i
        iris[clusterCenterInitIndex[i]].clusterID = i;
    }
}

void KMeans() {
    int rounds;
    for (rounds = 0; rounds < MAX_ROUNDS; rounds++) {
        printf("\nRounds: %d\n", rounds + 1); // 显示聚类次数
        Partition4AllPointOneCluster(); // 将每个点划分到距离最近的簇群
        CalClusterCenter(); // 重新计算新质心
        if (isContinue == 0) { // 判断是否继续聚类
            printf("\n 经过 %d 次聚类，聚类完成!\n", rounds + 1);
            break; // 结束聚类
        }
    }
}

void CalDistance2OneCenters(int pointID, int centerID) {
    double x1 = pow((iris[pointID].sepalLength - clusterCenter[centerID].sepalLength), 2.0);
    double x2 = pow((iris[pointID].sepalWidth - clusterCenter[centerID].sepalWidth), 2.0);
    double x3 = pow((iris[pointID].petalLength - clusterCenter[centerID].petalLength), 2.0);
    double x4 = pow((iris[pointID].petalWidth - clusterCenter[centerID].petalWidth), 2.0);
    distanceFromCenter[centerID] = sqrt(x1 + x2 + x3 + x4);
}

void CalDistance2AllCenters(int pointID) {
    for (int i = 0; i < K; i++) {
        CalDistance2OneCenters(pointID, i); // 计算一个点到一个质心的距离
    }
}

void Partition4OnePoint(int pointID) {
    int minIndex = 0; // 记录距离最近的质心编号
    double minValue = distanceFromCenter[0];
    
    // 求解最短距离，并更新距离最近的质心编号
    for (int i = 0; i < K; i++) {
        if (distanceFromCenter[i] < minValue) {
            minValue = distanceFromCenter[i];
            minIndex = i;
        }
    }
    
    // 将点的类标号设置为最近质心所在的类标号
    iris[pointID].clusterID = clusterCenter[minIndex].clusterID;
}

void Partition4AllPointOneCluster() {
    for (int i = 0; i < dataNum; i++) {
        if (iris[i].clusterID != -1) {
            continue; // 这个点就是质心，不需要划分簇群
        }
        else{
        	CalDistance2AllCenters(i); // 计算第i个点到所有质心的距离
        	Partition4OnePoint(i); // 将第i个点划分到距离最近的簇群
		}
    }
}

void CalClusterCenter() {
    // 初始化所需的数组，使数组中的各元素值为0
    memset(centerCalc, 0, sizeof(centerCalc));
    memset(dataSizePerCluster, 0, sizeof(dataSizePerCluster));
    
    // 分别对每个簇群内的每个点的4个特征值求和，并计算每个簇群内点的个数
    for (int i = 0; i < dataNum; i++) {
        centerCalc[iris[i].clusterID].sepalLength += iris[i].sepalLength;
        centerCalc[iris[i].clusterID].sepalWidth += iris[i].sepalWidth;
        centerCalc[iris[i].clusterID].petalLength += iris[i].petalLength;
        centerCalc[iris[i].clusterID].petalWidth += iris[i].petalWidth;
        dataSizePerCluster[iris[i].clusterID]++;
    }
    
    // 计算每个簇群内点的4个特征值的均值，作为新质心
    for (int i = 0; i < K; i++) {
        if (dataSizePerCluster[i] != 0) {
            centerCalc[i].sepalLength /= dataSizePerCluster[i];
            centerCalc[i].sepalWidth /= dataSizePerCluster[i];
            centerCalc[i].petalLength /= dataSizePerCluster[i];
            centerCalc[i].petalWidth /= dataSizePerCluster[i];
            printf("cluster%d point cnt：%d\n",i,dataSizePerCluster[i]);// 输出每个簇群的总数
			printf("cluster%d center:sepalLength:%.2lf, sepalWidth:%.2lf, petalLength:%.2lf, petalWidth:%.2lf\n", i,centerCalc[i].sepalLength, centerCalc[i].sepalWidth, centerCalc[i].petalLength,centerCalc[i].petalWidth);
		}
		else{
			printf("cluster %d count is zero\n", i);
		}
    }
    
    CompareNewOldClusterCenter();
    
    // 将新质心的值放入质心结构体中
    for (int i = 0; i < K; i++) {
        clusterCenter[i].sepalLength = centerCalc[i].sepalLength;
        clusterCenter[i].sepalWidth = centerCalc[i].sepalWidth;
        clusterCenter[i].petalLength = centerCalc[i].petalLength;
        clusterCenter[i].petalWidth = centerCalc[i].petalWidth;
        clusterCenter[i].clusterID=i;
    }
    
    // 重新计算新质心后，要重新为每一个点进行聚类，所以数据集中所有点的clusterID都要重置为-1
    for (int i = 0; i < dataNum; i++) {
        iris[i].clusterID = -1;
    }
}

void CompareNewOldClusterCenter() {
    isContinue = 0; // 初始化标记变量isContinue的值为0，其中0表示停止聚类。1表示继续聚类
    
    for (int i = 0; i < K; i++) {
        if (centerCalc[i].sepalLength != clusterCenter[i].sepalLength ||
            centerCalc[i].sepalWidth != clusterCenter[i].sepalWidth ||
            centerCalc[i].petalLength != clusterCenter[i].petalLength ||
            centerCalc[i].petalWidth != clusterCenter[i].petalWidth) {
            isContinue = 1; // 如果有一个质心的值不同，就将isContinue的值设置为1，表示需要继续聚类
            break; // 跳出循环
        }
    }
}


