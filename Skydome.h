#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Player.h"
#include "Affine.h"
#include <memory>
#include <cassert>

/// <summary>
/// 天球
/// </summary>
class Skydome
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};

