// ProjectDivaController.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8")
#define BOOST_SYSTEM_USE_UTF8
// Store the current warning state and set a minimal warning level (level 0 is lowest)
#pragma warning(push, 0)
// Warning: I modified boost_1_89_0\boost\beast\websocket\stream.hpp, 
// setting boost::beast::websocket::stream.impl_; to public.
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/flat_map.hpp>
// Restore the original warning state
#pragma warning(pop)
#include "HelperFunctionAndClass.h"

#include<Windows.h>
#include<fstream>
#include <thread>
#include <iostream>
#include <print>


namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

double g_slide_require_multiplier = 1.0;
#ifdef _DEBUG
bool g_output_received_message = true;
bool g_output_keyboard_operation = true;
#else
bool g_output_received_message = false;
bool g_output_keyboard_operation = false;
#endif // DEBUG

std::array<BYTE, 8> vk_button{
    'U',
    'H',
    'J',
    'K',
    VK_UP,
    VK_LEFT,
    VK_DOWN,
    VK_RIGHT,
};
auto vk_stick = [vk_s = std::array<BYTE, 5>{ '1', 'A', '\0', 'D', '3' }]
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
        if (std::getline(file, str) && str.size() >= 1) {
            double temp = atof(str.c_str());
            if (temp) {
                g_slide_require_multiplier = temp;
                std::getline(file, str)
                    && ((g_output_received_message = atoi(str.c_str())), std::getline(file, str))
                    && (g_output_keyboard_operation = atoi(str.c_str()));
            }
            else {
                error = true;
                goto err;
            }
        }
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
        "slide_require_multiplier : {:.2f}\n"
        "output_received_message : {}\n"
        "output_keyboard_operation : {}\n\n"
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
        , g_slide_require_multiplier
        , g_output_received_message
        , g_output_keyboard_operation
    );
}
//負責解析輸入並模擬按下按鍵
class Controller {
    struct PointerInfo {
        int x_coordinate = 0; //當前的x座標
        int y_coordinate = 0; //按下時的y座標(不是當前y座標)
        int next_x_coordinate = 0;
        int momentum = 0;
        int8_t pressingButton = 0;// 0保留給未按下 // 1/2/3/4 / 5/6/7/8
        int8_t pressingDirectionalButton = 0; //負代表向左，正代表向右  1是左邊的搖桿；2是右邊
        DWORD press_time = 0;
    };
    boost::container::flat_map<long long, int, std::less<long long>,
        boost::container::static_vector<std::pair<long long, int>, 20>> identifier_map;
    std::array<PointerInfo, 20> map_ID_cache;
    int later_up_count;
    int m_width;
    int m_height;
    int m_slider_height;
    struct {
        std::array<bool, 8> buttons;
        std::array<DWORD, 4> button_up_time;//主要按鍵上次抬起的時間；0代表最後抬起的是次要按鍵
        std::array<int16_t, 2>sticks;
    }keybd_state{};
    std::chrono::nanoseconds last_update_time;
public:

    Controller(int width = 1, int height = 1, int slider_height_ratio = 0) :
        m_width(width), m_height(height), m_slider_height(height* slider_height_ratio / 100),
        map_ID_cache(), keybd_state(), later_up_count(),
        last_update_time(time_since_epoch()) {
    }
    //return true 代表在在一小段時間後必須呼叫FlushLaterUp()
    bool OnTouchAction(const char* event) {
        switch (event[0]) {
        case 'P': {
            // pong
            // format:
            // "PONG" time_since_epoch
            auto now = time_since_epoch();

            long long timept = atoll(event + 4);
            if (timept == 0) {
                printError("PING returned an incorrect PONG format.\n");
            }
            else {
                std::print("Round-Trip Time: {}ns\n", format_thousands_separator(now.count() - timept));
            }
            return 0;
        }
        case 'D': {
            // pointer down
            // format:
            // 'D' identifier x y
            cheap_istrstream iss(event + 1);
            int pointer_ID = new_ID(iss.getLLInt());
            int x = iss.getInt();
            int y = iss.getInt();
            int button_index = x * 4 / m_width;
            if (button_index >= 4 || button_index < 0) {
                throw std::runtime_error("Invalid touch point coordinates");
            }
            map_ID_cache.at(pointer_ID) = {};
            map_ID_cache[pointer_ID].x_coordinate = x;
            map_ID_cache[pointer_ID].y_coordinate = y;
            map_ID_cache[pointer_ID].next_x_coordinate = x;
            if (y < m_slider_height) {
                break;
            }
            if (keybd_state.buttons[button_index] && keybd_state.buttons[button_index + 4]) {
                //兩個按鍵都已經按下去了，忽略這第三根手指
            }
            else {
                if (!keybd_state.buttons[button_index] && !keybd_state.buttons[button_index + 4]) {
                    //兩個按鍵都沒被按著
                    //優先選擇主要按鍵，除非上一個抬起來的是主要按鍵，且才剛被抬起來
                    if (keybd_state.button_up_time[button_index] != 0
                        && GetTickCount32() - keybd_state.button_up_time[button_index] < 100) {
                        button_index += 4;
                    }
                }
                else {
                    //只有一個按鍵按著；選另一個
                    if (keybd_state.buttons[button_index]) {
                        button_index += 4;
                    }
                }
            }
            if (!keybd_state.buttons[button_index]) {
                //按下按鍵
                keybd_state.buttons[button_index] = true;
                map_ID_cache[pointer_ID].pressingButton = button_index + 1;
                map_ID_cache[pointer_ID].press_time = GetTickCount32();
                SendKeybdInput(vk_button[button_index]);
            }
            break;
        }
        case 'U': {
            // pointer up
            // format:
            // 'U' ID
            FlushMoveAction();
            cheap_istrstream iss(event + 1);
            long long identifier = iss.getLLInt();
            int pointer_ID = get_ID(identifier);
            remove_ID(identifier);
            return ActionPointerUp(pointer_ID);
        }
        case 'M': {
            // move
            // format:
            // 'M' ID x1 y1 ID x2 y2 ID x3 y3 ...
            cheap_istrstream iss(event + 2);
            do {
                int ID = get_ID(iss.getInt());
                int x = iss.getInt();
                int y = iss.getInt();
                map_ID_cache.at(ID).next_x_coordinate = x;
            } while (iss.data() && *iss.data() == ' ');
            break;
        }
        case 'C': {
            cleanup_keybd_state();
            printError("[GameController] ACTION_CANCEL");
            MessageBeep(MB_ICONERROR);
            break;
        }
        default:
            return 0;
        }
        return 0;
    }
    void FlushLaterUp() {
        SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        for (; later_up_count > 0; --later_up_count) {
            map_ID_cache[map_ID_cache.size() - later_up_count].press_time = 0;
            ActionPointerUp(map_ID_cache.size() - later_up_count);
        }
        SetConsoleColor();
    }
    void FlushMoveAction() {
        SetConsoleColor(FOREGROUND_GREEN);

        const auto nexttp = time_since_epoch();
        const auto duration = nexttp - last_update_time;
        last_update_time = nexttp;
        struct ID_displacement {
            int ID;
            int displacement;
        };
        boost::container::static_vector<ID_displacement, 20> candidate;
        for (int i = 0; i < map_ID_cache.size(); ++i) {
            if (map_ID_cache[i].next_x_coordinate - map_ID_cache[i].x_coordinate != 0) {
                candidate.push_back({ i,map_ID_cache[i].next_x_coordinate - map_ID_cache[i].x_coordinate });
            }
        }
        for (auto& i : candidate) {
            if (map_ID_cache[i.ID].y_coordinate < m_slider_height) {
                i.displacement *= 16;
            }
        }

        if (g_output_received_message) {
            for (auto i : candidate) {
                std::print("candidates:[ID:{} displacement:{}]", i.ID, i.displacement);
            }
            std::print("\n");
        }
        const int min_displacement_require = MinDisplacementRequire((std::min)(duration, std::chrono::nanoseconds(33'554'432)));
        const int max_momentum = MinDisplacementRequire(std::chrono::nanoseconds(16'777'216));
        const int reduce_displacement = MinDisplacementRequire(duration) / 8;
        //結算先前動量
        {
            //增加動量
            int max_momentum2 = max_momentum + reduce_displacement;
            for (auto i : candidate) {
                map_ID_cache[i.ID].momentum += i.displacement;
                if (abs(map_ID_cache[i.ID].momentum) > max_momentum2) {
                    map_ID_cache[i.ID].momentum =
                        (map_ID_cache[i.ID].momentum > 0) ?
                        max_momentum2 : -max_momentum2;
                }
            }

            //減少動量並抬起按鍵
            for (auto& i : map_ID_cache) {
                if (i.momentum > reduce_displacement) {
                    i.momentum -= reduce_displacement;
                }
                else if (i.momentum < -reduce_displacement) {
                    i.momentum += reduce_displacement;
                }
                else {
                    i.momentum = 0;
                }
                if (i.pressingDirectionalButton) {
                    if (i.pressingDirectionalButton * i.momentum <= 0) {
                        //動量消耗殆盡或方向反了
                        //抬起滑鍵
                        keybd_state.sticks.at(abs(i.pressingDirectionalButton) - 1) = 0;
                        SendKeybdInput(vk_stick(i.pressingDirectionalButton), KEYEVENTF_KEYUP);
                        i.pressingDirectionalButton = 0;
                    }
                    else {
                        //把此手指從候選者中剃除
                        for (auto& cand : candidate) {
                            if (cand.ID == std::addressof(i) - map_ID_cache.data()) {

                                cand.displacement = 0;
                            }
                        }
                    }
                }
            }
        }

        std::sort(candidate.begin(), candidate.end(), [](ID_displacement a, ID_displacement b) {
            return abs(a.displacement) > abs(b.displacement);
            });

        if (g_output_received_message && candidate.size() > 0) {
            std::print("min_displacement_require:{} maxdisplacement:{} {}\n\n"
                , min_displacement_require, candidate.front().displacement, candidate.front().ID);
        }
        int freestickcount = (keybd_state.sticks[0] == 0) + (keybd_state.sticks[1] == 0);
        if (freestickcount == 2
            && candidate.size() >= 2
            && abs(candidate[1].displacement) > min_displacement_require) {
            auto candidateL = candidate.front();
            auto candidateR = candidate[1];
            if (map_ID_cache[candidateL.ID].x_coordinate > map_ID_cache[candidateR.ID].x_coordinate) {
                std::swap(candidateL, candidateR);
            }
            map_ID_cache[candidateL.ID].pressingDirectionalButton =
                keybd_state.sticks[0] =
                candidateL.displacement > 0 ? 1 : -1;
            map_ID_cache[candidateL.ID].press_time = GetTickCount32();

            map_ID_cache[candidateR.ID].pressingDirectionalButton =
                keybd_state.sticks[1] =
                candidateR.displacement > 0 ? 2 : -2;
            map_ID_cache[candidateR.ID].press_time = GetTickCount32();
            //給予初始動量
            {
                map_ID_cache[candidateL.ID].momentum += candidateL.displacement;
                if (abs(map_ID_cache[candidateL.ID].momentum) > max_momentum) {
                    map_ID_cache[candidateL.ID].momentum =
                        (map_ID_cache[candidateL.ID].momentum > 0) ?
                        max_momentum : -max_momentum;
                }
                map_ID_cache[candidateR.ID].momentum += candidateR.displacement;
                if (abs(map_ID_cache[candidateR.ID].momentum) > max_momentum) {
                    map_ID_cache[candidateR.ID].momentum =
                        (map_ID_cache[candidateR.ID].momentum > 0) ?
                        max_momentum : -max_momentum;
                }
            }
            INPUT input[2]{};
            input[0].type = input[1].type = INPUT_KEYBOARD;
            input[0].ki.wVk = vk_stick(keybd_state.sticks[0]);
            input[1].ki.wVk = vk_stick(keybd_state.sticks[1]);
            SendInput(2, input, sizeof(INPUT));
            std::print("{} {}  [DOWN]\n",
                keybd_state.sticks[0] > 0 ? "->" : "<-",
                keybd_state.sticks[1] > 0 ? "->" : "<-");
        }
        else if (freestickcount >= 1
            && candidate.size() >= 1
            && abs(candidate[0].displacement) > min_displacement_require) {
            auto Candidate = candidate.front();
            int LR;
            if (freestickcount == 2) {
                LR = 1 + (map_ID_cache[Candidate.ID].x_coordinate > m_width / 2);
            }
            else {
                LR = keybd_state.sticks[0] == 0 ? 1 : 2;
            }
            map_ID_cache[Candidate.ID].pressingDirectionalButton =
                keybd_state.sticks[LR - 1] =
                Candidate.displacement > 0 ? LR : -LR;
            map_ID_cache[Candidate.ID].press_time = GetTickCount32();

            //給予初始動量
            map_ID_cache[Candidate.ID].momentum += Candidate.displacement;
            if (abs(map_ID_cache[Candidate.ID].momentum) > max_momentum) {
                map_ID_cache[Candidate.ID].momentum =
                    (map_ID_cache[Candidate.ID].momentum > 0) ?
                    max_momentum : -max_momentum;
            }
            SendKeybdInput(vk_stick(keybd_state.sticks[LR - 1]));
        }
        for (auto& i : map_ID_cache) {
            i.x_coordinate = i.next_x_coordinate;
        }
        SetConsoleColor();
    }
    ~Controller() {
        cleanup_keybd_state();
    }
private:
    int MinDisplacementRequire(std::chrono::nanoseconds duration) const noexcept {
        return static_cast<int>(duration.count() * g_slide_require_multiplier * m_width / 1'073'741'824);
    }
    bool ActionPointerUp(int pointer_ID) {
        if (map_ID_cache.at(pointer_ID).pressingButton || map_ID_cache[pointer_ID].pressingDirectionalButton) {
            if (20 > GetTickCount32() - map_ID_cache[pointer_ID].press_time) {
                //This pointer才剛按下某個按鍵，為了確保此按鍵能被遊戲偵測到，稍微延遲1幀的時間再抬起
                ++later_up_count;
                map_ID_cache[map_ID_cache.size() - later_up_count] = map_ID_cache[pointer_ID];
                map_ID_cache[pointer_ID] = {};
                return true;
            }
            if (map_ID_cache[pointer_ID].pressingButton) {
                int button_index = map_ID_cache[pointer_ID].pressingButton - 1;
                keybd_state.buttons.at(button_index) = false;
                SendKeybdInput(vk_button[button_index], KEYEVENTF_KEYUP);
                if (button_index < 4) {
                    keybd_state.button_up_time[button_index] = GetTickCount32();
                }
                else {
                    keybd_state.button_up_time[button_index - 4] = 0;
                }
            }
            if (map_ID_cache[pointer_ID].pressingDirectionalButton) {
                keybd_state.sticks.at(abs(map_ID_cache[pointer_ID].pressingDirectionalButton) - 1) = 0;
                SendKeybdInput(vk_stick(map_ID_cache[pointer_ID].pressingDirectionalButton), KEYEVENTF_KEYUP);
            }
        }
        map_ID_cache[pointer_ID] = {};
        return false;
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
    void cleanup_keybd_state() {
        map_ID_cache = {};
        identifier_map.clear();
        for (int i = 0; i < keybd_state.buttons.size(); ++i) {
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
    int new_ID(long long identifier) {
        int ID = 0;
    again:
        for (const auto& i : identifier_map) {
            if (i.second == ID) {
                ++ID;
                goto again;
            }
        }
        identifier_map.insert({ identifier, ID });
        return ID;
    }
    int get_ID(long long identifier) {
        return identifier_map.at(identifier);
    }
    size_t remove_ID(long long identifier) {
        return identifier_map.erase(identifier);
    }
};

std::atomic<bool> g_WebSocketRunning = false;
static void WebSocketProc(websocket::stream<tcp::socket> ws) {
    try {
        std::string internal_strbuffer;
        auto buffer = boost::asio::dynamic_buffer(internal_strbuffer);

        WSAPOLLFD pfd{};
        pfd.fd = ws.next_layer().native_handle();
        pfd.events = POLLRDNORM;

        Controller controller;
        bool idle = false;
        bool flash_laterUp = false;
        while (true) {
            int r = WSAPoll(&pfd, 1, flash_laterUp ? 10 : 5000);   // 單位:毫秒
            if (r == 0) {
                // timeout
                if (flash_laterUp) {
                    controller.FlushLaterUp();
                    flash_laterUp = false;
                }
                else {
                    if (!idle) {
                        std::string msg = std::format("PING {}\n", time_since_epoch().count());
                        ws.write(boost::asio::const_buffer(msg.c_str(), msg.size()));
                        std::print("<-{}", msg);
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
                idle = false;
                controller.FlushLaterUp();
                flash_laterUp = false;
                do {
                    buffer.consume((std::numeric_limits<std::size_t>::max)());
                    ws.read(buffer);  // <-- 若連線斷開這裡會丟例外
                    if (g_output_received_message) {
                        std::print("->{}", internal_strbuffer);
                    }
                    const char* p = internal_strbuffer.data();
                    switch (p[0]) {
                    case 'D':
                    case 'U':
                    case 'C':
                    case 'M':
                    case 'P':
                        flash_laterUp |= controller.OnTouchAction(p);
                        break;
                    case 'S': {
                        //format:"SCREENSIZE:" width height xdpi ydpi devicename
                        cheap_istrstream iss(p);
                        iss.skip();
                        int width = iss.getInt();
                        int height = iss.getInt();
                        int slider_height_ratio = iss.getInt();
                        controller = Controller(width, height, slider_height_ratio);
                        break;
                    }
                    }
                    //Warning: This depends on internal implementation details that were originally encapsulated as private.
                    //Because ws.next_layer().available() does not take into account the state of the internal buffer.
                } while (ws.impl_->rd_buf.size());
                controller.FlushMoveAction();
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
    std::println("[Server] Waiting for connection...");
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

        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        // ---- Serve HTML ----
        if (req.target() == "/") {
            tcp::endpoint remote = socket.remote_endpoint();
            std::println("[Server] HTTP request from {}:{}", remote.address().to_string(), remote.port());

            std::ifstream ifs("WebClient/client.html", std::ios::binary);

            http::response<http::string_body> res{
                http::status::ok, req.version()
            };
            res.set(http::field::content_type, "text/html");
            res.body() = std::string(
                (std::istreambuf_iterator<char>(ifs)),
                std::istreambuf_iterator<char>()
            );
            if (res.body().empty()) {
                res.body() = html404;
            }
            res.prepare_payload();
            http::write(socket, res);
            std::println("[Server] Reply {} bytes", res.body().size());
            return;
        }
        else if (req.target().starts_with("/image/")) {
            const char* content_type;
            if (req.target().ends_with(".svg")) {
                content_type = "image/svg+xml";
            }
            else if (req.target().ends_with(".png")) {
                content_type = "image/png";
            }
            else if (req.target().ends_with(".jpg") || req.target().ends_with(".jpeg")) {
                content_type = "image/jpeg";
            }
            else if (req.target().ends_with(".gif")) {
                content_type = "image/gif";
            }
            else {
                goto unknown_target;
            }

            tcp::endpoint remote = socket.remote_endpoint();
            std::println("[Server] HTTP request [{}] from {}:{}", req.target(), remote.address().to_string(), remote.port());

            std::string filename = "WebClient" + std::string(req.target());
            std::ifstream ifs(filename, std::ios::binary);

            http::response<http::vector_body<uint8_t>> res{
                http::status::ok, req.version()
            };

            res.set(http::field::content_type, content_type);
            res.body() = std::vector<uint8_t>(
                std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>()
            );
            res.prepare_payload();
            http::write(socket, res);
            std::println("[Server] Reply {} bytes", res.body().size());
            return;
        }
        // ---- WebSocket Session ----
        else if (req.target() == "/ws") {
            websocket::stream<tcp::socket> ws{ std::move(socket) };
            ws.accept(req);
            {
                tcp::endpoint remote = ws.next_layer().remote_endpoint();
                std::println("[Server] WebSocket connected from {}:{}", remote.address().to_string(), remote.port());
                SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
            }
            if (!g_WebSocketRunning) {
                g_WebSocketRunning = true;
                std::thread(WebSocketProc, std::move(ws)).detach();
            }
            else {
                printError("WebSocketProc is already running. Only one connection can be established at a time. Please close the previous connection and try again.");
            }
        }
        else {
            unknown_target:
            tcp::endpoint remote = socket.remote_endpoint();
            std::println("[Server] HTTP request [{}] from {}:{}\n"
                "[Server] Nothing replied.\n", req.target(), remote.address().to_string(), remote.port());
        }
    }
    catch (const beast::system_error& se) {
        printError("[Server] Error: {}", se.code().message());
        MessageBeep(MB_ICONERROR);
    }
    catch (const std::exception& e) {
        printError("[Server] Exception: {}", e.what());
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
