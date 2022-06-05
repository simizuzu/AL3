#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

Matrix4 Identity() {
	// 単位行列を設定
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1.0f;
	matIdentity.m[1][1] = 1.0f;
	matIdentity.m[2][2] = 1.0f;
	matIdentity.m[3][3] = 1.0f;

	return matIdentity;
}

// スケーリング行列を設定
Matrix4 Scaling(Vector3 scale) {
	Matrix4 matScale;
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matScale.m[3][3] = 1.0f;

	return matScale;
}

Matrix4 RotationZ(float angle) {
	//Z軸回りの回転
	Matrix4 matRotZ;
	matRotZ.m[0][0] = cosf(angle);
	matRotZ.m[0][1] = sinf(angle);

	matRotZ.m[1][1] = cosf(angle);
	matRotZ.m[1][0] = -sinf(angle);

	matRotZ.m[2][2] = 1.0f;
	matRotZ.m[3][3] = 1.0f;

	return matRotZ;
}

//回転行列を設定
Matrix4 RotationX(float angle) {
	//X軸周りの回転
	Matrix4 matRotX;
	matRotX.m[1][1] = cosf(angle);
	matRotX.m[1][2] = sinf(angle);

	matRotX.m[2][1] = -sinf(angle);
	matRotX.m[2][2] = cosf(angle);

	matRotX.m[0][0] = 1.0f;

	matRotX.m[3][3] = 1.0f;

	return matRotX;
}

Matrix4 RotationY(float angle) {
	//Y軸周りの回転
	Matrix4 matRotY;
	matRotY.m[0][0] = cosf(angle);
	matRotY.m[0][2] = -sinf(angle);

	matRotY.m[2][0] = sinf(angle);
	matRotY.m[2][2] = cosf(angle);

	matRotY.m[1][1] = 1.0f;
	matRotY.m[3][3] = 1.0f;

	return matRotY;
}

Matrix4 Translation(Vector3 tranlation) {
	//平行移動行列の宣言
	Matrix4 matTrans;
	//単位行列を代入
	matTrans = Identity();

	matTrans.m[3][0] = tranlation.x;
	matTrans.m[3][1] = tranlation.y;
	matTrans.m[3][2] = tranlation.z;

	return matTrans;
}

float PI = 3.141592654f;
float DegreeMethod(const float& degree) {
	float radian = degree * PI / 180.0f;
	return radian;
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	// 3Dモデルの生成
	model_ = Model::Create();

	// 乱数シード生成器
	std::random_device seed_gen;
	// メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, 2*PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (WorldTransform& worldTransform : worldTransforms_) {
		//ワールドトランスフォームの初期化
		worldTransform.Initialize();

		// X, Y, Z 方向のスケーリングを設定
		worldTransform.scale_ = { 1.0f,1.0f,1.0f };
		// スケーリング行列を宣言
		Matrix4 matScale = Scaling(worldTransform.scale_);

		// X, Y, Z 軸回りの回転角を設定
		worldTransform.rotation_ = { rotDist(engine) ,rotDist(engine) ,rotDist(engine) };
		// 合成用回転行列を宣言
		Matrix4 matRot = Identity(); // 単位行列を代入
		// 各軸回転行列を宣言
		Matrix4 matRotZ = RotationZ(worldTransform.rotation_.z);
		Matrix4 matRotX = RotationX(worldTransform.rotation_.x);
		Matrix4 matRotY = RotationY(worldTransform.rotation_.y);

		// X, Y, Z 軸回りの平行移動を設定
		worldTransform.translation_ = { posDist(engine),posDist(engine),posDist(engine) };
		// 平行行列を宣言
		Matrix4 matTrans = Translation(worldTransform.translation_);

		// 角回転行列の各要素を設定
		matRot *= matRotZ;
		matRot *= matRotX;
		matRot *= matRotY;

		// 単位行列の代入
		worldTransform.matWorld_ = Identity();
		// matScaleの代入
		worldTransform.matWorld_ *= matScale;
		// matScaleの代入
		worldTransform.matWorld_ *= matRot;
		// matTransの代入
		worldTransform.matWorld_ *= matTrans;

		// 行列の転送
		worldTransform.TransferMatrix();
	}

	// カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = DegreeMethod(45.0f);

	// アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	// ニアクリップ距離を設定
	viewProjection_.nearZ = 52.0f;
	// ファークリップ距離を設定
	viewProjection_.farZ = 53.0f;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する (アドレス渡し)
	AxisIndicator::SetTargetViewProjection(&viewProjection_);

	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {
	debugCamera_->Update();
	
	/*// 視点移動処理
	{
		//視点の移動ベクトル
		Vector3 move = { 0,0,0 };

		// 視点の移動速さ
		const float kEyeSpeed = 0.2f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_W)) {
			move = { 0,0,kEyeSpeed };
		}
		else if (input_->PushKey(DIK_S)) {
			move = { 0,0,-kEyeSpeed };
		}

		//視点移動（ベクトルの加算）
		viewProjection_.eye.x += move.x;
		viewProjection_.eye.y += move.y;
		viewProjection_.eye.z += move.z;

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf(
			"eye:(%f,%f,%f)", 
			viewProjection_.eye.x, 
			viewProjection_.eye.y, 
			viewProjection_.eye.z);
	}

	// 注視点移動処理
	{
		// 注視点の移動ベクトル
		Vector3 move = { 0,0,0 };

		// 視点の移動速さ
		const float kTargetSpeed = 0.2f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = { 0,0,-kTargetSpeed };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { 0,0,kTargetSpeed };
		}

		//視点移動（ベクトルの加算）
		viewProjection_.target += move;
		
		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 70);
		debugText_->Printf(
			"eye:(%f,%f,%f)", 
			viewProjection_.target.x, 
			viewProjection_.target.y,
			viewProjection_.target.z);
	}

	// 上方向回転処理
	{
		// 上方向の回転速さ[ラジアン/frame]
		const float kUpRotSpeed = 0.05f;

		// 押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_SPACE)) {
			viewAngle += kUpRotSpeed;
			// 2πを超えたら0に戻す
			viewAngle = fmodf(viewAngle, PI * 2.0f);
		}

		// 上方向ベクトルを計算（半径1の円周上の座標）
		viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 90);
		debugText_->Printf(
			"up(%f,%f,%f)",
			viewProjection_.up.x,
			viewProjection_.up.y,
			viewProjection_.up.z);
	}*/

	// FoV変更処理
	/* {
		// 上キーで視野角が広がる
		if (input_ ->PushKey(DIK_UP)) {
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, PI);
		// 下キーで視野角が狭まる
		}
		else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
		}

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 110);
		debugText_->Printf(
			"fovAngle(Degree):%f",
			DegreeMethod(viewProjection_.fovAngleY));
	}*/

	//クリップ距離変更処理
	{
		// 上下キーでニアクリップ距離を構成
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.nearZ += 0.1f;
		}
		else if(input_->PushKey(DIK_DOWN)) {
			viewProjection_.nearZ -= 0.1f;
		}

		// 行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 130);
		debugText_->Printf(
			"nearZ:%f", viewProjection_.nearZ);
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
	}
	// ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	//PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3{ 0,0,0 }, Vector3{ 100,100,100 }, Vector4{ 0xff,0x00,0x00,0xff });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
