#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <assert.h>
#include "Affine.h"

/// <summary>
/// 敵
/// </summary>
class Enemy
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initailize(Model* model, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="affine">アフィン変換</param>
	void Update(Affine* affine);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

private:

	/// <summary>
	/// 敵の移動
	/// </summary>
	void Move();

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;
};

