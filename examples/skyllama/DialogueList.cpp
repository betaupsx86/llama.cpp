#include "DialogueList.h"

using namespace RE;
namespace {
    DialogueItemList::DialogueItemList(GFxMovieView* a_view) : view(a_view), root(GFxValue::ValueType::kDisplayObject) {}

    //GFxValue root;       // 08 - kDisplayObject - "_level0.Menu_mc.inventoryLists.panelContainer.itemList"
    //GFxValue entryList;  // 20 - kArray - root.GetMember("entryList", &entryList);

    auto DialogueItemList::GetSelectedDialogueItem() -> DialogueItem* {
        if (unk50) {
            return nullptr;
        }

        GFxValue selectedIndex;
        auto success = root.GetMember("selectedIndex", &selectedIndex);
        if (!success || !selectedIndex.IsNumber()) {
            return nullptr;
        }

        auto idx = static_cast<std::int32_t>(selectedIndex.GetNumber());
        if (idx < 0 || static_cast<std::size_t>(idx) >= items.size()) {
            return nullptr;
        }

        return items[idx];
    }

    void DialogueItemList::Update() {
        auto player = PlayerCharacter::GetSingleton();
        return;
    }

    void DialogueItemList::Update(TESObjectREFR* a_owner) { Update_Impl(a_owner); }

    void DialogueItemList::Update_Impl(TESObjectREFR* a_owner) {
        return;
    }
}