#include "WorldTransform.h"
#include <MathUtility.h>

void WorldTransform::ParentChilldrenUpdate()
{
	MathUtility::MatrixCalculation(*this);
	matWorld_ *= parent_->matWorld_;
	TransferMatrix();
}