#include "StackOverloadModule.h"

#include <string>

#include "SML/Public/Patching/NativeHookManager.h"
#include "FactoryGame/Public/Equipment/FGWeapon.h"
#include "GameFramework/GameState.h"
#include "FGCharacterPlayer.h"
#include "Configuration/ConfigManager.h"


UFGItemDescriptor* GetItemClass(const TCHAR* Name)
{
    TObjectIterator<UFGItemDescriptor> it;
    FString ClassName = FString(Name) + "_C";
    UFGItemDescriptor* LoadedClass = nullptr;
    while(!!it)
    {
        UFGItemDescriptor* ItClass = *it;
        if(ItClass->GetName() == ClassName)
        {
            LoadedClass = ItClass;
            break;
        }
        ++it;
    }
    return LoadedClass;
}

void FStackOverloadModule::StartupModule() {
     FString configFilePath = UConfigManager::GetConfigurationFolderPath() + FString("/StackOverload.cfg");
	 if (!FPaths::FileExists(configFilePath)) {
	     TSharedRef<FJsonObject> defaultConfig = MakeShareable(new FJsonObject());
	     defaultConfig->SetNumberField("Stack Size", 500);
	     defaultConfig->SetBoolField("Use Multiplier", false);
	     defaultConfig->SetNumberField("Multiplier", 5);
	     FString defaultConfigString = "";
	     TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&defaultConfigString);
	     FJsonSerializer::Serialize(defaultConfig, Writer);
	     FFileHelper::SaveStringToFile(defaultConfigString, *configFilePath);
	 }
     // defaultConfig->SetStringField("List Type", "Whitelist or Blacklist");
     // TArray < TSharedPtr < FJsonValue >> array;
     // array.Add(MakeShareable(new FJsonValueString("Screw, for example")));
     // defaultConfig->SetArrayField("Item List",array);
     FString configstring = "";
     FFileHelper::LoadFileToString(configstring , *configFilePath);
     TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(configstring);
     TSharedPtr<FJsonObject> config = nullptr;

     FJsonSerializer::Deserialize(Reader, config);
     int stackSize = config->GetNumberField("Stack Size");
     bool useMultiplier = config->GetBoolField("Use Multiplier");
     int multiplier = config->GetNumberField("Multiplier");
     // FString ListType = config->GetStringField("List Type");
     // array = config->GetArrayField("Item List");
#if !WITH_EDITOR
     // SUBSCRIBE_METHOD(UFGItemDescriptor::GetStackSizeConverted, [=](auto& scope, TSubclassOf<UFGItemDescriptor> item) {
     //     if (item->IsValidLowLevel())
     //     {
     //       if (UFGItemDescriptor::GetForm(item) != EResourceForm::RF_LIQUID && item.GetDefaultObject()->mStackSize != EStackSize::SS_ONE)
     //       {
     //           if (useMultiplier)
     //           {
     //               scope.Override(scope(item) * multiplier);
     //           }
     //           else
     //           {
     //               scope.Override(stackSize);
     //           }
     //       }
     //     }
     // });
     SUBSCRIBE_METHOD(UFGItemDescriptor::GetStackSize, [=](auto& scope, TSubclassOf<UFGItemDescriptor> item) {
              if (item->IsValidLowLevel())
              {
                if (UFGItemDescriptor::GetForm(item) == EResourceForm::RF_SOLID && item.GetDefaultObject()->mStackSize != EStackSize::SS_ONE)
                {
                    if (useMultiplier)
                    {
                        scope.Override(scope(item) * multiplier);
                    }
                    else
                    {
                        scope.Override(stackSize);
                    }
                }
              }
          });
     // AFGWeapon* FGWeaponInstance = GetMutableDefault<AFGWeapon>();
     // SUBSCRIBE_METHOD_VIRTUAL(AFGWeapon::UnEquip, FGWeaponInstance, [](auto& scope, AFGWeapon* self)
     // {
     //     if (self != nullptr)
     //     {
     //         AFGCharacterPlayer* owner = static_cast<AFGCharacterPlayer*>(self->GetOwner());
     //         if(owner->IsValidLowLevel())
     //         {
     //             UFGInventoryComponent* inventory = owner->GetInventory();
     //             if(inventory->IsValidLowLevel())
     //             {
     //                 FInventoryStack stack = FInventoryStack(self->GetCurrentAmmo(), self->mAmmunitionClass);
     //                 self->mCurrentAmmo = 0;
     //                 inventory->AddStack(stack);
     //             }
     //         }
     //     }
     //
     // });
#endif
}

IMPLEMENT_GAME_MODULE(FStackOverloadModule, StackOverload)