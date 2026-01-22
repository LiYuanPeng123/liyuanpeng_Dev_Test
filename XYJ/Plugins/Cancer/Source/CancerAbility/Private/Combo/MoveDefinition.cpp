#include "Combo/MoveDefinition.h"
#include "Animation/AnimMontage.h"

UAnimMontage* UMoveDefinition::ResolveMontage() const
{
	return Montage.LoadSynchronous();
}

