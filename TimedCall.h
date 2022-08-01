#pragma once

#include <functional>

/// <summary>
/// ��������
/// </summary>
class TimedCall
{
public:
	//�R���X�g���N�^
	TimedCall(std::function<void(void)> f, uint32_t time);
	//�X�V
	void Update();
	//�����Ȃ�true��Ԃ�
	bool IsFinished()
	{
		return finishedFlag_;
	}
private:

	//�c�莞��
	uint32_t time_;
	//�����t���O
	bool finishedFlag_ = false;

	//�R�[���o�b�N
	std::function<void(void)>func;
	
};

