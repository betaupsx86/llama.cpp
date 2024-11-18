#include <spdlog/sinks/basic_file_sink.h>
#include "PCH.h"
#include "common.h"
#include "arg.h"


using namespace SKSE;
using namespace RE;
namespace logger = SKSE::log;

struct RE::TESTopicInfoEvent {
    Actor* speaker;      // 00 - NiTPointer<Actor>
    void* unk04;         // 04 - BSTSmartPointer<REFREventCallbacks::IEventCallback>
    FormID topicInfoID;  // 08
    bool flag;           // 0C

    inline bool IsStarting() const { return !flag; }
    inline bool IsStopping() const { return flag; }
};

class RE::MenuModeChangeEvent {
public:
    enum class Mode { kNone = static_cast<std::underlying_type_t<Mode>>(-1), kHidden = 0, kDisplayed = 1 };

    BSFixedString menu;                         // 00
    stl::enumeration<Mode, std::uint8_t> mode;  // 08
};

class CharEvent : public InputEvent {
public:
    inline static constexpr auto RTTI = RTTI_CharEvent;

    ~CharEvent() override;  // 00

    // members
    std::uint32_t keycode;  // 18
    std::uint32_t pad1C;    // 1C
};

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) {
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
        return;
    }
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::info);
}

struct CustomHandler : public MenuEventHandler {
public:
    ~CustomHandler() override;  // 00

    // override
    bool CanProcess(InputEvent* a_event) override;      // 01
    bool ProcessKinect(KinectEvent* a_event) override;  // 02
    bool ProcessButton(ButtonEvent* a_event) override;  // 05
    bool processEnter = false;
};

CustomHandler::~CustomHandler(){};
bool CustomHandler::CanProcess(InputEvent* a_event) {
    if (a_event == nullptr || UI::GetSingleton()->GameIsPaused() || !UI::GetSingleton()->IsMenuOpen(DialogueMenu::MENU_NAME)) {
        return false;
    }

    if (a_event && !UI::GetSingleton()->GameIsPaused() && UI::GetSingleton()->IsMenuOpen(DialogueMenu::MENU_NAME)) {
        auto lastSelectedDialogue = RE::MenuTopicManager::GetSingleton()->lastSelectedDialogue;
        auto console = RE::ConsoleLog::GetSingleton();
        auto manager = MenuTopicManager::GetSingleton();
        CharEvent* charEvent = nullptr;
        ThumbstickEvent* thumbEvent = nullptr;
        switch (a_event->GetEventType()) {
            case INPUT_EVENT_TYPE::kButton:
                // RE::ConsoleLog::GetSingleton()->Print("kButton");
                //if (lastKeycode == 13) {
                //    lastKeycode = 0;
                //    return true;
                //}
                //RE::ConsoleLog::GetSingleton()->Print("Button pressed: %u, %u", a_event->AsButtonEvent()->GetIDCode(),
                //                                      a_event->AsButtonEvent()->IsUp()?0:1);
                if (a_event->AsButtonEvent()->GetIDCode() == 28) {
                    return true;
                }
                return false;
                break;
            case INPUT_EVENT_TYPE::kKinect:
                // RE::ConsoleLog::GetSingleton()->Print("kKinect");
                break;
            case INPUT_EVENT_TYPE::kChar:
                charEvent = nullptr;
                // Perform the dynamic cast
                if ((charEvent =  static_cast<CharEvent*>(a_event))) {
                    //RE::ConsoleLog::GetSingleton()->Print("Key pressed: %u", charEvent->keycode);
                    return true;
                } else {
                    RE::ConsoleLog::GetSingleton()->Print("Event is not of type CharEvent");
                }
                //if (process && manager && manager->dialogueList && !manager->dialogueList->empty() && manager->dialogueList->front()) {
                //    static std::random_device rd;
                //    static std::mt19937 gen(rd());
                //    // Define the range (10000 to 99999 ensures exactly 5 digits)
                //    std::uniform_int_distribution<> dis(10000, 99999);
                //    int randomInt = dis(gen);

                //    auto fullname = manager->dialogueList->front()->parentTopic->As<TESFullName>();
                //    using func_t = void (*)(TESFullName*, const char*);
                //    REL::Relocation<func_t> func{RELOCATION_ID(22318, 22791)};
                //    func(fullname, std::to_string(randomInt).c_str());
                //    manager->dialogueList->front()->topicText = std::to_string(randomInt);

                //    RE::UIMessage msg;
                //    msg.menu = RE::DialogueMenu::MENU_NAME;
                //    msg.type = RE::UI_MESSAGE_TYPE::kUpdate;
                //    msg.data = new IUIMessageData();
                //    auto menu = UI::GetSingleton()->GetMenu<DialogueMenu>();
                //   
                //    ///////////////////////////////////////////////////////////////////
                //    auto to_string = [](GFxValue::ValueType valueType) -> std::string {
                //        switch (valueType) {
                //            case GFxValue::ValueType::kUndefined:
                //                return "Undefined";
                //            case GFxValue::ValueType::kNull:
                //                return "Null";
                //            case GFxValue::ValueType::kBoolean:
                //                return "Boolean";
                //            case GFxValue::ValueType::kNumber:
                //                return "Number";
                //            case GFxValue::ValueType::kString:
                //                return "String";
                //            case GFxValue::ValueType::kStringW:
                //                return "StringW";
                //            case GFxValue::ValueType::kObject:
                //                return "Object";
                //            case GFxValue::ValueType::kArray:
                //                return "Array";
                //            case GFxValue::ValueType::kDisplayObject:
                //                return "DisplayObject";
                //            case GFxValue::ValueType::kManagedBit:
                //                return "ManagedBit";
                //            case GFxValue::ValueType::kConvertBit:
                //                return "ConvertBit";
                //            case GFxValue::ValueType::kValueMask:
                //                return "ValueMask";
                //            case GFxValue::ValueType::kTypeMask:
                //                return "TypeMask";
                //            case GFxValue::ValueType::kConvertBoolean:
                //                return "ConvertBoolean";
                //            case GFxValue::ValueType::kConvertNumber:
                //                return "ConvertNumber";
                //            case GFxValue::ValueType::kConvertString:
                //                return "ConvertString";
                //            case GFxValue::ValueType::kConvertStringW:
                //                return "ConvertStringW";
                //            default:
                //                throw std::invalid_argument("Unknown ValueType");
                //        }
                //    };
                //    auto movie = menu->uiMovie;
                //    GFxValue stateVal;
                //    GFxValue topicIndexVal;
                //    GFxValue topicList;
                //    movie->GetVariable(&stateVal, "_level0.DialogueMenu_mc.eMenuState");
                //    movie->GetVariable(&topicIndexVal, "_level0.DialogueMenu_mc.TopicList.iSelectedIndex");
                //    movie->GetVariable(&topicList, "_level0.DialogueMenu_mc.TopicList");

                //    GFxValue entryList;
                //    topicList.GetMember("entryList", &entryList);
                //    if (entryList.IsArray() && entryList.GetArraySize() > 0) {
                //        GFxValue entry;
                //        entryList.GetElement(0, &entry);
                //        GFxValue text;
                //        entry.GetMember("text", &text);

                //        const char* permanentStr = manager->dialogueList->front()->topicText.c_str();
                //        GFxValue new_text(permanentStr);
                //        entry.SetMember("text", new_text);
                //        // console->Print(fmt::format("entry 0 of {} type {} has text {} has infoIndex {}  has saidOnce
                //        // {} has flags {}", entryList.GetArraySize(), to_string(entry.GetType()),
                //        // entry.HasMember("text"),
                //        //         entry.HasMember("infoIndex"), entry.HasMember("saidOnce"), entry.HasMember("flags"))
                //        //                    .c_str());
                //    }
                //    console->Print(
                //        fmt::format("Running update return {}", topicList.Invoke("UpdateList", NULL, NULL, 0)).c_str());
                //    // menu->ProcessMessage(msg);
                //}
                break;
            case INPUT_EVENT_TYPE::kDeviceConnect:
                // RE::ConsoleLog::GetSingleton()->Print("kDeviceConnect");
                break;
            case INPUT_EVENT_TYPE::kMouseMove:
                // RE::ConsoleLog::GetSingleton()->Print("kMouseMove");
                break;
            case INPUT_EVENT_TYPE::kThumbstick:
                // RE::ConsoleLog::GetSingleton()->Print("kThumbstick");
                thumbEvent = nullptr;
                // Perform the dynamic cast
                if ((thumbEvent = static_cast<ThumbstickEvent*>(a_event))) {
                    RE::ConsoleLog::GetSingleton()->Print("Thumbstick pressed: %u", thumbEvent->GetIDCode());
                    return true;
                } else {
                    RE::ConsoleLog::GetSingleton()->Print("Event is not of type CharEvent");
                }
                break;
            default:
                break;
        }
        // for (auto handler : RE::MenuControls::GetSingleton()->handlers) {
        //     if (handler != nullptr && handler != this) {
        //         RE::ConsoleLog::GetSingleton()->Print(fmt::format("can proces {}",
        //         handler->CanProcess(a_event)).c_str());
        //     }
        // }
    }
    return false;
}
bool CustomHandler::ProcessKinect(KinectEvent* a_event) { return true; }
bool CustomHandler::ProcessButton(ButtonEvent* a_event) {
    if (a_event->GetIDCode() == 28) {
        if (a_event->IsPressed() && !processEnter) {
            processEnter = true;
            //send message
            auto next_iter = MenuTopicManager::GetSingleton()->dialogueList->front()->unk40->begin();
            next_iter++;
            RE::ConsoleLog::GetSingleton()->Print("Processing Enter");
            RE::ConsoleLog::GetSingleton()->Print(
                MenuTopicManager::GetSingleton()->dialogueList->front()->unk40->front()->voice.c_str());
            MenuTopicManager::GetSingleton()->dialogueList->front()->unk40->front()->voice = (*next_iter)->voice;
            RE::ConsoleLog::GetSingleton()->Print(
                MenuTopicManager::GetSingleton()->dialogueList->front()->unk40->front()->voice.c_str());
        }
        if (!a_event->IsPressed()) {
            processEnter = false;
            RE::ConsoleLog::GetSingleton()->Print("Processing Enter Done");
        }
    }
    return true;
}
bool custom_handler = false;
CustomHandler* handler = new CustomHandler();
bool topicInfoStarting = false;

class EventProcessor : public RE::BSTEventSink<RE::TESActivateEvent>,
                       public RE::BSTEventSink<RE ::TESTopicInfoEvent>,
                       public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
                       public RE::BSTEventSink<RE::MenuModeChangeEvent> {
    EventProcessor() = default;
    ~EventProcessor() = default;
    EventProcessor(const EventProcessor&) = delete;
    EventProcessor(EventProcessor&&) = delete;
    EventProcessor& operator=(const EventProcessor&) = delete;
    EventProcessor& operator=(EventProcessor&&) = delete;

public:
    static EventProcessor& GetSingleton() {
        static EventProcessor singleton;
        return singleton;
    }
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
                                          RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
        if (event && event->menuName == DialogueMenu::MENU_NAME) {
            RE::ConsoleLog::GetSingleton()->Print(
                fmt::format("Processor Event Menu Open Close, name {} opening {}", event->menuName, event->opening)
                    .c_str());
        }
        return RE::BSEventNotifyControl::kContinue;
    }
    RE::BSEventNotifyControl ProcessEvent(const RE::MenuModeChangeEvent* event,
                                          RE::BSTEventSource<RE::MenuModeChangeEvent>*) override {
        if (event && event->menu == DialogueMenu::MENU_NAME) {
            RE::ConsoleLog::GetSingleton()->Print(fmt::format("Processor Event Menu Mode Change, name {} opening {}",
                                                              event->menu, event->mode.underlying())
                                                      .c_str());
        }
        return RE::BSEventNotifyControl::kContinue;
    }
    RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event,
                                          RE::BSTEventSource<RE::TESActivateEvent>*) override {
        return RE::BSEventNotifyControl::kContinue;
    }
    // Log information about Activate events that happen in the game
    RE::BSEventNotifyControl ProcessEvent(const RE::TESTopicInfoEvent* event,
                                          RE::BSTEventSource<RE::TESTopicInfoEvent>*) override {
        auto manager = MenuTopicManager::GetSingleton();
        if (event != nullptr && manager != nullptr) {
            // RE::ConsoleLog::GetSingleton()->Print(
            //     fmt::format("TESTopicInfoEvent {} starting {} speaker is player {}", event->topicInfoID,
            //                 event->IsStarting(),
            //                 (event->speaker != nullptr) ? event->speaker->IsPlayer() ? "true" : "false" : "unkown")
            //         .c_str());
            //  auto* form = TESForm::LookupByID(event->topicInfoID);
            //  if (form && form->GetFormType() == FormType::Info) {
            //      TESTopicInfo* topicInfo = reinterpret_cast<TESTopicInfo*>(form);
            //      if (topicInfo && topicInfo->parentTopic) {
            //          RE::ConsoleLog::GetSingleton()->Print(
            //              fmt::format("topicInfo {}", topicInfo->parentTopic->GetFullName()).c_str());
            //      }
            //  }

            auto* correctNodePtr =
                reinterpret_cast<BSSimpleList<MenuTopicManager::Dialogue*>::Node*>(manager->selectedResponseNode);
            if (event->IsStarting() && correctNodePtr &&
                correctNodePtr->item->parentTopicInfo->GetFormID() == event->topicInfoID) {
                RE::ConsoleLog::GetSingleton()->Print("=================SelectedTopicStarting===================================");
                topicInfoStarting = true;


                //// Grab Form
                //constexpr std::uint32_t bufferSize = 512;
                //char buffer[bufferSize];
                //memset(buffer, 0, bufferSize);
                //correctNodePtr->item->parentTopicInfo->GetFormDetailedString(buffer, bufferSize);
                //RE::ConsoleLog::GetSingleton()->Print("=============================================================================");
                //RE::ConsoleLog::GetSingleton()->Print(buffer);
                //logger::info("GetFormDetailedString: {}", buffer);

                if (strcmp(correctNodePtr->item->parentQuest->GetFormEditorID(), "PerkInvestor") == 0) {
                    auto new_response = new DialogueResponse();
                    new_response->text = "THIS WAS GENERATED PUNK";
                    new_response->percent = correctNodePtr->item->responses.front()->percent;
                    new_response->voice = correctNodePtr->item->responses.front()->voice;
                    new_response->voiceSound = correctNodePtr->item->responses.front()->voiceSound;
                    new_response->animFaceArchType = correctNodePtr->item->responses.front()->animFaceArchType;
                    new_response->useEmotion = correctNodePtr->item->responses.front()->useEmotion;
                    new_response->listenIdle = correctNodePtr->item->responses.front()->listenIdle;
                    new_response->speakerIdle = correctNodePtr->item->responses.front()->speakerIdle;
                    new_response->soundLip = correctNodePtr->item->responses.front()->soundLip;                    

                    //auto dialogue_data = correctNodePtr->item->parentTopicInfo->GetDialogueData(event->speaker);
                    //dialogue_data.responses.insert_after(dialogue_data.responses.begin(), new_response);
                    //dialogue_data.responses.front()->text = "THIS WAS CHANGED";
                    //correctNodePtr->item->unk40->front()->text = "AND THIS WAS CHANGED PUNK";
                    //correctNodePtr->item->unk40->push_front(new_response);
                    // correctNodePtr->item->responses.front()->text = "AND THIS WAS CHANGED PUNK";
                    //dialogue_data.responses.insert_after(dialogue_data.responses.begin(), new_response);
                    //auto responselist = *correctNodePtr->item->unk40;
                    //auto responselist = dialogue_data.responses;
                    //correctNodePtr->item->responses.insert_after(correctNodePtr->item->responses.begin(),new_response);
                    auto responselist = correctNodePtr->item->responses;
                    std::size_t responsecount = 0;     
                    
                    for (auto response : responselist) {            
                        responsecount++;
                        if (response == nullptr) continue;
                        response->voice = responselist.front()->voice;
/*                        RE::ConsoleLog::GetSingleton()->Print(
                            fmt::format("ChatGPT topic said {}, info pointer {}, Response {}, text{}, front voice {}, voice form {}",
                                correctNodePtr->item->neverSaid,
                                std::to_string(reinterpret_cast<uintptr_t>(correctNodePtr->item->parentTopicInfo)),
                                responsecount,
                                response->text,
                                response->voice,
                                (response->voiceSound) ? response->voiceSound->GetFile()->GetFilename() : "None")
                                .c_str());  */   
                    }                  


                    //RE::ConsoleLog::GetSingleton()->Print(
                    //    fmt::format("ChatGPT Current Response {}.", dialogue_data.currentResponse->item->text).c_str());
                }
            }
            auto currentTopicInfo = manager->currentTopicInfo;
            if (currentTopicInfo) {
                // auto response = currentTopicInfo->GetDialogueData(event->speaker).currentResponse;
                //RE::ConsoleLog::GetSingleton()->Print(
                //    fmt::format("currentTopicInfo {}", currentTopicInfo->GetFormID()).c_str());
                if (currentTopicInfo->GetFormID() == event->topicInfoID && event->IsStopping() &&
                    manager->dialogueList) {
                    auto list = *manager->dialogueList;
                    std::size_t count = 0;
                    for (auto dialogue : list) {
                        count++;
                    }
                    //RE::ConsoleLog::GetSingleton()->Print(fmt::format("DialogueList size {}", count).c_str());
                    // auto source = RE::UI::GetSingleton()->GetEventSource<MenuOpenCloseEvent>();
                    // MenuOpenCloseEvent event;
                    // event.menuName = DialogueMenu::MENU_NAME;
                    // event.opening = true;
                    // source->SendEvent(&event);
                }
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};

int64_t* PopulateResponseText(TESTopicInfo* a1, int64_t* a2);
REL::Relocation<decltype(PopulateResponseText)>& GetHookedFunction() noexcept {
    static REL::Relocation<decltype(PopulateResponseText)> value(RELOCATION_ID(34436, 35256), 0x3D);
    return value;
}
REL::Relocation<decltype(PopulateResponseText)> OriginalPopulateResponseText;
int64_t* PopulateResponseText(TESTopicInfo* a1, int64_t* a2) {
    OriginalPopulateResponseText(a1, a2);
    logger::info("PopulateResponseText");
    if (a1 == nullptr) {
        RE::ConsoleLog::GetSingleton()->Print("Calling Populate response text. No TopicInfo");
    } else {
        RE::ConsoleLog::GetSingleton()->Print(
            fmt::format("Calling Populate response text. {}", a1->parentTopic->GetFullName()).c_str());
        //if (a2) {
        //    RE::ConsoleLog::GetSingleton()->Print(
        //        fmt::format("Calling Populate response text. Response {}",
        //                    reinterpret_cast<TESTopicInfo::ResponseData*>(a2)->responseText)
        //            .c_str());
        //}
    }
    return nullptr;
}

int64_t* PopulateResponseText1(TESTopicInfo* a1, int64_t* a2);
REL::Relocation<decltype(PopulateResponseText1)>& GetHookedFunction1() noexcept {
    static REL::Relocation<decltype(PopulateResponseText1)> value(RELOCATION_ID(34434, 35254), 0x26A);
    return value;
}
REL::Relocation<decltype(PopulateResponseText1)> OriginalPopulateResponseText1;
int64_t* PopulateResponseText1(TESTopicInfo* a1, int64_t* a2) {
    if (a1 == nullptr) {
        RE::ConsoleLog::GetSingleton()->Print("Calling Populate response text1. No TopicInfo");
    } else {
        RE::ConsoleLog::GetSingleton()->Print(fmt::format("Calling Populate response text1. {}", a1->parentTopic->GetFullName())
                .c_str());
    }
    OriginalPopulateResponseText1(a1, a2);
    RE::ConsoleLog::GetSingleton()->Print(
        fmt::format("Calling Populate response text1. pointer value {}", int64_t(*a2)).c_str());
    RE::ConsoleLog::GetSingleton()->Print(
        fmt::format("Calling Populate response text1. pointer value {}", uint64_t(*a2)).c_str());
    //if (a2) {
    //    RE::ConsoleLog::GetSingleton()->Print(
    //        fmt::format("Calling Populate response text1. Response {}",
    //                    reinterpret_cast<TESTopicInfo::ResponseData*>(*a2)->responseText)
    //            .c_str());
    //}
    auto manager = MenuTopicManager::GetSingleton();
    if (manager && manager->dialogueList && !manager->dialogueList->empty() &&
        !manager->dialogueList->front()->responses.empty()) {
        for (auto dialogue : *manager->dialogueList) {
            for (auto response : dialogue->responses) {
                response->text = "NOT FUCK FACE";            
            }        
        }
    }
    if (topicInfoStarting) {
        topicInfoStarting = false;
    }
    return nullptr;
}

int64_t* PopulateResponseText2(TESTopicInfo* a1, int64_t* a2);
REL::Relocation<decltype(PopulateResponseText2)>& GetHookedFunction2() noexcept {
    static REL::Relocation<decltype(PopulateResponseText2)> value(RELOCATION_ID(34413, 35220), 0x80);
    return value;
}
REL::Relocation<decltype(PopulateResponseText2)> OriginalPopulateResponseText2;
int64_t* PopulateResponseText2(TESTopicInfo* a1, int64_t* a2) {
    OriginalPopulateResponseText2(a1, a2);
    logger::info("PopulateResponseText2");
    if (a1 == nullptr) {
        RE::ConsoleLog::GetSingleton()->Print("Calling Populate response text2. No TopicInfo");
    } else {
        RE::ConsoleLog::GetSingleton()->Print(
            fmt::format("Calling Populate response text2. {}", a1->parentTopic->GetFullName()).c_str());
        //if (a2) {
        //    RE::ConsoleLog::GetSingleton()->Print(
        //        fmt::format("Calling Populate response text2. Response {}",
        //                    reinterpret_cast<TESTopicInfo::ResponseData*>(a2)->responseText)
        //            .c_str());
        //}
    }
    return nullptr;
}

int32_t PopulateResponseText3(int64_t a1, int64_t a2);
REL::Relocation<decltype(PopulateResponseText3)>& GetHookedFunction3() noexcept {
    static REL::Relocation<decltype(PopulateResponseText3)> value(RELOCATION_ID(25083, 25626), 0x3B5);
    return value;
}
REL::Relocation<decltype(PopulateResponseText3)> OriginalPopulateResponseText3;
int32_t PopulateResponseText3(int64_t a1, int64_t a2) {
    logger::info("PopulateResponseText3");
    auto ret = OriginalPopulateResponseText3(a1, a2);
     RE::ConsoleLog::GetSingleton()->Print(
         fmt::format("Populate response text. {}", reinterpret_cast<TESTopicInfo::ResponseData*>(a1)->responseText)
         .c_str());
    RE::ConsoleLog::GetSingleton()->Print(fmt::format("Populate response text. pointer value {}", a1).c_str());
    if (topicInfoStarting) {
        reinterpret_cast<TESTopicInfo::ResponseData*>(a1)->responseText = "FUCK FACE TOPIC STARTING";
    }
    //auto ret = 1;
    return ret;
}

void printHeadResponse() {
    REL::Relocation<uint64_t*>F(RELOCATION_ID(515421, 401560), 0x00);
    RE::ConsoleLog::GetSingleton()->Print(fmt::format("Head pointer value {}", *F).c_str());
}

extern int main_llm_loop(int argc, char** argv);


static void fake_print_usage(int argc, char** argv) {
}

struct Hook_DialogueMenu {
    static RE::UI_MESSAGE_RESULTS ProcessMessage(RE::DialogueMenu* a_this, UIMessage& a_message) {
        auto result = _ProcessMessage(a_this, a_message);
        if (a_message.menu == DialogueMenu::MENU_NAME) {
            auto resultToString = [](UI_MESSAGE_RESULTS result) {
                switch (result) {
                    case UI_MESSAGE_RESULTS::kHandled:
                        return "Handled";
                    case UI_MESSAGE_RESULTS::kPassOn:
                        return "PassOn";
                    case UI_MESSAGE_RESULTS::kIgnore:
                        return "Ignore";
                    default:
                        return "Unknown";
                }
            };
            std::size_t count = 0;
            auto manager = MenuTopicManager::GetSingleton();
            if (manager && manager->dialogueList) {
                auto list = *manager->dialogueList;
                for (auto dialogue : list) {
                    count++;
                }
            }
            //printHeadResponse();
            //RE::ConsoleLog::GetSingleton()->Print(
            //    fmt::format("HookedDialogueMenuProcessMessage name {}, type {}, result {}, dialogue size {}",
            //                a_message.menu, std::to_string(a_message.type.underlying()), resultToString(result), count)
            //        .c_str());
        }
        return result;
    }
    static inline REL::Relocation<decltype(ProcessMessage)> _ProcessMessage;

    static RE::BSEventNotifyControl ProcessEvent(RE::DialogueMenu* a_this, const RE::MenuOpenCloseEvent* a_event,
                                                 RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) {
        //RE::ConsoleLog::GetSingleton()->Print(
        //    fmt::format("HookedDialogueMenuProcessEvent name {} opening {}", a_event->menuName, a_event->opening)
        //        .c_str());
        //if (a_event && a_event->menuName == DialogueMenu::MENU_NAME) {
        //    if (a_event->opening) {
        //        RE::ConsoleLog::GetSingleton()->Print("Adding handler");
        //        //MenuControls::GetSingleton()->AddHandler(handler);
        //    } else {
        //        RE::ConsoleLog::GetSingleton()->Print("Removing handler");
        //        //MenuControls::GetSingleton()->RemoveHandler(handler);
        //    }
        //}
        return _ProcessEvent(a_this, a_event, a_eventSource);
    }
    static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;

    static RE::BSEventNotifyControl TopicManagerMenuOpenClose(
        RE::MenuTopicManager* a_this, const RE::MenuOpenCloseEvent* a_event,
        RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) {
        if (a_event && a_event->menuName == DialogueMenu::MENU_NAME) {

        }
        return _TopicManagerMenuOpenClose(a_this, a_event, a_eventSource);
    }

    static inline REL::Relocation<decltype(TopicManagerMenuOpenClose)> _TopicManagerMenuOpenClose;

    static RE::BSEventNotifyControl TopicManagerPlayerRelocation(
        RE::MenuTopicManager* a_this, const RE::PositionPlayerEvent* a_event,
        RE::BSTEventSource<RE::PositionPlayerEvent>* a_eventSource) {
        // RE::ConsoleLog::GetSingleton()->Print(
        //     fmt::format("TopicManagerPlayerRelocation type {}", a_event->type.underlying()).c_str());
        return _TopicManagerPlayerRelocation(a_this, a_event, a_eventSource);
    }
    static inline REL::Relocation<decltype(TopicManagerPlayerRelocation)> _TopicManagerPlayerRelocation;

    static void Install() {
        REL::Relocation<std::uintptr_t> target{RE::VTABLE_DialogueMenu[0]};
        _ProcessMessage = target.write_vfunc(0x4, ProcessMessage);
        REL::Relocation<std::uintptr_t> target_1{RE::VTABLE_DialogueMenu[1]};
        _ProcessEvent = target_1.write_vfunc(0x1, ProcessEvent);

        REL::Relocation<std::uintptr_t> target_2{RE::VTABLE_MenuTopicManager[0]};
        _TopicManagerMenuOpenClose = target_2.write_vfunc(0x1, TopicManagerMenuOpenClose);
        REL::Relocation<std::uintptr_t> target_3{RE::VTABLE_MenuTopicManager[0]};
        _TopicManagerPlayerRelocation = target_3.write_vfunc(0x1, TopicManagerPlayerRelocation);
        
        auto& trampoline = GetTrampoline();
        trampoline.create(128);
        log::trace("Trampoline initialized.");
        OriginalPopulateResponseText =  trampoline.write_call<5>(GetHookedFunction().address(), reinterpret_cast<uintptr_t>(PopulateResponseText));
        OriginalPopulateResponseText1 = trampoline.write_call<5>(GetHookedFunction1().address(), reinterpret_cast<uintptr_t>(PopulateResponseText1));      
        OriginalPopulateResponseText2 = trampoline.write_call<5>(GetHookedFunction2().address(), reinterpret_cast<uintptr_t>(PopulateResponseText2));

        OriginalPopulateResponseText3 = trampoline.write_call<5>(GetHookedFunction3().address(),
                                                            reinterpret_cast<uintptr_t>(PopulateResponseText3));

        // Create the args vector with the command line arguments
        auto args = std::make_shared<std::vector<std::string>>(std::vector<std::string>{
                "G:\\GitHub\\llama.cpp\\build-x64-windows-msvc-release\\bin\\llama-cli.exe",
                "-p",
                "You are a dark elf in service of the Jarl of Whiterun. You will respond with two paragraphs to each line from the player. First one is a single word on how the line makes you feel. Second paragraph is your response",
                "-cnv",
                "-m",
                ".\\meta-llama-3.1-8b-instruct-q4_0.gguf"
        });
        // Convert to C-style argv
        auto argv = std::make_shared<std::vector<char*>>(args->size());
        for (size_t i = 0; i < args->size(); ++i) {
            (*argv)[i] = const_cast<char*>(args->at(i).c_str());
        }
             
        common_params params;

        if (!common_params_parse(argv->size(), argv->data(), params, LLAMA_EXAMPLE_MAIN, fake_print_usage)) {
            std::cerr << "Problems parsing args\n";
        }
        //// Launch the LLM loop in a background thread
        //std::thread llm_thread([args, argv]() {
        //    main_llm_loop(static_cast<int>(argv->size()), argv->data());
        //    });
        //// Detach the thread to run in the background
        //llm_thread.detach();
    }
};

//class DialogueMenuExtended : public DialogueMenu {
//public:
//    DialogueMenuExtended();
//    virtual ~DialogueMenuExtended();
//
//    // Additional members can be added here
//};
//DialogueMenuExtended::DialogueMenuExtended() {
//    // Constructor implementation
//}
//DialogueMenuExtended::~DialogueMenuExtended() {
//    // Destructor implementation
//}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();
    Hook_DialogueMenu::Install();

    //auto& trampoline = SKSE::GetTrampoline();
    //trampoline.create(64);
    //OriginalPopulateResponseText = trampoline.write_call<5>(GetHookedFunction().address(), reinterpret_cast<uintptr_t>(PopulateResponseText));

    // This example prints "Hello, world!" to the Skyrim ~ console.
    // To view it, open the ~ console from the Skyrim Main Menu.
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            RE::ConsoleLog::GetSingleton()->Print("Hello, world!");
            RE::ConsoleLog::GetSingleton()->Print(std::to_string(GetHookedFunction().address()).c_str());
            MenuControls::GetSingleton()->AddHandler(handler);
            logger::info("GetMessagingInterface");
        }
    });

    // Get a reference to our Event Processor singleton
    auto& eventProcessor = EventProcessor::GetSingleton();

    //// Listen for Activate events!
    ////
    //// For more events, I recommend searching CommonLibSSE for files starting with TESEvent
    ////
    //// 1. Visit this link: https://github.com/CharmedBaryon/CommonLibSSE-NG
    //// 2. Press the "T" key to open the fuzzy find file search
    //// 3. Type "TESEvent" (without the " quotes)
    //// 4. Choose any of the files that show up! They're probably events that you can get via an event sink :)
    //RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&eventProcessor);
    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESTopicInfoEvent>(&eventProcessor);
    //// RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::InputEvent>(&eventProcessor);

    //// Note: a few UI events are registered via RE::UI instead of via RE::ScriptEventSourceHolder
    ////- MenuOpenCloseEvent
    ////- MenuModeChangeEvent>
    ////- MenuModeCounterChangedEvent (use <void*> for template and cast)
    ////- TutorialEvent               (use <void*> for template and cast)
    //RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(&eventProcessor);
    //RE::UI::GetSingleton()->AddEventSink<RE::MenuModeChangeEvent>(&eventProcessor);

    return true;
}
