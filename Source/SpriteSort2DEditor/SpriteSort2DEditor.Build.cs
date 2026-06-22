using UnrealBuildTool;

public class SpriteSort2DEditor : ModuleRules
{
	public SpriteSort2DEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Paper2D",
			"SpriteSort2D"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"Kismet",
			"PropertyEditor",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"Projects"
		});
	}
}
