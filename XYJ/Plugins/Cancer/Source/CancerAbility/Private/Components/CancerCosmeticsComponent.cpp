#include "Components/CancerCosmeticsComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/CombatTargetCosmeticsModifierInterface.h"
#include "Kismet/GameplayStatics.h"

UCancerCosmeticsComponent::UCancerCosmeticsComponent()
{
}

void UCancerCosmeticsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCancerCosmeticsComponent::HandleImpactCosmetics(const FCombatImpactCosmetics& Parameters)
{
	ImpactTrailComponent = Parameters.ImpactTrailComponent;
	TArray<FSoftObjectPath> Cosmetics;

	if (Parameters.Sound.IsPending())
	{
		Cosmetics.Emplace(Parameters.Sound.ToSoftObjectPath());
	}

	if (Parameters.ImpactParticles.IsPending())
	{
		Cosmetics.Emplace(Parameters.ImpactParticles.ToSoftObjectPath());
	}

	if (Parameters.TrailParticles.IsPending())
	{
		Cosmetics.Emplace(Parameters.TrailParticles.ToSoftObjectPath());
	}

	if (Parameters.CameraShake.IsPending())
	{
		Cosmetics.Emplace(Parameters.CameraShake.ToSoftObjectPath());
	}

	if (Cosmetics.IsEmpty())
	{
		HandleImpactSound(Parameters.Sound.Get(), Parameters);
		HandleImpactParticles(Parameters.ImpactParticles.Get(), Parameters);
		HandleImpactTrailParticles(Parameters.TrailParticles.Get(), Parameters);
		HandleImpactCameraShake(Parameters.CameraShake.Get(), Parameters);
	}
	else
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(Cosmetics, [this, Parameters]()
		{
			// Make sure that this is still valid as the owner might be destroyed once the load completes.
			if (IsValid(this) && IsValid(GetWorld()) && !GetWorld()->bIsTearingDown)
			{
				if (Parameters.Sound.IsValid()) { HandleImpactSound(Parameters.Sound.Get(), Parameters); }
				if (Parameters.ImpactParticles.IsValid())
				{
					HandleImpactParticles(Parameters.ImpactParticles.Get(), Parameters);
				}
				if (Parameters.TrailParticles.IsValid())
				{
					HandleImpactTrailParticles(Parameters.TrailParticles.Get(), Parameters);
				}
				if (Parameters.CameraShake.IsValid())
				{
					HandleImpactCameraShake(Parameters.CameraShake.Get(), Parameters);
				}
			}
		});
	}
}

void UCancerCosmeticsComponent::HandleImpactParticles_Implementation(UNiagaraSystem* Particles,
                                                                     const FCombatImpactCosmetics& Parameters) const
{
	if (!IsValid(Particles))
	{
		return;
	}

	const UWorld* World = GetWorld();
	const FVector Location = Parameters.HitResult.Location;
	const FVector Scale = FVector::OneVector;
	const FRotator Rotation = FRotationMatrix::MakeFromZ(Parameters.HitResult.Normal).Rotator();

	UNiagaraComponent* NiagaraComponent;
	if (Parameters.NiagaraComponent)
	{
		NiagaraComponent = Parameters.NiagaraComponent;
		NiagaraComponent->SetAsset(Particles);
	}
	else
	{
		static constexpr bool bAutoDestroyParticles = true;
		static constexpr bool bAutoActivateParticles = false;

		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Particles,
		                                                                  Location, Rotation, Scale,
		                                                                  bAutoDestroyParticles,
		                                                                  bAutoActivateParticles);
	}

	if (!IsValid(NiagaraComponent))
	{
		return;
	}

	if (NiagaraCallbackParameterName != NAME_None)
	{
		UNiagaraSystem* System = NiagaraComponent->GetAsset();
		NiagaraComponent->SetAsset(System);
		NiagaraComponent->SetVariableObject(NiagaraCallbackParameterName, GetOwner());
	}

	// Adds relevant parameters to the niagara component, before activating it.
	ModifyImpactParticleComponent(NiagaraComponent, Parameters);

	AActor* Target = Parameters.HitResult.GetActor();
	if (IsValid(Target) && Target->Implements<UCombatTargetCosmeticsModifierInterface>())
	{
		// Allows the target to further modify the niagara component.
		// Useful to handle things like different blood colors, flesh types, etc.
		//
		ICombatTargetCosmeticsModifierInterface::Execute_ModifyParticleEffect(Target, NiagaraComponent,
		                                                                      Parameters.HitResult,
		                                                                      Parameters.SourceTags,
		                                                                      Parameters.TargetTags);
	}

	static constexpr bool bAsJustAttached = true;
	NiagaraComponent->ActivateSystem(bAsJustAttached);
}

void UCancerCosmeticsComponent::HandleImpactTrailParticles_Implementation(UNiagaraSystem* Particles,
                                                                          const FCombatImpactCosmetics& Parameters)
const
{
}

void UCancerCosmeticsComponent::HandleImpactSound_Implementation(USoundBase* Sound,
                                                                 const FCombatImpactCosmetics& Parameters) const
{
	if (!IsValid(Sound))
	{
		return;
	}

	AActor* Target = Parameters.HitResult.GetActor();
	if (!IsValid(Target))
	{
		return;
	}

	const FVector HitLocation = Parameters.HitResult.Location;

	UAudioComponent* AudioComponent = Parameters.AudioComponent;
	if (IsValid(AudioComponent))
	{
		AudioComponent->SetUsingAbsoluteLocation(true);
		AudioComponent->SetWorldLocation(HitLocation);
		AudioComponent->SetSound(Sound);

		// Allows modification of the Sound Cue or MetaSound before playing the audio.
		ModifyImpactAudioComponent(AudioComponent, Parameters);

		if (Target->Implements<UCombatTargetCosmeticsModifierInterface>())
		{
			// Allows the target to further modify the niagara component.
			// Useful to handle things like different blood colors, flesh types, etc.
			//
			ICombatTargetCosmeticsModifierInterface::Execute_ModifyAudioEffect(Target, AudioComponent,
			                                                                   Parameters.HitResult,
			                                                                   Parameters.SourceTags,
			                                                                   Parameters.TargetTags);
		}

		AudioComponent->Play();
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(Target->GetWorld(), Sound, HitLocation);
	}
}

void UCancerCosmeticsComponent::HandleImpactCameraShake_Implementation(TSubclassOf<UCameraShakeBase> CameraShakeClass,
                                                                       const FCombatImpactCosmetics& Parameters) const
{
	const APawn* Instigator = Parameters.Instigator;
	if (!IsValid(CameraShakeClass) || !IsValid(Instigator))
	{
		return;
	}

	const float Scale = GetImpactCameraShakeScale(Parameters);

	APlayerController* PlayerController = Cast<APlayerController>(Instigator->GetController());
	if (IsValid(PlayerController))
	{
		PlayCameraShakeOnController(PlayerController, CameraShakeClass, Scale);
	}
	else
	{
		AGameState* GameState = Instigator->GetWorld()->GetGameState<AGameState>();
		if (IsValid(GameState))
		{
			for (const APlayerState* PlayerState : GameState->PlayerArray)
			{
				PlayCameraShakeOnController(PlayerState->GetPlayerController(), CameraShakeClass, Scale);
			}
		}
	}
}

void UCancerCosmeticsComponent::ModifyImpactParticleComponent_Implementation(UNiagaraComponent* NiagaraComponent,
                                                                             const FCombatImpactCosmetics& Parameters)
const
{
	if (!IsValid(NiagaraComponent))
	{
		return;
	}

	const FVector HitLocation = Parameters.HitResult.Location;
	const FVector HitNormal = Parameters.HitResult.Normal;
	AActor* Target = Parameters.HitResult.GetActor();

	NiagaraComponent->SetVariableActor("user.Target", Target);
	NiagaraComponent->SetVariableVec3("user.HitLocation", HitLocation);
	NiagaraComponent->SetVariableVec3("user.HitNormal", HitNormal);
}

void UCancerCosmeticsComponent::ModifyImpactAudioComponent_Implementation(UAudioComponent* AudioComponent,
                                                                          const FCombatImpactCosmetics& Parameters)
const
{
	if (!IsValid(AudioComponent))
	{
		return;
	}

	AActor* Target = Parameters.HitResult.GetActor();
	AudioComponent->SetObjectParameter("Target", Target);
}

void UCancerCosmeticsComponent::PlayCameraShakeOnController(APlayerController* PlayerController,
                                                            const TSubclassOf<UCameraShakeBase>& CameraShakeClass,
                                                            float Scale)
{
	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		PlayerController->ClientStartCameraShake(CameraShakeClass, Scale);
	}
}

float UCancerCosmeticsComponent::GetImpactCameraShakeScale_Implementation(
	const FCombatImpactCosmetics& Parameters) const
{
	return 1.f;
}
