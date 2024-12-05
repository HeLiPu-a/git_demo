#include <cstdint>
#include "arm_math.h" // Include the ARM Math Library header

class vector2d{
private:
    float x = 0.0f; // 私有成员变量，存储向量的 x 分量
    float y = 0.0f; // 私有成员变量，存储向量的 y 分量

public:
    // 默认构造函数，初始化向量的 x 和 y 分量为 0.0f
    vector2d(); 
    // 构造函数，接受两个浮点数参数，用于初始化向量的 x 和 y 分量
    vector2d(float x, float y); 
    // 重载加法运算符，实现两个向量的加法运算
    vector2d operator+(vector2d v); 
    // 重载减法运算符，实现两个向量的减法运算
    vector2d operator-(vector2d v); 
    // 重载乘法运算符，实现两个向量的点积运算
    vector2d operator*(vector2d v); 
    // 重载除法运算符，实现两个向量的除法运算（分量相除）
    vector2d operator/(vector2d v); 
    // 重载加法运算符，实现向量与浮点数的加法运算
    vector2d operator+(float v1,float v2); 
    // 重载减法运算符，实现向量与浮点数的减法运算
    vector2d operator-(float v1,float v2); 
    // 重载乘法运算符，实现向量与浮点数的乘法运算
    vector2d operator*(float v); 
    // 重载除法运算符，实现向量与浮点数的除法运算（分量相除）
    vector2d operator/(float v); 
    float dotProduct(vector2d v); // 点积
    float magnitude(); // 模长
    float crossProduct(vector2d v); // 叉积
    void update(float x,float y); // 更新向量的 x 和 y 分量
};


