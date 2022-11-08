

//此文件用于处理摄像头数据
//文档的camtest 应当在系统计时器中调用
//去掉dma里的callback函数

#include "camprocess.h"
void CenterLine_show(CamStruct *ov5640);

/*-------------Canny算法检测边缘---------------LQ--
==============================================LQ=
 * Gx:
 *     1  -1
 *     1  -1
 *---------------
 * Gy:
 *    -1  -1
 *     1   1
*----------------------------------------
% 输入：
% org：图像，如果不是灰度图转成灰度图
% lowThr：低阈值
% 输出：
% ImgEdge： 两个偏导的平方差，反映了边缘的强度
% Sector：将方向分为3个区域，具体如下
% ------->x
% |3 2 1
% |0 X 0
% |1 2 3
% V
% y
*---------------------------------------*/

short CannyEdgeTest(CamStruct *ov5640, uint8_t lowThr)
{
    short wideth = ov5640->CamWide;
    short height = ov5640->CanHigh;
    uint8_t(*org)[wideth];

    org = (uint8_t(*)[wideth])ov5640->Cam_Result;
    float(*A)[wideth];
    A = (float(*)[wideth])(ov5640->BufferAddress + wideth * height * 2);

    memcpy((uint8_t *)org, (uint8_t *)ov5640->BufferAddress, wideth * height);

    uint8_t(*Sector)[wideth]; //像素梯度所在扇区
    Sector = (uint8_t(*)[wideth])(ov5640->BufferAddress + wideth * height * 6);

    float Temp[9]; //双阈值检测临时数组
    // float TemMax = 0; //临时最大值
    // float theta = 0; //梯度方向
    int Xg = 0; // X方向梯度
    int Yg = 0; // Y方向梯度

    short x, y;
    uint8_t heighthr = 40; // ratio*lowThr; //高阈值
    uint8_t loss = 5;     //非极大值抑制补偿,增加边缘

    //==========求解梯度，幅度和方向===========
    for (y = 0; y < height - 1; y++) //最下面一行不处理
    {
        for (x = 0; x < wideth - 1; x++) //最上一列不处理
        {
            Xg = org[y][x] + org[y + 1][x] - org[y][x + 1] - org[y + 1][x + 1];  //计算X方向梯度
            Yg = -org[y][x] + org[y + 1][x] - org[y][x + 1] + org[y + 1][x + 1]; //计算Y方向梯度
            A[y][x] = Fsqrt(Xg * Xg + Yg * Yg);                                  //求幅值，快速开平方算法
            Sector[y][x] = Atan2(Yg, Xg);                                        //求梯度方向分区
        }
    }

    //===============非极大值抑制================
    for (y = 1; y < height - 1; y++)
    {
        for (x = 0; x < wideth - 1; x++)
        {
            if (0 == Sector[y][x]) //水平方向
            {
                if ((A[y][x] > A[y][x + 1] - loss) && A[y][x] > (A[y][x - 1] - loss))
                    A[y][x] = A[y][x];
                else
                    A[y][x] = 0;
            }
            if (1 == Sector[y][x]) //右上、左下
            {
                if (A[y][x] > (A[y - 1][x + 1] - loss) && A[y][x] > (A[y + 1][x - 1] - loss))
                    A[y][x] = A[y][x];
                else
                    A[y][x] = 0;
            }
            if (2 == Sector[y][x]) //竖直方向
            {
                if (A[y][x] > (A[y - 1][x] - loss) && A[y][x] > (A[y + 1][x] - loss))
                    A[y][x] = A[y][x];
                else
                    A[y][x] = 0;
            }
            if (3 == Sector[y][x]) //左上、右下
            {
                if (A[y][x] > (A[y - 1][x - 1] - loss) && A[y][x] > (A[y + 1][x + 1] - loss))
                    A[y][x] = A[y][x];
                else
                    A[y][x] = 0;
            }
        }
    }

    //=============双阈值检测=================
    for (y = 1; y < height - 2; y++)
    {
        for (x = 1; x < wideth - 2; x++)
        {
            if (A[y][x] < lowThr) //低于低阈值
                org[y][x] = 0;
            else if (A[y][x] > heighthr) //高于高阈值
                org[y][x] = 0xf0;
            else
            {
                Temp[0] = A[y + 1][x - 1];
                Temp[1] = A[y + 1][x];
                Temp[2] = A[y + 1][x + 1];
                Temp[3] = A[y][x - 1];
                Temp[4] = A[y][x];
                Temp[5] = A[y][x + 1];
                Temp[6] = A[y - 1][x - 1];
                Temp[7] = A[y - 1][x];
                Temp[8] = A[y - 1][x + 1]; // 3x3区域内点
                Quicksort(Temp, 9, 0, 8);  //快速排序,低到高

                if (Temp[8] > heighthr)
                {
                    org[y][x] = 0xf0;
                    A[y][x] = 1;
                }
                else
                {
                    org[y][x] = 0;
                    A[y][x] = 0;
                }
            }
        }
    }

    //==========中心线计算============
    uint8_t *center;
    center = (ov5640->BufferAddress + wideth * height * 2);
    center[0] = wideth/2;

    double k, b;
    int sum1 = 0, sum2 = 0, sum3 = 0, sum4=0;

    for (y = 1; y < height; y++)
    {
        for (x = center[y-1]; x < wideth - 20; x++)
        {
            if (org[y][x] > 0xe0)
            {
                Xg = x;
                break;
            }
        }
        for (x = center[y-1]; x > 20; x--)
        {
            if (org[y][x] > 0xe0)
            {
                Yg = x;
                break;
            }
        }
        center[y] = (Xg + Yg) / 2;

        sum1 += y*center[y];
        sum2 += y*y;
        sum3 += center[y];
        sum4 += y;
    }

    //========中心散点显示========（可去除）
    for (y = 1; y < height; y++)
    {
        org[y][center[y]] = 0xf0;
    }

    //========最小二乘法计算========

    k = (sum1*1.0-(sum3*sum4*1.0)/(height-1))/(sum2*1.0-(sum4*sum4*1.0)/(height-1));
    b = sum3*1.0/(height-1) - k*(sum4*1.0/(height-1));

    //=======展示拟合之后的线==========

    for (y = 1; y < height; y++)
    {
        x = (short)(k*y+b);
        org[y][x] = 0xf0;
    }

    //==============目标线==============
    for (x=0; x<wideth; x++)
    {
        org[height*targetlinePer/100][x] = 0xf0;
    }


    //==============这里返回线与目标线的插值============
    return 0;
}

//    uint8_t* center;
//    center = ov5640->BufferAddress+wideth*height*6;
//
//    memcpy ((uint8_t*)ov5640->Cam_Result, (uint8_t*)org, wideth*height);
//
//
//    CenterLineFind((uint8_t*)ov5640->Cam_Result,  wideth, height, center);
//
//    for (x=0; x<height; x++)
//    {
//        org[x][center[x]]=0xff;
//    }

//    //没用的东西 为了效率请注释
//    Linestruct b;
//    x = CenterLineFind ((uint8_t*)org,  wideth, height, &b);
//    //printf("%d\n",x);
//    for (y=0; y<height; y++)
//    {
//        x = (float)(y - b.b)/b.k;
//        org[y][x] = 0xf0;
//    }
//    for (x=0; x<wideth; x++)
//    {
//        org[height*targetlinePer/100][x] = 0xf0;
//    }
//这里要处理
// memcpy ((uint8_t*)ov5640->Cam_Result, (uint8_t*)org, wideth*height);

//======== 快速开方 原理不懂，快！好用！=======LQ==
float Fsqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x; // evil floating point bit level hacking
    // i = 0x5f3759df - (i >> 1);  // what the fuck?
    i = 0X5F3504F3 - (i >> 1); //精度更高
    x = *(float *)&i;
    x = x * (1.5f - (xhalf * x * x));
    return 1 / x;
}

//=============反正切函数============LQ==
/********************************************
 *  直接计算几个分界点的值,只需判断 y/x 的值所
 * 对应的角度在哪个范围即可，不用实时求atan
 *--------------------------------------------
 *    角度范围        |         弧度范围
 *--------------------------------------------
 * 0     ~ 22.5  ------> 0         ~ 0.3926990
 * 22.5  ~ 67.5  ------> 0.3926990 ~ 1.1780972
 * 67.5  ~ 112.5 ------> 1.1780972 ~ 1.9634954
 * 112.5 ~ 157.5 ------> 1.9634954 ~ 2.7488935
 * 157.5 ~ 180   ------> 2.7488935 ~ 3.1415926
 *--------------------------------------------
 *         y/x值对应弧度
 *  0          ----  0.41421356  水平方向
 *  0.41421347 ----  2.41421356  右上、左下
 *  2.41421326 ---- -2.41421356  竖直方向
 * -2.41421362 ---- -0.41421356  左上、右下
 * -0.41421365 ----  0           水平方向
 ********************************************/
uint8_t Atan2(float y, float x)
{
    float tanNum;
    uint8_t alpha; //返回角度
    tanNum = y / x;
    if (tanNum > -0.41421365 && tanNum < 0.41421356)
        alpha = 0; //水平方向
    else if (tanNum >= 0.41421356 && tanNum < 2.41421356)
        alpha = 1; //右上、左下
    else if (tanNum <= -0.41421356 && tanNum > -2.41421362)
        alpha = 3; //左上、右下
    else
        alpha = 2; //竖直方向

    return alpha; //方向
}

//=============快速排序============LQ====
void Quicksort(float array[], uint8_t maxlen, uint8_t begin, uint8_t end)
{
    uint8_t i, j;

    if (begin < end)
    {
        i = begin + 1; // 将array[begin]作为基准数，因此从array[begin+1]开始与基准数比较！
        j = end;       // array[end]是数组的最后一位

        while (i < j)
        {
            if (array[i] > array[begin]) // 如果比较的数组元素大于基准数，则交换位置。
            {
                Swap(&array[i], &array[j]); // 交换两个数
                j--;
            }
            else
                i++; // 将数组向后移一位，继续与基准数比较。
        }

        /* -------------------------------------------------------------------------------------------------
         *跳出while循环后，i = j。
         * 此时数组被分割成两个部分  -->  array[begin+1] ~ array[i-1] < array[begin]
         *                           -->  array[i+1] ~ array[end] > array[begin]
         * 这个时候将数组array分成两个部分，再将array[i]与array[begin]进行比较，决定array[i]的位置。
         * 最后将array[i]与array[begin]交换，进行两个分割部分的排序！以此类推，直到最后i = j不满足条件就退出！
         *-------------------------------------------------------------------------------------------------*/

        if (array[i] >= array[begin]) // 这里必须要取等“>=”，否则数组元素由相同的值时，会出现错误！
            i--;

        Swap(&array[begin], &array[i]); // 交换array[i]与array[begin]

        Quicksort(array, maxlen, begin, i);
        Quicksort(array, maxlen, j, end);
    }
}

//==========交换两个数的值======LQ===
void Swap(float *a, float *b)
{
    float temp;

    temp = *a;
    *a = *b;
    *b = temp;
}













//以下全都没用！！！！！！！！！！！！！！！！






//摄像头中心线处理
//应当控制摄像头画面范围
// buffer 是处理后图片的首地址
//返回偏差值
//存在目标线定义 define
// short CenterLineFind (uint8_t* buffer, short width, short height, uint8_t* centerpoint)
// {
//     short i, j;
//     short left , right;
//     uint8_t *center = NULL;

//     //内存分配注意
//     center = buffer + width*height;

//     //找到每行的中点
//     for (j=0; j<height; j++)
//     {
//         for (i=width/2; i<width-20; i++)
//         {
//             if (*(buffer+width*j+i)>0xe0)
//             {
//                 right = i;
//             }
//         }
//         for (i=width/2; i>20; i--)
//         {
//             if (*(buffer+width*j+i)>0xe0)
//             {
//                 left = i;
//             }
//         }
//         center[j] = (left+right)/2;
//     }

//     memcpy (centerpoint, center, height);

//    //最小二乘法

//    float k,b;
//    //sum1=x[i]y[i] sum2=x[i] sum3=x^2[i]
//    int sum1=0, sum2=0, sum3=0;
//
//    for (i=0; i<height; i++)
//    {
//        sum1 = sum1 + i * center[i];
//        sum2 = center[i];
//        sum3 = center[i]*center[i];
//    }

//    k = (sum1-(float)height/2*sum2)/(sum3-sum2*sum2/height);
//    sum2 = sum2/height;
//    b = height/2 - k * sum2;

//    //最小二乘法拟合完成
//    //计算偏差
//
//    i = (height * targetlinePer/100 - b) / k;

//    //没用的东西
//    a->b = b;
//    a->k = k;
//
//     return i;
// }

// void CenterLine_show(CamStruct *ov5640)
// {
//     uint8_t *center;
//     short i=0;
//     center = ov5640->Cam_Result+ov5640->CamWide*ov5640->CanHigh;
//     CenterLineFind (ov5640->Cam_Result, ov5640->CamWide, ov5640->CanHigh, center);
//     for (i=0; i<ov5640->CanHigh; i++)
//     {
//         *(Usart_Buffer +ov5640->CamWide*i+center[i])=0x0f;
//     }
// }
