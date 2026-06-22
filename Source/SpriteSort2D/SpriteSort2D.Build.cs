using UnrealBuildTool;

public class SpriteSort2D : ModuleRules
{
	public SpriteSort2D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Paper2D",
			"DeveloperSettings"
		});
	}
}
