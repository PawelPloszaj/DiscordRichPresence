// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <Windows.h>
#include "../Dependencies/discord/include/discord_register.h"
#include "../Dependencies/discord/include/discord_rpc.h"
#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void Initialize(std::string client_id) {
    DiscordEventHandlers Handler;
    memset(&Handler, 0, sizeof(Handler));
    Discord_Initialize(client_id.c_str(), &Handler, TRUE, NULL);
}

void Update(std::string state, std::string details, std::string LargeImageKey, std::string SmallImageKey, std::string LargeImageText, std::string SmallImageText, long long time,bool enable_firstbtn, bool enable_secondbtn, std::string Button1Label,std::string Button1Url,std::string Button2Label,std::string Button2Url) {
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = state.c_str();
    discordPresence.details = details.c_str();
    discordPresence.startTimestamp = /*time(0)*/ time;
    discordPresence.largeImageKey = LargeImageKey.c_str();
    discordPresence.smallImageKey = SmallImageKey.c_str();
    discordPresence.largeImageText = LargeImageText.c_str();
    discordPresence.smallImageText = SmallImageText.c_str();
    discordPresence.enable_firstbtn = enable_firstbtn;
    discordPresence.enable_secondbtn = enable_secondbtn;
    discordPresence.Button1Label = Button1Label.c_str();
    discordPresence.Button1Url = Button1Url.c_str();
    discordPresence.Button2Label = Button2Label.c_str();
    discordPresence.Button2Url = Button2Url.c_str();
    Discord_UpdatePresence(&discordPresence);

}

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void Shutdown()
{
    Discord_Shutdown();
}

std::string title_spotify;
#include <Psapi.h>
#include <chrono>
#include <thread>

void spotify() {
    const static auto& spotify = true; //xD

    if (!spotify)
        return;

    static HWND spotify_hwnd = nullptr;
    static int last_hwnd_time = 0, last_title_time = 0;
    static int sc = 0;
    sc++;
    /* hilariously overengineered solution to get fucking spotify song name */
    if ((!spotify_hwnd || spotify_hwnd == INVALID_HANDLE_VALUE) && last_hwnd_time <= sc) {
        /* game freezes if we try to open a handle 300 times per second (understandable) so this workaround will do */

        last_hwnd_time = sc + 10;
        for (HWND hwnd = GetTopWindow(0); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
            if (!(IsWindowVisible)(hwnd))
                continue;

            int length = (GetWindowTextLengthW)(hwnd);
            if (length == 0)
                continue;

            WCHAR filename[300];
            DWORD pid;
            (GetWindowThreadProcessId)(hwnd, &pid);

            const auto spotify_handle = (OpenProcess)(PROCESS_QUERY_INFORMATION, FALSE, pid);
            (K32GetModuleFileNameExW)(spotify_handle, nullptr, filename, 300);

            std::wstring sane_filename{ filename };

            (CloseHandle)(spotify_handle);

            /* fucking finally found spotify */
            if (sane_filename.find((L"Spotify.exe")) != std::string::npos)
                spotify_hwnd = hwnd;
        }
    }
    else if (spotify_hwnd && spotify_hwnd != INVALID_HANDLE_VALUE && last_title_time < sc) {
        last_title_time = sc + 5;
        WCHAR title[300];

        /* returns 0 if something went wrong, so try to get handle again */
        if (!(GetWindowTextW)(spotify_hwnd, title, 300)) {
            spotify_hwnd = nullptr;
            title_spotify = "";
        }
        else {
            const std::wstring sane_title{ title };
            const std::string ascii_title{ sane_title.begin() , sane_title.end() };

            static std::uint32_t hash = 0;

            /* if we find a dash, song is likely playing */
            if (sane_title.find((L"-")) != std::string::npos) {
                title_spotify = ascii_title;
            }
            else title_spotify = "";
        }
    }
}

inline bool exists_test(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(480, 300, "Discord Rich Presence by Pafffcio#0733", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glewInit();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // 
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial-unicode-ms.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static long long jd = 0;
    static bool do_once = true;
    static bool one_time = true;
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        std::chrono::system_clock::duration dtn = tp.time_since_epoch();
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
        bool open = true;
        static char buf_client[50] = { 0 };
        static char buf_state[300] = { 0 };
        static char buf_details[300] = { 0 };
        static char buf_largeimgkey[300] = { 0 };
        static char buf_smallimgkey[300] = { 0 };
        static char buf_largeimgtext[300] = { 0 };
        static char buf_smallimgtext[300] = { 0 };
        static bool spotif;
        static bool enable_firstbtn;
        static bool enable_secondbtn;
        static char buf_button1label[50] = { 0 };
        static char buf_button1url[300] = { 0 };
        static char buf_button2label[50] = { 0 };
        static char buf_button2url[300] = { 0 };
        auto tm = dtn.count() * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
        if (do_once) 
        {
            HideConsole();
            if (exists_test("config.ini")) {
                FILE* stream;
                fopen_s(&stream, "config.ini", "r+");
                fseek(stream, 0, SEEK_SET);
                fread_s(&buf_client, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_state, sizeof(char[300]), sizeof(char[50]), 1, stream);
                fread_s(&buf_details, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_largeimgkey, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_smallimgkey, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_largeimgtext, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_smallimgtext, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&spotif, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&enable_firstbtn, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&enable_secondbtn, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&buf_button1label, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_button1url, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_button2label, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_button2url, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fclose(stream);
                Initialize(buf_client);
                Update(buf_state, buf_details, buf_largeimgkey, buf_smallimgkey, buf_largeimgtext, buf_smallimgtext, tm, enable_firstbtn, enable_secondbtn, buf_button1label, buf_button1url, buf_button2label, buf_button2url);
            }
            do_once = false;
        }

        ImGui::Begin("Discord Rich Presence",&open, ImVec2(417,238),ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
        {
            //static char buf_client[50] = { 0 };
            ImGui::InputText("Client ID", buf_client, IM_ARRAYSIZE(buf_client));
            //static char buf_state[300] = { 0 };
            ImGui::InputText("State", buf_state, IM_ARRAYSIZE(buf_state));
            //static char buf_details[300] = { 0 };
            ImGui::InputText("Details", buf_details, IM_ARRAYSIZE(buf_details));
            //static char buf_largeimgkey[300] = { 0 };
            ImGui::InputText("Large Image Key", buf_largeimgkey, IM_ARRAYSIZE(buf_largeimgkey));
            //static char buf_smallimgkey[300] = { 0 };
            ImGui::InputText("Small Image Key", buf_smallimgkey, IM_ARRAYSIZE(buf_smallimgkey));
            //static char buf_largeimgtext[300] = { 0 };
            ImGui::InputText("Large Image Text", buf_largeimgtext, IM_ARRAYSIZE(buf_largeimgtext));
            //static char buf_smallimgtext[300] = { 0 };
            ImGui::InputText("Small Image Text", buf_smallimgtext, IM_ARRAYSIZE(buf_smallimgtext));
            ImGui::Checkbox("Spotify State", &spotif);
            ImGui::SameLine();
            ImGui::Checkbox("Enable First Button", &enable_firstbtn);
            ImGui::SameLine();
            ImGui::Checkbox("Enable Second Button", &enable_secondbtn);
            if (enable_firstbtn)
            {
                ImGui::InputText("First Button Label", buf_button1label, IM_ARRAYSIZE(buf_button1label));
                ImGui::InputText("First Button Url", buf_button1url, IM_ARRAYSIZE(buf_button1url));
            }
            if (enable_secondbtn)
            {
                ImGui::InputText("Second Button Label", buf_button2label, IM_ARRAYSIZE(buf_button2label));
                ImGui::InputText("Second Button Url", buf_button2url, IM_ARRAYSIZE(buf_button2url));
            }
            if (spotif)
                jd++;

            if (spotif && jd == 60) {
                spotify();
                std::string text;
                if (title_spotify.empty()) {
                    text = "";
                    if (one_time) {
                        Update(buf_state, buf_details, buf_largeimgkey, buf_smallimgkey, buf_largeimgtext, buf_smallimgtext, tm, enable_firstbtn, enable_secondbtn, buf_button1label, buf_button1url, buf_button2label, buf_button2url);
                        one_time = false;
                    }
                }
                else { 
                text = "Now listening: \n";
                std::string txt = text + title_spotify;
                Update(txt.c_str(), buf_details, buf_largeimgkey, buf_smallimgkey, buf_largeimgtext, buf_smallimgtext, tm, enable_firstbtn, enable_secondbtn, buf_button1label, buf_button1url, buf_button2label, buf_button2url);
                one_time = true;
                }
                jd = 0;
            }

            if (ImGui::Button("Update Discord Rich Presence")) {
                Initialize(buf_client);
                Update(buf_state, buf_details, buf_largeimgkey, buf_smallimgkey, buf_largeimgtext, buf_smallimgtext, tm, enable_firstbtn, enable_secondbtn, buf_button1label, buf_button1url, buf_button2label, buf_button2url);
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Config")) {
                FILE* stream;
                fopen_s(&stream, "config.ini", "r+");
                fseek(stream, 0, SEEK_SET);
                fread_s(&buf_client, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_state, sizeof(char[300]), sizeof(char[50]), 1, stream);
                fread_s(&buf_details, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_largeimgkey, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_smallimgkey, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_largeimgtext, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_smallimgtext, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&spotif, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&enable_firstbtn, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&enable_secondbtn, sizeof(bool), sizeof(bool), 1, stream);
                fread_s(&buf_button1label, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_button1url, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fread_s(&buf_button2label, sizeof(char[50]), sizeof(char[50]), 1, stream);
                fread_s(&buf_button2url, sizeof(char[300]), sizeof(char[300]), 1, stream);
                fclose(stream);
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Config")) {
                FILE* p_stream;
                fopen_s(&p_stream, "config.ini", "w+");
                fseek(p_stream, 0, SEEK_SET);
                fwrite(&buf_client, sizeof(char[50]), 1, p_stream);
                fwrite(&buf_state, sizeof(char[50]), 1, p_stream);
                fwrite(&buf_details, sizeof(char[300]), 1, p_stream);
                fwrite(&buf_largeimgkey, sizeof(char[300]), 1, p_stream);
                fwrite(&buf_smallimgkey, sizeof(char[300]), 1, p_stream);
                fwrite(&buf_largeimgtext, sizeof(char[300]), 1, p_stream);
                fwrite(&buf_smallimgtext, sizeof(char[300]), 1, p_stream);
                fwrite(&spotif, sizeof(bool), 1, p_stream);
                fwrite(&enable_firstbtn, sizeof(bool), 1, p_stream);
                fwrite(&enable_secondbtn, sizeof(bool), 1, p_stream);
                fwrite(&buf_button1label, sizeof(char[50]), 1, p_stream);
                fwrite(&buf_button1url, sizeof(char[300]), 1, p_stream);
                fwrite(&buf_button2label, sizeof(char[50]), 1, p_stream);
                fwrite(&buf_button2url, sizeof(char[300]), 1, p_stream);
                fclose(p_stream);
            }
        }
        ImGui::End();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    Shutdown();
    return 0;
}
