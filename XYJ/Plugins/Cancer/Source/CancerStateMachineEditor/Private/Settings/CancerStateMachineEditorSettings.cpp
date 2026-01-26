#include "Settings/CancerStateMachineEditorSettings.h"

UCancerStateMachineEditorSettings::UCancerStateMachineEditorSettings()
{
	EntryNodeColor = FLinearColor(0.86f, 0.08f, 0.24f);   // Crimson (绯红)
	ConditionEdgeColor = FLinearColor(1.0f, 0.0f, 0.0f); // Red (真红色)
	EventEdgeColor = FLinearColor(1.0f, 0.5f, 0.0f);     // Orange (保持不变)
	TaskNodeColor = FLinearColor(0.4f, 0.7f, 1.0f);      // Light Blue (浅蓝)
	InlineNodeColor = FLinearColor(0.5f, 0.0f, 0.5f);    // Purple (紫色)
	CustomBPNodeColor = FLinearColor(0.0f, 0.2f, 0.6f);  // Dark Blue (深蓝)
	EndNodeColor = FLinearColor(0.2f, 0.2f, 0.2f);       // Dark Gray (深灰色)
	DefaultStateNodeColor = FLinearColor::Gray;
	GlobalNodeColor = FLinearColor(0.5f, 0.2f, 0.8f);    // Purple/Magenta
	SubStateMachineNodeColor = FLinearColor(0.0f, 0.8f, 0.8f); // Cyan (青色)
}

UCancerStateMachineEditorSettings* UCancerStateMachineEditorSettings::Get()
{
	return GetMutableDefault<UCancerStateMachineEditorSettings>();
}
