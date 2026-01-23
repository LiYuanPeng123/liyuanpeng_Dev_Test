
#include "XYJ_Character.h"
#include "XYJ_CharacterMovementComponent.h"


AXYJ_Character::AXYJ_Character(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UXYJ_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	
}

void AXYJ_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

