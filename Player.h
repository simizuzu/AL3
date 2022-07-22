#pragma once
#include "WorldTransform.h"
#include "input.h"
#include "Model.h"
#include "DebugText.h"
#include <assert.h>
#include <memory.h>
#include <List>

#include "Affine.h"
#include "PlayerBullet.h"

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
	void Initailize(Model* model, uint32_t textureHandle, WorldTransform* parent, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection_">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection &viewProjection_);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 移動限界座標
	/// </summary>
	void ScreenOut();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	//衝突時コールバックを呼び出す
	void OnCollision();

	// 弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
		
	/// <summary>
	/// 旋回処理
	/// </summary>
	void Rotate();

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

	// 弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
};

