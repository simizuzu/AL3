#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "TextureManager.h"

/// <summary>
/// 自キャラ
/// </summary>
class Player
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHnadle">テクスチャハンドル</param>
	void Initialize(Model* model,uint32_t textureHnadle);

	/// <summary>
	/// 更新
	/// </summary>
	void update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 自キャラ
	Player* player_ = nullptr;
};

