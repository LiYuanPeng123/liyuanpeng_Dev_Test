
#include "Weapon/CancerWeaponActor.h"

UMeshComponent* ACancerWeaponActor::GetInteractionMesh_Implementation()
{
	return Execute_GetWeaponMesh(this);
}
