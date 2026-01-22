
#include "CancerAnimNativeTags.h"

namespace ViewModeTag
{
	UE_DEFINE_GAMEPLAY_TAG(FirstPerson, FName{TEXTVIEW("Cancer.ViewMode.FirstPerson")})
	UE_DEFINE_GAMEPLAY_TAG(ThirdPerson, FName{TEXTVIEW("Cancer.ViewMode.ThirdPerson")})
}

namespace LocomotionModeTag
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, FName{TEXTVIEW("Cancer.LocomotionMode.Grounded")})
	UE_DEFINE_GAMEPLAY_TAG(InAir, FName{TEXTVIEW("Cancer.LocomotionMode.InAir")})
}

namespace RotationModeTag
{
	UE_DEFINE_GAMEPLAY_TAG(OrientToMovement, FName{TEXTVIEW("Cancer.RotationMode.OrientToMovement")})
	UE_DEFINE_GAMEPLAY_TAG(Strafe, FName{TEXTVIEW("Cancer.RotationMode.Strafe")})
}

namespace StanceTag
{
	UE_DEFINE_GAMEPLAY_TAG(Stand, FName{TEXTVIEW("Cancer.Stance.Stand")})
	UE_DEFINE_GAMEPLAY_TAG(Crouch, FName{TEXTVIEW("Cancer.Stance.Crouch")})
}

namespace GaitTag
{
	UE_DEFINE_GAMEPLAY_TAG(Walk, FName{TEXTVIEW("Cancer.Gait.Walk")})
	UE_DEFINE_GAMEPLAY_TAG(Run, FName{TEXTVIEW("Cancer.Gait.Run")})
	UE_DEFINE_GAMEPLAY_TAG(Sprint, FName{TEXTVIEW("Cancer.Gait.Sprint")})
}