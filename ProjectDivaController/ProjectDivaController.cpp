// ProjectDivaController.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8")
#define BOOST_SYSTEM_USE_UTF8
// Store the current warning state and set a minimal warning level (level 0 is lowest)
#pragma warning(push, 0)
#pragma warning(disable : 6385 6388 26439 26495)
// Warning: I modified boost_1_91_0\boost\beast\websocket\stream.hpp, 
// setting boost::beast::websocket::stream.impl_; to public.
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/flat_map.hpp>
// Restore the original warning state
#pragma warning(pop)
#include "HelperFunctionAndClass.h"
#include "resource.h"

#include<Windows.h>
#include<fstream>
#include <thread>
#include <iostream>
#include <print>
#include <span>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

#ifdef _DEBUG
bool g_output_received_message = true;
bool g_output_keyboard_operation = true;
#else
bool g_output_received_message = false;
bool g_output_keyboard_operation = false;
#endif // DEBUG

std::array<BYTE, 8> vk_button{
    'I',
    'J',
    'K',
    'L',
    'W',
    'A',
    'S',
    'D',
};
auto vk_stick = [vk_s = std::array<BYTE, 5>{ 'Q', 'U', '\0', 'E', 'O' }]
(int stick) mutable ->BYTE& {
    return vk_s.at(stick + 2);
    };
static void ReadAndPrintSettings() {
    std::ifstream file("ProjectDivaControllerSettings.txt");

    if (file.is_open()) {

        std::string str;
        auto SetVk = [&str](BYTE& value) ->bool {
            if (str.size() >= 1 && (str[1] == '\0' || isspace(str[1]))) {
                if (isupper(str[0])) {
                    value = str[0];
                    return 0;
                }
                else {
                    return -1;
                }
            }
            else {
                int vk = atoi(str.c_str());
                if (vk <= 0 || vk > 255) {
                    return -1;
                }
                else {
                    value = vk;
                    return 0;
                }
            }
            };
        bool error = false;
        for (int i = 0; i < 8 && std::getline(file, str); ++i)
        {
            if (SetVk(vk_button[i])) {
                error = true;
                goto err;
            }
        }
        //這裡使用了or的短路求值
        error = !std::getline(file, str) || SetVk(vk_stick(-1))
            || !std::getline(file, str) || SetVk(vk_stick(1))
            || !std::getline(file, str) || SetVk(vk_stick(-2))
            || !std::getline(file, str) || SetVk(vk_stick(2));
        //以及comma , operator
        std::getline(file, str)
            && ((g_output_received_message = atoi(str.c_str())), std::getline(file, str))
            && ((g_output_keyboard_operation = atoi(str.c_str())), std::getline(file, str));
        if (error) {
        err:
            printError("The \"ProjectDivaControllerSettings.txt\" file does not contain enough settings or format incorrect; the rest will use default values.");
        }
    }
    else {
        printError("Can't open \"ProjectDivaControllerSettings.txt\"\n"
            "using default settings\n");
    }
    std::print("Settings:\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{} : {}\n"
        "{}\n{} {}\t{} {}\n"
        "output_received_message : {}\n"
        "output_keyboard_operation : {}\n"
        , "△", vkToString(vk_button[0])
        , "□", vkToString(vk_button[1])
        , "×", vkToString(vk_button[2])
        , "◯", vkToString(vk_button[3])
        , "🡅", vkToString(vk_button[4])
        , "🡄", vkToString(vk_button[5])
        , "🡇", vkToString(vk_button[6])
        , "🡆", vkToString(vk_button[7])
        , "↼ ⇀\t↼ ⇀"
        , vkToString(vk_stick(-1)), vkToString(vk_stick(1)), vkToString(vk_stick(-2)), vkToString(vk_stick(2))
        , g_output_received_message
        , g_output_keyboard_operation
    );
}
//負責模擬按下按鍵並管理按鍵狀態
class Controller {
    struct {
        std::array<bool, 8> buttons{};
        std::array<int16_t, 2> sticks{};
        std::array<bool, 8> pendingLaterUp{};
        std::array<bool, 2> pendingStickLaterUp{};
        std::array<std::chrono::nanoseconds, 8> button_downTime{};
        std::array<std::chrono::nanoseconds, 2> stick_downTime{};
    } keybd_state{};

    // MM+ polls the keyboard state once per frame, instead of through keyboard messages.
    // To prevent input from being lost, ensure that each keystroke is maintained for at least one frame.
    static constexpr std::chrono::nanoseconds min_keepdown_time = std::chrono::nanoseconds(16'600'000);
public:
    void ButtonDown(BYTE index) {
        if (keybd_state.pendingLaterUp.at(index)) [[unlikely]] {
            keybd_state.buttons.at(index) = false;
            SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
            SendKeybdInput(vk_button.at(index), KEYEVENTF_KEYUP);
            SetConsoleColor();
            keybd_state.pendingLaterUp[index] = false;
            Sleep(1);
        }
        keybd_state.buttons.at(index) = true;
        SendKeybdInput(vk_button.at(index));
        keybd_state.button_downTime.at(index) = time_since_epoch();
    }
    void ButtonUp(BYTE index) {
        std::chrono::nanoseconds now = time_since_epoch();
        if ((now - keybd_state.button_downTime.at(index)) < min_keepdown_time) {
            keybd_state.pendingLaterUp.at(index) = true;
            return;
        }
        keybd_state.buttons.at(index) = false;
        SendKeybdInput(vk_button.at(index), KEYEVENTF_KEYUP);
    }
    void StickDown(char index) {
        size_t i = std::abs(index) - 1;
        if (keybd_state.pendingStickLaterUp.at(i)) {
            SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
            SendKeybdInput(vk_stick(keybd_state.sticks.at(i)), KEYEVENTF_KEYUP);
            SetConsoleColor();
            keybd_state.pendingStickLaterUp[i] = false;
        }
        keybd_state.sticks.at(i) = index;
        SendKeybdInput(vk_stick(index));
        keybd_state.stick_downTime.at(i) = time_since_epoch();
    }
    void StickUp(char index) {
        size_t i = std::abs(index) - 1;
        std::chrono::nanoseconds now = time_since_epoch();
        if ((now - keybd_state.stick_downTime.at(i)) < min_keepdown_time) {
            keybd_state.pendingStickLaterUp.at(i) = true;
            return;
        }
        keybd_state.sticks.at(i) = 0;
        SendKeybdInput(vk_stick(index), KEYEVENTF_KEYUP);
    }
    void SendKeybdInput(BYTE vk_code, DWORD Flags = NULL) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk_code;
        input.ki.dwFlags = Flags;
        SendInput(1, &input, sizeof(INPUT));
        if (g_output_keyboard_operation) {
            std::print("{} [{}]\n"
                "keybd_state:\n"
                "{:d} {:d} {:d} {:d}\n"
                "{:d} {:d} {:d} {:d}\n"
                "[{}{}]\n", vkToString(vk_code), Flags ? "UP" : "DOWN",
                keybd_state.buttons[0],
                keybd_state.buttons[1],
                keybd_state.buttons[2],
                keybd_state.buttons[3],
                keybd_state.buttons[4],
                keybd_state.buttons[5],
                keybd_state.buttons[6],
                keybd_state.buttons[7],
                keybd_state.sticks[0] == 0 ? " • " : keybd_state.sticks[0] > 0 ? " •>" : "<• ",
                keybd_state.sticks[1] == 0 ? " • " : keybd_state.sticks[1] > 0 ? " •>" : "<• "
            );
        }
    }

    int FlushLaterUp() {
        std::chrono::nanoseconds now = time_since_epoch();
        long long stillPending = 0;
        for (size_t idx = 0; idx < keybd_state.pendingLaterUp.size(); ++idx) {
            if (!keybd_state.pendingLaterUp[idx]) continue;
            auto down_time = now - keybd_state.button_downTime[idx];
            if (down_time >= min_keepdown_time) {
                keybd_state.buttons[idx] = false;
                SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
                SendKeybdInput(vk_button[idx], KEYEVENTF_KEYUP);
                SetConsoleColor();
                keybd_state.pendingLaterUp[idx] = false;
            }
            else {
                long long tmp = 1 + (min_keepdown_time - down_time).count() / 1'000'000;
                if (stillPending == 0) {
                    stillPending = tmp;
                }
                else {
                    stillPending = (std::min)(stillPending, tmp);
                }
            }
        }
        for (size_t idx = 0; idx < keybd_state.pendingStickLaterUp.size(); ++idx) {
            if (!keybd_state.pendingStickLaterUp[idx]) continue;
            auto down_time = now - keybd_state.stick_downTime[idx];
            if (down_time >= min_keepdown_time) {
                BYTE vk = vk_stick(keybd_state.sticks[idx]);
                keybd_state.sticks[idx] = 0;
                SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
                SendKeybdInput(vk, KEYEVENTF_KEYUP);
                SetConsoleColor();
                keybd_state.pendingStickLaterUp[idx] = false;
            }
            else {
                long long tmp = 1 + (min_keepdown_time - down_time).count() / 1'000'000;
                if (stillPending == 0) {
                    stillPending = tmp;
                }
                else {
                    stillPending = (std::min)(stillPending, tmp);
                }
            }
        }
        return static_cast<int>(stillPending);
    }
    void cleanup_keybd_state() {
        for (size_t i = 0; i < keybd_state.buttons.size(); ++i) {
            if (keybd_state.buttons[i]) {
                SendKeybdInput(vk_button[i], KEYEVENTF_KEYUP);
            }
        }
        if (keybd_state.sticks[0]) {
            SendKeybdInput(vk_stick(keybd_state.sticks[0]), KEYEVENTF_KEYUP);
        }
        if (keybd_state.sticks[1]) {
            SendKeybdInput(vk_stick(keybd_state.sticks[1]), KEYEVENTF_KEYUP);
        }
        keybd_state = {};
    }
    ~Controller() {
        cleanup_keybd_state();
    }
};

std::atomic<bool> g_WebSocketRunning = false;
static void WebSocketProc(websocket::stream<tcp::socket> ws) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    ws.binary(true);
    try {
        std::string internal_strbuffer;
        internal_strbuffer.reserve(24); //多保留些空間，以便略過邊界檢查
        auto buffer = boost::asio::dynamic_buffer(internal_strbuffer);

        WSAPOLLFD pfd{};
        pfd.fd = ws.next_layer().native_handle();
        pfd.events = POLLRDNORM;

        Controller controller;
        NetStabilityMeter2 connection_tester;
        bool idle = false;
        int flush_laterUp = 0;
        auto sendPing = [&ws]() {
            long long time = time_since_epoch().count();
            static_assert(sizeof(time) == 8);
            ws.write(boost::asio::const_buffer(&time, 8));
            };
        sendPing();
        while (true) {
            int r = WSAPoll(&pfd, 1, flush_laterUp ? flush_laterUp-1 : 5000);   // 單位:毫秒
            if (r == 0) {
                // timeout
                if (flush_laterUp) {
                    flush_laterUp = controller.FlushLaterUp();
                    if (flush_laterUp == 1) {
                        //自旋跑完這最後1毫秒吧
                        for (int i = 0; i < 8192; ++i) {
                            _mm_pause();
                        }
                    }
                }
                else {
                    if (!idle) {
                        std::println("<-PING");
                        sendPing();
                        idle = true;
                    }
                    else {
                        // second timeout → disconnect
                        printError("[WebSocket] Idle timeout disconnect.");
                        break;
                    }
                }
                continue;
            }
            else {
                if (idle) {
                    std::println("->PONG");
                    idle = false;
                }
                do {
                    flush_laterUp = controller.FlushLaterUp();
                    buffer.consume((std::numeric_limits<std::size_t>::max)());
                    ws.read(buffer);  // <-- 若連線斷開這裡會丟例外
                    if (g_output_received_message) {
                        std::string s;
                        s.reserve(buffer.size() * 2);
                        for (int i = 0; i < buffer.size(); ++i) {
                            std::format_to(std::back_inserter(s), "{:02X}", static_cast<BYTE>(internal_strbuffer[i]));
                        }
                        std::println("->{}", s);
                    }
                    const char* p = internal_strbuffer.data();
                    switch (p[0]) {
                    case 'D': {
                        controller.ButtonDown(p[1]);
                        break;
                    }
                    case 'U': {
                        controller.ButtonUp(p[1]);
                        break;
                    }
                    case 'd': {
                        controller.StickDown(p[1]);
                        break;
                    }
                    case 'u': {
                        controller.StickUp(p[1]);
                        break;
                    }
                    case 'C': {
                        controller.cleanup_keybd_state();
                        printError("touchcancel");
                        MessageBeep(MB_ICONERROR);
                        break;
                    }
                    case 'R': {
                        INT64 serverSendTime = *(reinterpret_cast<const UNALIGNED INT64*>(p + 1));
                        INT64 clientRevcTime = *(reinterpret_cast<const UNALIGNED INT64*>(p + 9));
                        INT64 serverRevcTime = time_since_epoch().count();
                        int res = connection_tester.AddSample(serverSendTime, clientRevcTime, serverRevcTime);
                        if (res) sendPing();
                        break;
                    }
                    case 'T':
                        std::println("-> {}", std::string_view(p + 1, p + buffer.size()));
                        break;
                    default:
                        printError("Unknown message");
                        MessageBeep(MB_ICONERROR);
                        break;
                    }
                    //Warning: This depends on internal implementation details that were originally encapsulated as private.
                    //Because ws.next_layer().available() does not take into account the state of the internal buffer.
                } while (ws.impl_->rd_buf.size() != 0);
                flush_laterUp = controller.FlushLaterUp();
                if (g_output_received_message) {
                    std::println("---------------------------------------");
                }
            }
        }
    }
    catch (const beast::system_error& se) {
        // WebSocket 斷線通常會進到這邊
        if (se.code() == websocket::error::closed) {
            printError("[WebSocket] Client disconnected.");
        }
        else {
            printError("[WebSocket] Error: {}", se.code().message());
        }
    }
    catch (const std::exception& e) {
        printError("[WebSocket] Exception: {}", e.what());
    }
    g_WebSocketRunning = false;
    MessageBeep(MB_ICONERROR);
    std::println("[WebSocket] closed.\n"
        "[Server] Waiting for connection...");
}
const char* html404 = R"(<!doctype html>
<html>
    <head>
        <title>404 Not Found</title>
    </head>
    <body>
<h1>Can't open /WebClient/client.html</h1>
    </body>
</html>)";
void do_session(tcp::socket socket) {
    try {
        std::string internal_strbuffer;
        auto buffer = boost::asio::dynamic_buffer(internal_strbuffer);
        std::println("[Server] accepted");
        {
            // 有時會有瀏覽器的推測連線進來但又不發HTTP請求，導致Server卡在http::read
            // 2秒內沒動靜就直接退出，避免阻塞
            WSAPOLLFD pfd{};
            pfd.fd = socket.native_handle();
            pfd.events = POLLRDNORM;
            int r = WSAPoll(&pfd, 1, 2000);
            if (r == 0) {
                printError("[Server] Timed out");
                return;
            }
            else if (r == SOCKET_ERROR) {
                throw boost::system::error_code(WSAGetLastError(),
                    boost::asio::error::get_system_category());
            }
        }
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        tcp::endpoint remote = socket.remote_endpoint();

        [[gsl::suppress]]
        std::string_view req_target = req.target();

        std::println("[Server] HTTP request [{}] from {}:{}", req_target, remote.address().to_string(), remote.port());
        
        // ---- Serve HTML ----
        if (req_target == "/") {
            std::ifstream ifs("WebClient/client.html", std::ios::binary);

            http::response<http::string_body> res{
                http::status::ok, req.version()
            };
            res.set(http::field::content_type, "text/html");
            res.body().reserve(20408);
            res.body().append(
                std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>()
            );
            if (res.body().empty()) {
                res.body() = html404;
            }
            else {
                if (auto pos = res.body().find("<head>"); pos != std::string::npos) {
                    res.body().insert(pos+6,std::format(R"(<meta name="apple-mobile-web-app-title" content="{}">)",
                        socket.local_endpoint().address().to_string()));
                }
            }
            res.prepare_payload();
            http::write(socket, res);
            std::println("[Server] Reply {} bytes\n", res.body().size());
            return;
        }
        // ---- WebSocket Session ----
        else if (req_target == "/ws") {
            websocket::stream<tcp::socket> ws{ std::move(socket) };
            ws.accept(req);
            if (!g_WebSocketRunning) {
                tcp::endpoint remote = ws.next_layer().remote_endpoint();
                std::println("[Server] WebSocket connected from {}:{}\n", remote.address().to_string(), remote.port());
                g_WebSocketRunning = true;
                std::thread(WebSocketProc, std::move(ws)).detach();
            }
            else {
                printError("[Server] WebSocketProc is already running. Only one connection can be established at a time. Please close the previous connection and try again.\n");
            }
            return;
        }
        else {
            const char* content_type;
            auto status_code = http::status::ok;
            if (req_target.ends_with(".svg")) {
                content_type = "image/svg+xml";
            }
            else if (req_target.ends_with(".png")) {
                content_type = "image/png";
            }
            else if (req_target.ends_with(".ico")) {
                content_type = "image/x-icon";
            }
            else if (req_target.ends_with(".jpg") || req_target.ends_with(".jpeg")) {
                content_type = "image/jpeg";
            }
            else if (req_target.ends_with(".gif")) {
                content_type = "image/gif";
            }
            else {
                status_code = http::status::unsupported_media_type;
                content_type = nullptr;
            }

            std::vector<char> paylaod_buf;
            std::span<const char> paylaod;
            if (content_type) {
                if (req_target == "/favicon.ico") {
                    HRSRC hRsrc = FindResourceW(NULL, MAKEINTRESOURCE(1), MAKEINTRESOURCE(RT_ICON));
                    assert(hRsrc);
                    if(!hRsrc) goto from_file;
                    const char* pRes = reinterpret_cast<const char*>(LockResource(LoadResource(NULL, hRsrc)));
                    assert(pRes);
                    if (pRes) [[likely]]{
                        paylaod = { pRes, SizeofResource(NULL, hRsrc) };
                        content_type = "image/png";
                    }
                    else {
                        goto from_file;
                    }
                }
                else {
                    from_file:
                    std::ifstream ifs(std::string("WebClient").append(req_target), std::ios::binary);
                    if (ifs.is_open()) {
                        paylaod_buf.reserve(1024);
                        paylaod_buf.insert(paylaod_buf.end(),
                            std::istreambuf_iterator<char>(ifs),
                            std::istreambuf_iterator<char>()
                        );
                        paylaod = paylaod_buf;
                    }
                    else {
                        status_code = http::status::not_found;
                        content_type = nullptr;
                    }
                }
            }

            http::response<http::span_body<const char>> res{
                status_code, req.version(), paylaod
            };
            
            if (content_type) res.set(http::field::content_type, content_type);

            res.prepare_payload();
            http::write(socket, res);
            switch (status_code) {
            case http::status::ok:
                std::println("[Server] Reply {} bytes\n", res.body().size());
                return;
            case http::status::unsupported_media_type:
                std::println("[Server] 415 unsupported_media_type\n");
                return;
            case http::status::not_found:
                std::println("[Server] 404 not_found\n");
            default:
                return;
            }
        }
    }
    catch (const beast::system_error& se) {
        printError("[Server] Error: {}\n", se.code().message());
        MessageBeep(MB_ICONERROR);
    }
    catch (const std::exception& e) {
        printError("[Server] Exception: {}\n", e.what());
        MessageBeep(MB_ICONERROR);
    }
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    ReadAndPrintSettings();
    listLocalIPsAndAdapters();
    std::println("Service listening on port 3939\n");

    boost::asio::io_context ioc;
    tcp::acceptor acceptor{ ioc, {tcp::v4(), 3939} };

    while (true) {
        if (!g_WebSocketRunning) {
            std::println("[Server] Waiting for connection...");
        }

        tcp::socket socket{ ioc };
        acceptor.accept(socket);  // 阻塞直到有客戶端

        do_session(std::move(socket));
    }
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
