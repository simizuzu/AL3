#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "WorldTransform.h"

class Affine
{
public:
#pragma region 度数法
	/// <summary>
	/// 度数
	/// </summary>
	float DegreeMethod(const float& degree);
#pragma endregion

#pragma region アフィン変換宣言
	/// <summary>
	/// スケーリング
	/// </summary>
	Matrix4 Scaling(Vector3 scale);

	/// <summary>
	/// 回転Z
	/// </summary>
	Matrix4 RotationZ(float angle);

	/// <summary>
	/// 回転X
	/// </summary>
	Matrix4 RotationX(float angle);
	
	/// <summary>
	/// 回転Y
	/// </summary>
	Matrix4 RotationY(float angle);

	/// <summary>
	/// 平行移動
	/// </summary>
	Matrix4 Translation(Vector3 translation);

	/// <summary>
	/// ワールド行列の計算
	/// </summary>
	Matrix4 CreateMatrix(const WorldTransform& worldTransform);

	/// <summary>
	/// 1つ分のワールドトランスフォーム更新関数
	/// </summary>
	void WorldTransUpdate(WorldTransform& childWorldtrans);

	/// <summary>
	/// 複数描画するためのfor文更新処理
	/// </summary>
	void OrderUpdate();
#pragma endregion
};

