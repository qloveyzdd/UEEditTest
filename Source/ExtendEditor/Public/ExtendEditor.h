// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FExtendEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command. */
	void PluginButtonClicked();

	void AddMenuExtension(FMenuBuilder& Builder);

	void AddMenuBarExtension(FMenuBarBuilder& Builder);
	void NewMenuBar(FMenuBuilder& Builder);
	void NewSubMenuBar(FMenuBuilder& Builder);

	static TSharedRef<SWidget> MakeNewWidget(TSharedPtr<FUICommandList> InCommands);

	void AddToolBarExtension(FToolBarBuilder& Builder);

	void MyPrint(int Temp);

	TSharedRef<FExtender> SelectedCurrentActors(const TSharedRef<FUICommandList> MyUICommandList,
	                                            const TArray<AActor*> AllActors);

	void AddSelectActorButton(FMenuBuilder& Builder);
	
	TSharedRef<FExtender> GetPathsFromEditor(const TArray<FString>& NewPaths);


private:
	void RegisterMenus();

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	FDelegateHandle LevelViewportMenuExtender_SelectedActor;

	FDelegateHandle ContentBrowserMenuExtender_SelectedPaths;
};
