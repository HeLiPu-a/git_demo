#include "vector2d.h"
// Implementation of member functions

// Implementation of member functions
vector2d::vector2d() {}

vector2d::vector2d(float x, float y) {
    this->x = x;
    this->y = y;
}

vector2d vector2d::operator+(vector2d v) {
    vector2d result;
    arm_add_f32(&this->x, &v.x, &result.x, 1);
    arm_add_f32(&this->y, &v.y, &result.y, 1);
    return result;
}

vector2d vector2d::operator-(vector2d v) {
    vector2d result;
    arm_sub_f32(&this->x, &v.x, &result.x, 1);
    arm_sub_f32(&this->y, &v.y, &result.y, 1);
    return result;
}

vector2d vector2d::operator*(vector2d v) {
    vector2d result;
    arm_mult_f32(&this->x, &v.x, &result.x, 1);
    arm_mult_f32(&this->y, &v.y, &result.y, 1);
    return result;
}

vector2d vector2d::operator/(vector2d v) {
    vector2d result;
    arm_divide_f32(&this->x, &v.x, &result.x, 1);
    arm_divide_f32(&this->y, &v.y, &result.y, 1);
    return result;
}

vector2d vector2d::operator+(float v) {
    vector2d result;
    result.x = this->x + v;
    result.y = this->y + v;
    return result;
}

vector2d vector2d::operator-(float v) {
    vector2d result;
    result.x = this->x - v;
    result.y = this->y - v;
    return result;
}

vector2d vector2d::operator*(float v) {
    vector2d result;
    result.x = this->x * v;
    result.y = this->y * v;
    return result;
}

vector2d vector2d::operator/(float v) {
    vector2d result;
    result.x = this->x / v;
    result.y = this->y / v;
    return result;
}

// 点积
float vector2d::dotProduct(vector2d v) {
    return (this->x * v.x) + (this->y * v.y);
}

// 模长
float vector2d::magnitude() {
    return arm_sqrt_f32((this->x * this->x) + (this->y * this->y));
}

// 叉积
float vector2d::crossProduct(vector2d v) {
    return (this->x * v.y) - (this->y * v.x);
}

void vector2d::update(float x, float y) {
    this->x = x;
    this->y = y;
}