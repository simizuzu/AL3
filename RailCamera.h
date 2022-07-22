#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "MathUtility.h"
#include "DebugText.h"
#include "Affine.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	RailCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RailCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="position">初期座標</param>
	/// <param name="rotation">初期角度</param>
	void Initialize(const Vector3& position, const Vector3& rotation);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	// ワールド行列を取得
	WorldTransform* GetWorldMatirx() { return &worldTransform_; }



private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// デバックテキスト
	DebugText* debugText_ = DebugText::GetInstance();
};

