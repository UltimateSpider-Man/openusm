#include "console.h"

#include "color32.h"
#include "common.h"
#include "consolecmds.h"
#include "consolevars.h"
#include "debug_menu.h"
#include "fetext.h"
#include "input_mgr.h"
#include "log.h"
#include "memory.h"
#include "game.h"
#include "ngl.h"
#include "os_developer_options.h"
#include "os_file.h"
#include "panelquad.h"
#include "tokenizer.h"
#include "trace.h"
#include "variables.h"
#include "vector2d.h"
#include "vtbl.h"

#include <vector.hpp>

std::stack<tokenizer *> s_exec_tok_stack{};

float s_exec_tick{0};

VALIDATE_SIZE(Console, 0x254);
VALIDATE_OFFSET(Console, current, 0x105);

Console *g_console = nullptr;

static void (*kbevcb)(KeyEvent, Key_Axes, void *) = nullptr;

static void *kbevudata = nullptr;

static void (*kbchcb)(char, void *) = nullptr;

static void *kbchudata = nullptr;

char KB_register_char_callback(void (*a1)(char, void *), void *a2) {
    kbchcb = a1;
    kbchudata = a2;
    return 1;
}

char KB_register_event_callback(void (*a1)(KeyEvent, Key_Axes, void *), void *a2) {
    kbevcb = a1;
    kbevudata = a2;
    return 1;
}

void _kbevcb(KeyEvent a1, Key_Axes a2) {
    if (kbevcb != nullptr) {
        kbevcb(a1, a2, kbevudata);
    }
}

void _kbchcb(char a1) {
    if (kbchcb != nullptr) {
        kbchcb(a1, kbchudata);
    }
}

void console_event_callback(KeyEvent a1, Key_Axes a2, void *a3) {
    if (g_console != nullptr) {
        g_console->handle_event(a1, a2, a3);
    }
}

void console_char_callback(char a1, void *a2) {
    if (g_console != nullptr) {
        g_console->handle_char(a1, a2);
    }
}

Console::Console()
{
    TRACE("Console::Console");

    this->m_visible = false;
    this->oldCurrent[0] = '\0';
    this->current[0] = '\0';

    this->field_220 = false;
    this->m_height = 240.0;

    this->field_224 = 0.0;
    this->lineNumber = 0;
    this->field_230 = 16;
    this->field_234 = 16;
    this->cmdLogNumber = 0;
    this->field_24E = true;

    auto *mem = mem_alloc(sizeof(PanelQuad));
    this->field_248 = new (mem) PanelQuad{};

    vector2d v8[4];
    v8[0] = {0.0, 0.0};
    v8[1] = {640.0, 0.0};
    v8[2] = {0.0, this->m_height};
    v8[3] = {640.0, this->m_height};

    color32 v4[4];
    v4[0] = {0, 0, 0, 255};
    v4[1] = {0, 0, 0, 255};
    v4[2] = {0, 0, 0, 255};
    v4[3] = {0, 0, 0, 255};

    Var<char[3]> Source{0x00871BD1};

    this->field_248->Init(v8, v4, static_cast<panel_layer>(0), 10.0f, Source());

    this->field_24C = false;
    this->field_24D = false;

    this->field_23C = mString{"console_log.txt"};

    this->setHeight(200.0);
    this->hide();
    KB_register_event_callback(console_event_callback, nullptr);
    KB_register_char_callback(console_char_callback, nullptr);
}

Console::~Console()
{
    sp_log("Console::~Console()");

    if (field_248 != nullptr) {
        void (__fastcall *finalize)(void *, void *, bool) = CAST(finalize, get_vfunc(field_248->m_vtbl, 0x8));
        finalize(field_248, nullptr, true);
    }
}

void * Console::operator new(size_t size)
{
    auto *mem = mem_alloc(size);
    return mem;
}

void Console::operator delete(void *ptr, size_t size)
{
    mem_dealloc(ptr, size);
}

void Console::addToCommandLog(const char *a1) {
    this->m_command_log.push_front(mString{a1});

    while (this->m_command_log.size() > this->field_234) {

        this->m_command_log.pop_back();
    }

    this->cmdLogNumber = 0;
}

bool Console::StrnCopy(const char *src, char *dest, int *a3) {
    if (src == nullptr) {
        return false;
    }

    while (src[*a3] == ' ') {
        ++*a3;
    }

    auto v6 = *a3;
    while (src[*a3] != ' ' && src[*a3]) {
        ++*a3;
    }

    auto v5 = *a3;
    if (v6 == *a3 || !dest) {
        return false;
    }

    strncpy(dest, &src[v6], v5 - v6);
    dest[v5 - v6] = '\0';
    while (src[*a3] == ' ') {
        ++*a3;
    }

    return true;
}

ConsoleCommand *Console::getCommand(const std::string &a1)
{
    if (g_console_cmds == nullptr) {
        return nullptr;
    }

    auto &cmds = (*g_console_cmds);

    auto it = std::find_if(cmds.begin(), cmds.end(), [&a1](auto &cmd) -> bool {
        return cmd->match(a1);
    });

    return (it != cmds.end() ? (*it) : nullptr);
}

ConsoleVariable *Console::getVariable(const std::string &a1)
{
    if (g_console_vars == nullptr) {
        return nullptr;
    }

    auto &vars = (*g_console_vars);
    auto it = std::find_if(vars.begin(), vars.end(), [&a1](auto &var) -> bool {
        return var->match(a1);
    });

    return (it != vars.end() ? (*it) : nullptr);
}

void Console::addToLog(const char *arg4, ...) {
    va_list va;

    va_start(va, arg4);

    if (arg4 != nullptr) {
        char v16[4096]{};

        vsprintf(v16, arg4, va);

        char *v14 = nullptr;

        char *a2 = v16;
        for (;; a2 = v14 + 1) {
            v14 = strchr(a2, '\n');
            if (v14 == nullptr) {
                break;
            }

            char a1[1024]{};

            const auto a3 = v14 - a2;
            strncpy(a1, a2, a3);
            a1[a3] = '\0';

            mString v2{a1};
            this->m_log.push_front(v2);
        }

        mString v3{a2};

        this->m_log.push_front(v3);

        while (this->m_log.size() > this->field_230) {
            this->m_log.pop_back();
        }

        mString v4{v16};
    }

    va_end(va);
}

void Console::processCommand(const char *a2, bool is_log) {
    if (a2 != nullptr && strlen(a2)) {
        if (is_log) {
            this->addToCommandLog(a2);
        }

        std::vector<std::string> v17{};

        char a1[256]{};

        auto a3a = 0;
        this->StrnCopy(a2, a1, &a3a);
        mString v11{a1};
        v11.to_lower();

        auto *v10 = this->getCommand(v11.c_str());
        if (v10 != nullptr)
        {
            while (this->StrnCopy(a2, a1, &a3a)) {
                std::string v5{a1};

                v17.push_back(v5);
            }

            if (is_log) {
                this->addToLog(a2);
            }

            if (!v10->process_cmd(v17))
            {
                if (is_log) {
                    this->addToLog("??? %s", a2);
                }
            }
        } else if (this->getVariable(v11.c_str())) {
            v17.push_back(v11.c_str());

            while (this->StrnCopy(a2, a1, &a3a)) {
                std::string v4 {a1};

                v17.push_back(v4);
            }

            if (is_log) {
                this->addToLog(a2);
            }

            if (v17.size() <= 1) {
                std::string v8{"get"};

                v10 = this->getCommand(v8);

            } else {
                std::string v7{"set"};

                v10 = this->getCommand(v7);
            }

            v10->process_cmd(v17);
        } else if (is_log) {
            this->addToLog("??? %s", a2);
        }
    }
}

bool Console::isVisible() const {
    return this->m_visible;
}

void Console::setHeight(Float a2) {
    this->m_height = a2;

    this->field_248->SetPos(Float{0.0}, Float{0.0}, Float{640.0}, a2);
}

void Console::handle_char(char a2, void *)
{
    if (this->m_visible && a2 >= ' ' && a2 != 127 && a2 != '`' && a2 != '~') {

        if (a2 == '[')
        {
            auto v3 = strlen(this->current);
            if (v3 > 0)
            {
                this->current[v3 - 1] = '\0';
            }

            strcpy(this->oldCurrent, this->current);
            return;
        }

        auto v3 = strlen(this->current);
        this->current[v3] = a2;
        this->current[v3 + 1] = '\0';
        strcpy(this->oldCurrent, this->current);
    }
}

void Console::hide() {
    sp_log("Console::hide");
    this->field_248->TurnOn(false);

    this->m_visible = false;
    color32 v1{100, 100, 100, 100};

    this->field_248->SetColor(v1);

#if 0
    v2 = sub_668F07();
    sub_69EDB4(v2);
#else
    input_mgr::instance->field_20 &= 0xFFFFFFFE;
#endif

    g_game_ptr->unpause();
}

float Console::getHeight() {
    return this->m_height;
}

void Console::exec_frame_advance(Float a2) {
    if (!s_exec_tok_stack.empty()) {
        s_exec_tick += a2;

        if (s_exec_tick >= os_developer_options::instance->get_int(mString{"EXEC_DELAY"}))
        {
            s_exec_tick = 0.0;
            auto &v4 = s_exec_tok_stack.top();
            auto *v3 = v4->get_token();
            if (v3 != nullptr) {
                this->processCommand(v3, false);
            } else {
                auto &v2 = s_exec_tok_stack.top();
                if (v2 != nullptr) {
                    delete v2;
                }

                s_exec_tok_stack.pop();
            }
        }
    }
}

void Console::exec(const mString &a2) {
    os_file v10{};

    v10.open(a2, 1);
    if (v10.is_open()) {
        auto file_size = v10.get_size();
        auto *data = new char[file_size + 1];

        v10.read(data, file_size);
        data[file_size] = '\0';

        v10.close();

        auto *mem = mem_alloc(sizeof(tokenizer));
        auto *v6 = new (mem) tokenizer{true};

        v6->parse(data, "\n\r");
        v6->setup_current_iterator();
        s_exec_tok_stack.push(v6);

        auto *v4 = a2.c_str();
        this->addToLog("Executing '%s'", v4);
    } else {
        auto *v5 = a2.c_str();
        this->addToLog("File '%s' not found", v5);
    }
}

void Console::frame_advance(Float a2) {
    this->field_224 += a2;
    if (this->field_24E && this->field_224 >= 0.5) {
        this->field_220 = !this->field_220;
        this->field_224 = 0.0;
    }

    this->exec_frame_advance(a2);
}

void Console::getMatchingCmds(const char *a2, std::list<mString> &cmds) {
    cmds.clear();
    const auto a3a = strlen(a2);

    if (g_console_cmds != nullptr)
    {
        for (auto &cmd : (*g_console_cmds))
        {
            if (cmd != nullptr)
            {
                if (strncmp(cmd->field_4, a2, a3a) == 0)
                {
                    mString v12{cmd->field_4};

                    cmds.push_back(v12);
                }
            }
        }
    }

    if (g_console_vars != nullptr)
    {
        auto func = [&a2, &a3a](auto &var) -> bool {
            return (var != nullptr
                    && strncmp(var->field_4, a2, a3a) == 0);
        };

        for (auto &var : (*g_console_vars))
        {
            if ( func(var) ) {
                cmds.push_back(mString {var->field_4});
            }
        }
    }
}

void Console::partialCompleteCmd(char *a1, std::list<mString> &list) {
    int a3 = -1;
    for (auto it = list.begin(); it != list.end(); ) {

        mString &v20 = (*it);

        ++it;

        if (it != list.end()) {
            mString &v19 = (*it);

            auto *v18 = v20.c_str();
            auto *v17 = v19.c_str();

            int i;
            for (i = 0; v18[i] == v17[i] && v18[i] && v17[i]; ++i) {
                ;
            }

            if (i < a3 || a3 == -1) {
                a3 = i;
            }
        }
    }

    if (a3 > 0) {
        int v7 = strlen(a1);
        if (a3 > v7) {
            auto v11 = a3;
            auto &v9 = list.front();
            auto *v10 = v9.c_str();
            strncpy(a1, v10, v11);
            a1[a3] = '\0';
        }
    }
}

void Console::handle_event(KeyEvent a2, Key_Axes a3, [[maybe_unused]] void *a4) {
    if (a2 == 1) {
        switch (a3) {
        case KB_RETURN:
            if (this->m_visible) {
                this->processCommand(this->current, false);
                this->current[0] = '\0';
                strcpy(this->oldCurrent, this->current);
            }

            break;
        case KB_BACKSPACE:
            if (this->m_visible) {
                auto len = strlen(this->current);
                if (len > 0) {
                    this->current[len - 1] = '\0'; 
                }

                strcpy(this->oldCurrent, this->current);
            } /*else if (debug_menu::active_menu != nullptr) {
                debug_menu::hide();
            } else {
                debug_menu::show();
            }*/
            break;
        case KB_TAB:
            if (this->m_visible && strlen(this->current)) {
                std::list<mString> v28{};

                this->getMatchingCmds(this->current, v28);
                if (v28.size()) {
                    if (v28.size() == 1) {
                        auto &v13 = v28.front();
                        auto *v14 = v13.c_str();
                        strcpy(this->current, v14);
                        strcat(this->current, " ");
                        strcpy(this->oldCurrent, this->current);
                    } else {
                        this->partialCompleteCmd(this->current, v28);
                        strcpy(this->oldCurrent, this->current);
                        this->addToLog((const char *) this);

                        this->addToLog((const char *) this);

                        for (auto &v16 : v28) {
                            auto *v17 = v16.c_str();
                            this->addToLog("%s", v17);
                        }
                    }

                    while (v28.size()) {
                        v28.pop_back();
                    }

                } else {
                    this->addToLog((const char *) this);
                    this->addToLog((const char *) this);
                }
            }
            break;
        case KB_TILDE:
            if (this->isVisible()) {
                this->hide();
            } else {
                this->show();
            }

            break;
        case KB_HOME:
            if (this->m_visible) {
                this->lineNumber = this->m_log.size() - 1;
            }

            break;
        case KB_END:
            if (this->m_visible) {
                this->lineNumber = 0;
            }

            break;
        case KB_PAGEUP:
            if (this->m_visible && this->lineNumber < this->m_log.size() - 1) {
                ++this->lineNumber;
            }

            break;
        case KB_PAGEDOWN: {
            if (this->m_visible && this->lineNumber > 0) {
                --this->lineNumber;
            }

            break;
        }
        case KB_DOWN:
            if (this->m_visible && this->m_command_log.size()) {
                --this->cmdLogNumber;
                if (this->cmdLogNumber > 0) {
                    auto it = this->m_command_log.begin();
                    auto end = this->m_command_log.end();

                    for (auto i = this->cmdLogNumber - 1; i > 0; --i) {
                        if (it == end) {
                            break;
                        }

                        ++it;
                    }

                    if (it != end) {
                        strcpy(this->current, it->c_str());
                    }

                } else {
                    this->cmdLogNumber = 0;
                    strcpy(this->current, this->oldCurrent);
                }
            }

            break;
        case KB_UP:
            if (this->m_visible && this->m_command_log.size()) {
                ++this->cmdLogNumber;
                while (this->cmdLogNumber > 0 && this->cmdLogNumber > this->m_command_log.size()) {
                    --this->cmdLogNumber;
                }

                if (this->cmdLogNumber > 0 && this->cmdLogNumber <= this->m_command_log.size()) {
                    auto it = this->m_command_log.begin();
                    auto end = this->m_command_log.end();
                    for (auto j = this->cmdLogNumber - 1; j > 0; --j) {
                        if (it == end) {
                            break;
                        }

                        ++it;
                    }

                    if (it != end) {
                        auto *v7 = it->c_str();
                        strcpy(this->current, v7);
                    }
                }
            }

            break;
        default:
            return;
        }
    }
}

void Console::show()
{
    TRACE("Console::show");
    this->field_248->TurnOn(true);

    this->m_visible = true;
    color32 v1 {100, 100, 100, 100};

    this->field_248->SetColor(v1);

#if 0
    v2 = sub_668F07();
    sub_69EDB4(v2);
#else
    input_mgr::instance->field_20 |= 1u;
#endif

    g_game_ptr->pause();
}

void Console::setRenderCursor(bool a2) {
    this->field_24E = a2;
}


void Console::render()
{
    TRACE("Console::render");

    const color32 font_color{255, 255, 255, 255};
    if (this->m_visible)
    {
        this->field_248->Draw();

        auto *font = g_femanager.GetFont(static_cast<font_index>(0));

        uint32_t v26, v25;
        nglGetStringDimensions(font, &v26, &v25, "M");
        auto v24 = (float) v25;
        auto v23 = this->m_height - 20.0;

        const char *v11;
        if (this->field_220) {
            v11 = "_";
        } else {
            v11 = "";
        }

        auto v9 = mString{"> "} + this->current;

        auto v8 = v9 + v11;

        mString v22 = v8;

        vector2di v2{10, (int) v23};
        render_console_text(v22, v2, font_color);
        v23 = v23 - v24;
        if (this->lineNumber > 0)
        {
            mString v17{"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"};

            auto v3 = vector2di{10, (int) v23};
            render_console_text(v17, v3, font_color);

            v23 = v23 - v24;
        }

        auto it = this->m_log.begin();
        for (auto i = 0u;; ++i) {
            auto end = this->m_log.end();
            if (it == end || v23 <= 0.0) {
                break;
            }

            if (i >= this->lineNumber) {
                auto &v5 = *it;
                if (v5.size() > 0) {
                    auto v7 = vector2di{10, (int) v23};

                    render_console_text(v5, v7, font_color);
                }

                v23 = v23 - v24;
            }

            ++it;
        }
    }
}

void render_console_text(const mString &a1, vector2di a2, const color32 &a4)
{
    FEText v7 {static_cast<font_index>(0),
              static_cast<global_text_enum>(0),
              (float) a2.x,
              (float) a2.y,
              1,
              static_cast<panel_layer>(0),
              1.0,
              16,
              0,
              a4};

    auto *v3 = a1.c_str();

    mString v4{v3};

    v7.field_1C = v4;

    v7.Draw();
}

void terrain_types_manager_create_inst()
{
    CDECL_CALL(0x005C54B0);

    g_console = new Console {};
}

void terrain_types_manager_delete_inst()
{
    CDECL_CALL(0x005BA680);

    delete g_console;
}

void __fastcall FEManager_Update(void *self, void *edx, Float a2)
{
    void (__fastcall *func)(void *, void *edx, Float) = CAST(func, 0x00642B30);
    func(self, edx, a2);

    {
        g_console->frame_advance(a2);
    }
}

void hook_nglListEndScene()
{
    g_console->render();

    CDECL_CALL(0x0076A030);
}

void console_patch()
{
    REDIRECT(0x0052B5D7, hook_nglListEndScene);

    REDIRECT(0x00552E75, terrain_types_manager_create_inst);

    REDIRECT(0x00524155, terrain_types_manager_delete_inst);

    {
        REDIRECT(0x00558289, FEManager_Update);
        REDIRECT(0x0055A102, FEManager_Update);
    }
}

