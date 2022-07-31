#include "MathUtility.h"
#include<DirectXMath.h>
using namespace DirectX;

float MathUtility::ChangeRadi(const float& angle)
{
	float PI = 3.141592654f;
	float radian = angle * PI / 180;
	return radian;
}

float MathUtility::ChangeDegree(const float& angle)
{
	float PI = 3.141592654f;
	float radian = angle * 180 / PI;
	return radian;
}

void MathUtility::SetMatScale(Matrix4& m, const Vector3& scale)
{
	//スケーリング倍率を行列に設定する
	m.m[0][0] = scale.x;
	m.m[1][1] = scale.y;
	m.m[2][2] = scale.z;
	m.m[3][3] = 1.0f;
}

void MathUtility::SetMatRotX(Matrix4& m, const float radian)
{
	//x軸回転行列の各要素を設定する
	m.m[1][2] = sinf(radian);
	m.m[2][1] = -sinf(radian);
	m.m[2][2] = cosf(radian);
	m.m[0][0] = 1.0f;
	m.m[3][3] = 1.0f;
	m.m[1][1] = cosf(radian);
}

void MathUtility::SetMatRotY(Matrix4& m, const float radian)
{
	//y軸回転行列の各要素を設定する
	m.m[0][0] = cosf(radian);
	m.m[0][2] = -sinf(radian);
	m.m[2][0] = sinf(radian);
	m.m[2][2] = cosf(radian);
	m.m[1][1] = 1.0f;
	m.m[3][3] = 1.0f;
}

void MathUtility::SetMatRotZ(Matrix4& m, const float radian)
{
	//z軸回転行列の各要素を設定する
	m.m[0][0] = cosf(radian);
	m.m[0][1] = sinf(radian);
	m.m[1][0] = -sinf(radian);
	m.m[1][1] = cosf(radian);
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
}

void MathUtility::SetMatTrans(Matrix4& m, const Vector3& trans)
{
	//移動量を行列に設定する
	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	m.m[3][0] = trans.x;
	m.m[3][1] = trans.y;
	m.m[3][2] = trans.z;
}

Matrix4 MathUtility::IdentityMatrix()
{
	Matrix4 m;
	m.m[0][0] = 1.0f;
	m.m[1][1] = 1.0f;
	m.m[2][2] = 1.0f;
	m.m[3][3] = 1.0f;
	return m;
}

void MathUtility::MatrixCalculation(WorldTransform& worldtransform)
{
	//スケーリング行列を宣言
	Matrix4 matScale;
	//合成用回転行列を宣言
	Matrix4 matRot;
	//各軸用回転行列を宣言
	Matrix4 matRotX, matRotY, matRotZ;
	//平行移動行列宣言
	Matrix4 matTrans;
	SetMatScale(matScale, worldtransform.scale_);
	SetMatRotX(matRotX, worldtransform.rotation_.x);
	SetMatRotY(matRotY, worldtransform.rotation_.y);
	SetMatRotZ(matRotZ, worldtransform.rotation_.z);
	SetMatTrans(matTrans, worldtransform.translation_);
	//各軸の回転行列を合成
	matRot = matRotZ * matRotX * matRotY;
	//worldTransform_.matWorld_にmatScaleを掛け算して代入
	worldtransform.matWorld_ = matScale * matRot * matTrans;
}

Vector3 MathUtility::VecMatMul(Vector3& vec, Matrix4& mat)
{
	Vector3 retVec = {};

	retVec.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0];

	retVec.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1];

	retVec.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2];

	return retVec;
}