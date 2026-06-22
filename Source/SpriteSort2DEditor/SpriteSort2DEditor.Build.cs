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
			"SpriteSort2D"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"PropertyEditor",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"Projects"
		});
	}
}
