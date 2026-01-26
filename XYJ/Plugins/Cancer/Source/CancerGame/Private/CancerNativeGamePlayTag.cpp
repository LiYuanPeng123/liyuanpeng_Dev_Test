#include "CancerNativeGamePlayTag.h"


//UI层级
namespace UILayer
{
	UE_DEFINE_GAMEPLAY_TAG(Game, FName{TEXTVIEW("UI.Layer.Game")})
	UE_DEFINE_GAMEPLAY_TAG(GameMenu, FName{TEXTVIEW("UI.Layer.GameMenu")})
	UE_DEFINE_GAMEPLAY_TAG(Menu, FName{TEXTVIEW("UI.Layer.Menu")})
	UE_DEFINE_GAMEPLAY_TAG(Modal, FName{TEXTVIEW("UI.Layer.Modal")})
}

//输入层级
namespace InputLayer
{
	UE_DEFINE_GAMEPLAY_TAG(Game, FName{TEXTVIEW("Input.Layer.Game")})
	UE_DEFINE_GAMEPLAY_TAG(UI, FName{TEXTVIEW("Input.Layer.UI")})
}

//快捷插槽
namespace QuickBar
{
	UE_DEFINE_GAMEPLAY_TAG(QuickBar_Weapon, FName{TEXTVIEW("QuickBar.Weapon")})
	UE_DEFINE_GAMEPLAY_TAG(QuickBar_MilitaryClassics, FName{TEXTVIEW("QuickBar.MilitaryClassics")})
	UE_DEFINE_GAMEPLAY_TAG(QuickBar_Skill, FName{TEXTVIEW("QuickBar.Skill")})
	UE_DEFINE_GAMEPLAY_TAG(QuickBar_Magic, FName{TEXTVIEW("QuickBar.Magic")})
	UE_DEFINE_GAMEPLAY_TAG(QuickBar_Medicine, FName{TEXTVIEW("QuickBar.Medicine")})
}


UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event, "Combat.Event");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_NotSoul, TEXT("Combat.Event.Soul.Depleted"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Attack_Light_L, "Combat.Event.Attack.Light_L");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Attack_Light_R, "Combat.Event.Attack.Light_R");

//输入标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Input, "Combat.Event.Input");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Input_Start, "Combat.Event.Input.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Input_Trigger, "Combat.Event.Input.Trigger");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Input_Completed, "Combat.Event.Input.Completed");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Stagger_Start, TEXT("Combat.Event.Stagger.Start"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Stagger_End, TEXT("Combat.Event.Stagger.End"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_BeParried, TEXT("Combat.Event.BeParried"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Startdyging, TEXT("Combat.Event.Death.Start"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Deaddead, TEXT("Combat.Event.Death.End"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_Damage_Received, TEXT("Combat.Event.Damage.Received"));

//伤害检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_MeleeScan_Start, "Combat.Event.MeleeScan.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_MeleeScan_Stop, "Combat.Event.MeleeScan.Stop");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_ActiveMeleeScan, "Combat.State.ActiveMeleeScan");

//目标检测
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_TargetingPreset_Start, "Combat.Event.TargetingPreset.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_TargetingPreset_Stop, "Combat.Event.TargetingPreset.Stop");

//连招检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_CombatWindow, "Combat.State.CombatWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_CombatWindow_Start, "Combat.Event.CombatWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_CombatWindow_Stop, "Combat.Event.CombatWindow.Stop");

//连招派生检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_ComboLinkWindow, TEXT("Combat.State.ComboLinkWindow"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_ComboLinkWindow_Start, TEXT("Combat.Event.ComboLinkWindow.Start"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_ComboLinkWindow_Stop, TEXT("Combat.Event.ComboLinkWindow.Stop"));

//特殊技派生检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_SpecialAbilityWindow, TEXT("Combat.State.SpecialAbilityWindow"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_SpecialAbilityWindow_Start, TEXT("Combat.Event.SpecialAbilityWindow.Start"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_SpecialAbilityWindow_Stop, TEXT("Combat.Event.SpecialAbilityWindow.Stop"));


//多段技能的打断标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_PostWindow, "Combat.State.PostWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_PostWindow_Start, "Combat.Event.PostWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_PostWindow_Stop, "Combat.Event.PostWindow.Stop");


//伤害衔接窗口
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_LinkHitWindow, "Combat.State.LinkHitWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_LinkHitWindow_Start, "Combat.Event.LinkHitWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_LinkHitWindow_End, "Combat.Event.LinkHitWindow.End");

//闪避派生
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_DodgeLinkWindow, "Combat.State.DodgeLinkWindow");

//弹反派生
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_PerfectBlockLinkWindow, "Combat.State.PerfectBlockLinkWindow");


//格挡检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_BlockWindow, "Combat.State.BlockWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_BlockWindow_Start, "Combat.Event.BlockWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_BlockWindow_Stop, "Combat.Event.BlockWindow.Stop");

//弹反检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_PerfectBlockWindow, "Combat.State.PerfectBlockWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_PerfectBlockWindow_Start, "Combat.Event.PerfectBlockWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_PerfectBlockWindow_Stop, "Combat.Event.PerfectBlockWindow.Stop");

//无敌帧检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_InvulnerableWindow, "Combat.State.InvulnerableWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_InvulnerableWindow_Start, "Combat.Event.InvulnerableWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_InvulnerableWindow_Stop, "Combat.Event.InvulnerableWindow.Stop");

//无敌帧检测事件标签
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_AvoidWindow, "Combat.State.AvoidWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AvoidWindow_Start, "Combat.Event.AvoidWindow.Start");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AvoidWindow_Stop, "Combat.Event.AvoidWindow.Stop");

//Gameplay
UE_DEFINE_GAMEPLAY_TAG(Tag_Gameplay_DamageSelfDestruct, TEXT("Gameplay.Gameplay.DamageSelfDestruct"));

//GE自定义属性
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, TEXT("SetByCaller.Damage"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Heal, TEXT("SetByCaller.Heal"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_DamageCoeff, TEXT("SetByCaller.DamageCoeff"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_SoulDamageCoeff, TEXT("SetByCaller.SoulDamageCoeff"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_DamageCoeff_X, TEXT("SetByCaller.DamageCoeff_X"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_DamageCoeff_Y, TEXT("SetByCaller.DamageCoeff_Y"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_DamageCoeff_Z, TEXT("SetByCaller.DamageCoeff_Z"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Cure, TEXT("SetByCaller.Cure"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_IronBody, TEXT("SetByCaller.IronBody"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Soul, TEXT("SetByCaller.Soul"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_DamageBonusPct, TEXT("SetByCaller.DamageBonusPct"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Level, TEXT("SetByCaller.Level"));
UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Exp, TEXT("SetByCaller.Exp"));

//GameplayCue
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_HitNumber, TEXT("GameplayCue.HitNumber"));
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_Block, TEXT("GameplayCue.Block"));
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_Invulnerable, TEXT("GameplayCue.Invulnerable"));
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_Avoid, TEXT("GameplayCue.Avoid"));
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_PerfectBlock, TEXT("GameplayCue.PerfectBlock"));
UE_DEFINE_GAMEPLAY_TAG(Tag_GameplayCue_Damage, TEXT("GameplayCue.Damage"));




UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Component_Mesh, TEXT("Combat.Component.Mesh"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Component_Forwardreference, TEXT("Combat.Component.ForwardReference"));






UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Invulnerable, TEXT("Combat.State.Invulnerable"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Evading, TEXT("Combat.State.Evading"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Initiative_Combo, TEXT("Combat.State.Combo.Initiative"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Passive_Combo, TEXT("Combat.State.Combo.Passive"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Hit, TEXT("Combat.State.Hit"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_IronBody, TEXT("Combat.State.IronBody"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Stagger, TEXT("Combat.State.Stagger"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_KnockDown, TEXT("Combat.State.KnockDown"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_KnockFly, TEXT("Combat.State.KnockFly"));


//事件触发器
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_Stagger, TEXT("Combat.Event.AbilityTrigger.Stagger"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_DamageFeedback,
                       TEXT("Combat.Event.AbilityTrigger.DamageFeedback"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_Block, TEXT("Combat.Event.AbilityTrigger.Block"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_PerfectBlock, TEXT("Combat.Event.AbilityTrigger.PerfectBlock"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_Avoid, TEXT("Combat.Event.AbilityTrigger.Avoid"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_PerfectDodge, TEXT("Combat.Event.AbilityTrigger.PerfectDodge"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_Weak, TEXT("Combat.Event.AbilityTrigger.Weak"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_DrawWeapon, TEXT("Combat.Event.AbilityTrigger.DrawWeapon"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_PutAwayWeapon,
                       TEXT("Combat.Event.AbilityTrigger.PutAwayWeapon"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_ReceiveHit, TEXT("Combat.Event.AbilityTrigger.ReceiveHit"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_BlockWindow, "Combat.Event.AbilityTrigger.BlockWindow");
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Event_AbilityTrigger_Death, TEXT("Combat.Event.AbilityTrigger.Death"));


UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_Effect_ApplySoul, TEXT("Combat.Effect.ApplySoul"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_InvulnerableSoul, TEXT("Combat.State.InvulnerableSoul"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_RestoreSoul, TEXT("Combat.State.RestoreSoul"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_Weak, TEXT("Combat.State.Weak"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_UnlimitedHealth, TEXT("Combat.State.UnlimitedHealth"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_UnlimitedEnergy, TEXT("Combat.State.UnlimitedEnergy"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_State_UnlimitedSoul, TEXT("Combat.State.UnlimitedSoul"));


// 伤害反馈信息
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_Invulnerable, TEXT("Combat.DamageFeedback.Invulnerable"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_PerfectBlock, TEXT("Combat.DamageFeedback.PerfectBlock"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_Avoid, TEXT("Combat.DamageFeedback.Avoid"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_Block, TEXT("Combat.DamageFeedback.Block"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_Kill, TEXT("Combat.DamageFeedback.Kill"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Combat_DamageFeedBack_NoSoul, TEXT("Combat.DamageFeedback.NoSoul"));

UE_DEFINE_GAMEPLAY_TAG(Tag_Input_BlockAbility, TEXT("Input.BlockAbility"));


UE_DEFINE_GAMEPLAY_TAG(Movement_Walking, TEXT("Movement.Walking"));
UE_DEFINE_GAMEPLAY_TAG(Movement_Falling, TEXT("Movement.Falling"));
UE_DEFINE_GAMEPLAY_TAG(Movement_Flying, TEXT("Movement.Flying"));
UE_DEFINE_GAMEPLAY_TAG(Movement_Swimming, TEXT("Movement.Swimming"));
UE_DEFINE_GAMEPLAY_TAG(Movement_Custom, TEXT("Movement.Custom"));

UE_DEFINE_GAMEPLAY_TAG(Tag_Quickbar_Message_Slotchange, TEXT("Quickbar.Message.SlotChange"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Inventory_Message_Activeslotindex, TEXT("Inventory.Message.ActiveSlotIndex"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Inventory_Message_Switch, TEXT("Inventory.Message.SwitchStart"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Quickbar_Message_EquipmentChanged, TEXT("Quickbar.Message.EquipmentChanged"));
UE_DEFINE_GAMEPLAY_TAG(Tag_Inventory_Message_Stackchanged, TEXT("Inventory.Message.StackChanged"));

