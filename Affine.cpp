#include "Affine.h"
#include "Matrix4.h"
#include <math.h>
#include "WorldTransform.h"

const float PI = 3.141592654f;

/// <summary>
/// 度数法
/// </summary>
/// <param name="degree">度数の指定</param>
/// <returns>Θの値</returns>
float Affine::DegreeMethod(const float& degree) {
	float radian = degree * PI / 180.0f;
	return radian;
}

/// <summary>
/// 単位行列を設定
/// </summary>
/// <returns>単位行列</returns>
Matrix4 Identity() {
	// 単位行列を設定
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1.0f;
	matIdentity.m[1][1] = 1.0f;
	matIdentity.m[2][2] = 1.0f;
	matIdentity.m[3][3] = 1.0f;

	return matIdentity;
}

/// <summary>
/// スケーリング行列を設定
/// </summary>
/// <param name="scaleX">スケールX</param>
/// <param name="scaleY">スケールY</param>
/// <param name="scaleZ">スケールZ</param>
/// <returns>スケーリング行列</returns>
Matrix4 Affine::Scaling(Vector3 scale) {
	Matrix4 matScale;
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

/// <summary>
/// Z軸の回転行列を設定
/// </summary>
/// <param name="angle">= 1.0</param>
/// <returns>Z軸の回転行列</returns>
Matrix4 Affine::RotationZ(float angle) {
	Matrix4 matRotZ;
	matRotZ.m[0][0] = cosf(angle);
	matRotZ.m[0][1] = sinf(angle);

	matRotZ.m[1][1] = cosf(angle);
	matRotZ.m[1][0] = -sinf(angle);

	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	return matRotZ;
}

/// <summary>
/// X軸の回転行列を設定
/// </summary>
/// <param name="angle"></param>
/// <returns>X軸の回転行列</returns>
Matrix4 Affine::RotationX(float angle) {
	Matrix4 matRotX;
	matRotX.m[1][1] = cosf(angle);
	matRotX.m[1][2] = sinf(angle);

	matRotX.m[2][1] = -sinf(angle);
	matRotX.m[2][2] = cosf(angle);

	matRotX.m[0][0] = 1.0f;

	matRotX.m[3][3] = 1.0f;

	return matRotX;
}

/// <summary>
/// Y軸の回転行列を設定
/// </summary>
/// <param name="angle"></param>
/// <returns>Y軸の回転行列</returns>
Matrix4 Affine::RotationY(float angle) {
	Matrix4 matRotY;
	matRotY.m[0][0] = cosf(angle);
	matRotY.m[0][2] = -sinf(angle);

	matRotY.m[2][0] = sinf(angle);
	matRotY.m[2][2] = cosf(angle);

	matRotY.m[1][1] = 1.0f;
	matRotY.m[3][3] = 1.0f;

	return matRotY;
}

/// <summary>
/// 平行移動行列を設定
/// </summary>
/// <param name="tranlationX">平行移動X</param>
/// <param name="tranlationY">平行移動Y</param>
/// <param name="tranlationZ">平行移動Z</param>
/// <returns>平行移動行列</returns>
Matrix4 Affine::Translation(Vector3 translation) {
	Matrix4 matTrans;
	//単位行列を代入
	matTrans = Identity();

	matTrans.m[3][0] = translation.x;
	matTrans.m[3][1] = translation.y;
	matTrans.m[3][2] = translation.z;

	return matTrans;
}

/// <summary>
/// ワールド行列の計算(参照渡し)
/// </summary>
/// <param name="worldTransform">スケール, 回転, 平行移動</param>
/// <returns>ワールド行列</returns>
Matrix4 Affine::CreateMatrix(const WorldTransform& worldTransform) {
	Matrix4 matWorld;

	matWorld = Identity();
	matWorld *= Scaling(worldTransform.scale_);
	matWorld *= RotationZ(worldTransform.rotation_.z);
	matWorld *= RotationX(worldTransform.rotation_.x);
	matWorld *= RotationY(worldTransform.rotation_.y);
	matWorld *= Translation(worldTransform.translation_);

	return matWorld;
}

/// <summary>
/// 1つ分のワールドトランスフォーム更新関数
/// </summary>
/// <param name="childWorldtrans">子のworldTransform</param>
void Affine::WorldTransUpdate(WorldTransform& childWorldtrans) {
	childWorldtrans.matWorld_ = CreateMatrix(childWorldtrans); // 合成した行列の計算
	childWorldtrans.matWorld_ *= childWorldtrans.parent_->matWorld_; // parent_のワールド行列の掛け算代入
	childWorldtrans.TransferMatrix(); // 行列の転送
}
