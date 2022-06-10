#pragma once
#include "WorldTransform.h"
#include "input.h"
#include "Model.h"
#include "DebugText.h"
#include <assert.h>
#include "Affine.h"

/// <summary>
/// 自キャラ
/// </summary>
class Player
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initailize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection_">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection &viewProjection_);

	void Move(Input* input_);
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 入力処理
	Input* input_ = nullptr;
	// デバックテキスト
	DebugText* debugText_ = nullptr;
};

