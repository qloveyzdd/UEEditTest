// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendEditor.h"

#include "ContentBrowserModule.h"
#include "ExtendEditorStyle.h"
#include "ExtendEditorCommands.h"
#include "LevelEditor.h"
#include "LevelEditorActions.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName ExtendEditorTabName("ExtendEditor");

#define LOCTEXT_NAMESPACE "FExtendEditorModule"

void FExtendEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FExtendEditorStyle::Initialize(); //Style的创建及注册
	FExtendEditorStyle::ReloadTextures();

	FExtendEditorCommands::Register(); //命令的映射

	PluginCommands = MakeShareable(new FUICommandList); //一个用于放置在编辑器某处的容器

	PluginCommands->MapAction( //绑定命令
		FExtendEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FExtendEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FExtendEditorModule::RegisterMenus));
	//简单的多播代理绑定了一个注册菜单的函数
}

void FExtendEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FExtendEditorStyle::Shutdown();

	FExtendEditorCommands::Unregister();
}

void FExtendEditorModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FExtendEditorModule::PluginButtonClicked()")),
		FText::FromString(TEXT("ExtendEditor.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FExtendEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection(TEXT("ExtendEditor"));
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.EndSection();
}

void FExtendEditorModule::AddMenuBarExtension(FMenuBarBuilder& Builder)
{
	// Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddPullDownMenu(LOCTEXT("ExtendEditor", "ExtendEditor"),LOCTEXT("CUSTOM", "CUSTOM"),
	                        FNewMenuDelegate::CreateRaw(this, &FExtendEditorModule::NewMenuBar), "ExtendEditor");
}

void FExtendEditorModule::NewMenuBar(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddMenuSeparator();
	Builder.AddSubMenu(LOCTEXT("ExtendEditor", "ExtendEditor"),LOCTEXT("CUSTOM", "CUSTOM"),
	                   FNewMenuDelegate::CreateRaw(this, &FExtendEditorModule::NewSubMenuBar));
}

void FExtendEditorModule::NewSubMenuBar(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);
	Builder.AddWidget(
		SNew(SImage),
		LOCTEXT("Image", "Just Image")
	);
	Builder.AddEditableText(
		LOCTEXT("ExtendEditor", "ExtendEditor"),LOCTEXT("CUSTOM", "CUSTOM"),
		FSlateIcon(),
		LOCTEXT("TTTTTT", "Just TTTTTTT")
	);

	Builder.AddWrapperSubMenu(
		LOCTEXT("ExtendEditor", "ExtendEditor"),LOCTEXT("CUSTOM", "CUSTOM"),
		FOnGetContent::CreateStatic<TSharedPtr<FUICommandList>>(&FExtendEditorModule::MakeNewWidget, PluginCommands),
		FSlateIcon()
	);
}

TSharedRef<SWidget> FExtendEditorModule::MakeNewWidget(TSharedPtr<class FUICommandList> PluginCommands)
{
	return SNew(SImage);
}

void FExtendEditorModule::AddToolBarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FExtendEditorCommands::Get().PluginAction);
}

void FExtendEditorModule::MyPrint(int Temp)
{
	UE_LOG(LogTemp, Display, TEXT("Actor Number = %d"), Temp);
}


TSharedRef<FExtender> FExtendEditorModule::SelectedCurrentActors(const TSharedRef<FUICommandList> MyUICommandList,
                                                                 const TArray<AActor*> AllActors)
{
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender);

	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Actor Number = %d"), AllActors.Num());
	// }
	// UE_LOG(LogTemp, Display, TEXT("Actor Number = %d"),AllActors.Num());
	MyPrint(AllActors.Num());

	if (AllActors.Num())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
		TSharedPtr<FUICommandList> LevelCommand = LevelEditorModule.GetGlobalLevelEditorActions(); //获取全局通用关卡编辑器命令列表

		Extender->AddMenuExtension("ActorControl", EExtensionHook::After, LevelCommand,
		                           FMenuExtensionDelegate::CreateRaw(
			                           this, &FExtendEditorModule::AddSelectActorButton));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
	}
	return Extender;
}

void FExtendEditorModule::AddSelectActorButton(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FExtendEditorCommands::Get().PluginAction);

	Builder.AddMenuSeparator();
	Builder.AddSubMenu(LOCTEXT("ExtendEditor", "ExtendEditor"),LOCTEXT("CUSTOM", "CUSTOM"),
	                   FNewMenuDelegate::CreateRaw(this, &FExtendEditorModule::NewSubMenuBar));
}

TSharedRef<FExtender> FExtendEditorModule::GetPathsFromEditor(const TArray<FString>& NewPaths)
{
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender);

	MyPrint(NewPaths.Num());

	if (NewPaths.Num())
	{
		FContentBrowserModule& LevelEditorModule = FModuleManager::GetModuleChecked<FContentBrowserModule>(
			"ContentBrowser");

		Extender->AddMenuExtension("NewFolder", EExtensionHook::After, PluginCommands,
		                           FMenuExtensionDelegate::CreateRaw(
			                           this, &FExtendEditorModule::AddSelectActorButton));
	}
	for (auto Tmp : NewPaths)
	{
		UE_LOG(LogTemp, Display, TEXT("Actor Path: %s"), *Tmp);
	}

	return Extender;
}

void FExtendEditorModule::RegisterMenus()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands,
		                               FMenuExtensionDelegate::CreateRaw(
			                               this, &FExtendEditorModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> MenuBarExtender = MakeShareable(new FExtender());
		MenuBarExtender->AddMenuBarExtension("Help", EExtensionHook::After, PluginCommands,
		                                     FMenuBarExtensionDelegate::CreateRaw(
			                                     this, &FExtendEditorModule::AddMenuBarExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuBarExtender);
	}

	{
		TSharedPtr<FExtender> ToolBarExtender = MakeShareable(new FExtender());
		ToolBarExtender->AddToolBarExtension("Content", EExtensionHook::After, PluginCommands,
		                                     FToolBarExtensionDelegate::CreateRaw(
			                                     this, &FExtendEditorModule::AddToolBarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);
	}

	{
		auto& MenuButtonArray = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
		MenuButtonArray.Add(
			FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(
				this, &FExtendEditorModule::SelectedCurrentActors));

		LevelViewportMenuExtender_SelectedActor = MenuButtonArray.Last().GetHandle();
		//MenuButtonArray是一个容器，获取到容器的最后一个，也就是Add的内容
	}

	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(
			"ContentBrowser");
		auto& MenuExtenderDelegates = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
		MenuExtenderDelegates.Add(
			FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FExtendEditorModule::GetPathsFromEditor));

		ContentBrowserMenuExtender_SelectedPaths = MenuExtenderDelegates.Last().GetHandle();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendEditorModule, ExtendEditor)
