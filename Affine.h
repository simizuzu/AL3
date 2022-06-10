#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include <math.h>

class Affine
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Affine() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Affine() = default;
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
	/// 
	/// </summary>
	/// <param name="angle"></param>
	/// <returns></returns>
	Matrix4 RotationY(float angle);

	/// <summary>
	/// 平行移動
	/// </summary>
	/// <param name="translation">平行移動</param>
	/// <returns></returns>
	Matrix4 Translation(Vector3 translation);

	/// <summary>
	/// ワールド行列の計算
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <returns></returns>
	Matrix4 CreateMatrix(const WorldTransform& worldTransform);

	/// <summary>
	/// 1つ分のワールドトランスフォーム更新関数
	/// </summary>
	void WorldTransUpdate(WorldTransform& childWorldtrans);
#pragma endregion
};

