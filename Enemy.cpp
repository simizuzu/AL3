#include "Enemy.h"

void Enemy::Initailize(Model* model, const Vector3& position) {
	assert(model);

	model_ = model;
	//	�e�N�X�`���ǂݍ���
	textureHandle_ = TextureManager::Load("enemy.png");

	debugText_ = DebugText::GetInstance();

	// ���[���h�g�����X�t�H�[���̏�����
	worldTransform_.Initialize();
	// �����Ŏ󂯎�����������W���Z�b�g
	worldTransform_.translation_ = position;

	ApproschInitislize();
}
void Enemy::ApproschInitislize() {
	//���˃^�C�������Z�b�g����
	timedCalls_.push_back(std::make_unique<TimedCall>(std::bind(&Enemy::FireReset, this), kFireInterval));
}

void Enemy::ApproechMove() {
	// ���x�ݒ�
	Vector3 approchMove;
	const float enemySpeed = 0.2f;
	approchMove = { 0,0,-enemySpeed };

	// �ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += approchMove;
	// �K��̈ʒu�ɓ��B�����痣�E
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}

	// ���˃^�C�}�[�J�E���g�_�E��
	fireTimer--;
	// �w�莞�ԂɒB����
	if (fireTimer < 0) {
		// �e�𔭎�
		Fire();
		// ���˃^�C�}�[��������
		fireTimer = kFireInterval;
	}

	debugText_->SetPos(50, 100);
	debugText_->GetInstance()->Printf(
		"fierTimer:(%f)", fireTimer
	);
}

void Enemy::LeaveMove() {
	// ���x�ݒ�
	Vector3 leaveMove;
	const float enemySpeed = 0.2f;
	leaveMove = { -0.1f,0.1f,-enemySpeed };

	// �ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += leaveMove;
}

void Enemy::Fire() {
	// ���L�����̍��W���R�s�[
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// �e�𐶐����A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique <EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// �e��o�^����
	bullets_.push_back(std::move(newBullet));
}

void Enemy::FireReset()
{
	Fire();
	timedCalls_.push_back(std::make_unique<TimedCall>(std::bind(&Enemy::FireReset, this), kFireInterval));
}

void Enemy::Update(Affine* affine) {

	// �f�X�t���O�̗����������폜
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});

	//�폜
	timedCalls_.remove_if([](std::unique_ptr<TimedCall>& call) { return call->IsFinished(); });


	// switch���Ńt�F�[�Y����
	switch (phase_) {
	case Phase::Approach:
	default:
		ApproechMove();
		//Fire();
		break;
	case Phase::Leave:
		LeaveMove();
		break;
	}

	// �e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) { // if (bullet_ != nullptr)
		bullet->Update(affine);
	}

	// ���[���h�g�����X�t�H�[���̍X�V
	worldTransform_.matWorld_ = affine->CreateMatrix(worldTransform_);
	worldTransform_.TransferMatrix(); // �s��̓]��

	// �f�o�b�N����
	debugText_->SetPos(50, 40);
	debugText_->GetInstance()->Printf(
		"Enemy(%f,%f,%f)",
		worldTransform_.translation_.x,
		worldTransform_.translation_.y,
		worldTransform_.translation_.z);
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// ���f���̕`��
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// �e�`��
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) { // if (bullet_ != nullptr)
		bullet->Draw(viewProjection);
	}
}