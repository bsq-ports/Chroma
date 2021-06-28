#pragma once

#include "ObjectColorizer.hpp"

#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "UnityEngine/Material.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

 namespace Chroma {
     class BombColorizer : public ObjectColorizer {
     private:
         UnityEngine::Renderer *_bombRenderer;

         inline static std::optional<UnityEngine::Color> GlobalColor;

         explicit BombColorizer(GlobalNamespace::NoteControllerBase *noteController);
     protected:
         void Refresh() override;

         std::optional<UnityEngine::Color> GlobalColorGetter() override;

     public:

         static std::shared_ptr<BombColorizer> New(GlobalNamespace::NoteControllerBase* noteController);

         using ColorizerMap = std::unordered_map<GlobalNamespace::NoteControllerBase*, std::shared_ptr<BombColorizer>>;
         static ColorizerMap Colorizers;

         static std::optional<UnityEngine::Color> getGlobalColor();

         static void GlobalColorize(std::optional<UnityEngine::Color> color);

         static void Reset();

         // extensions
         inline static std::shared_ptr<BombColorizer> GetBombColorizer(GlobalNamespace::NoteControllerBase* noteController) {
             auto it = Colorizers.find(noteController);
             if (it == Colorizers.end())
                 return nullptr;

             return it->second;
         }

         inline static void ColorizeBomb(GlobalNamespace::NoteControllerBase* noteController, std::optional<UnityEngine::Color> color) {
             GetBombColorizer(noteController)->Colorize(color);
         }
     };
 }

