#pragma once
#ifndef HELPERFUNCTIONANDCLASS_H
#define HELPERFUNCTIONANDCLASS_H
#include <windows.h>
#include <print>
#define BOOST_NOWIDE_NO_LIB
#include <boost/nowide/utf/convert.hpp>
#include <boost/nowide/convert.hpp>
#include <format>
#include <algorithm>
#include <vector>
#include <chrono>
#include <numeric>
#include <cassert>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
const char* vkToString(BYTE vk) {
    if (vk >= '0' && vk <= 'Z') {
        return ((vk - '0') * 2) + "0\0""1\0""2\0""3\0""4\0""5\0""6\0""7\0""8\0""9\0"
            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
            "A\0B\0C\0D\0E\0F\0G\0H\0I\0J\0K\0L\0M\0N\0O\0P\0Q\0R\0S\0T\0U\0V\0W\0X\0Y\0Z";
    }
    switch (vk) {
        //這個macro沒遵守ALL_CAPS原則，但話說回來，這個macro的作用域也被限制在了這個switch中，因此沒有問題(也許還可以因此避免與其他macro撞名)
#define caseStringify(x) case x: return #x
        // VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
        // 0x3A - 0x40 : Undefined
        // VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
        caseStringify(VK_LBUTTON);
        caseStringify(VK_RBUTTON);
        caseStringify(VK_CANCEL);
        caseStringify(VK_MBUTTON);  // NOT contiguous with L & RBUTTON
        caseStringify(VK_XBUTTON1); // NOT contiguous with L & RBUTTON
        caseStringify(VK_XBUTTON2); // NOT contiguous with L & RBUTTON
        caseStringify(VK_BACK);
        caseStringify(VK_TAB);
        caseStringify(VK_CLEAR);
        caseStringify(VK_RETURN);
        caseStringify(VK_SHIFT);
        caseStringify(VK_CONTROL);
        caseStringify(VK_MENU);
        caseStringify(VK_PAUSE);
        caseStringify(VK_CAPITAL);
        caseStringify(VK_KANA);
        caseStringify(VK_JUNJA);
        caseStringify(VK_FINAL);
        caseStringify(VK_KANJI);
        caseStringify(VK_ESCAPE);
        caseStringify(VK_CONVERT);
        caseStringify(VK_NONCONVERT);
        caseStringify(VK_ACCEPT);
        caseStringify(VK_MODECHANGE);
        caseStringify(VK_SPACE);
        caseStringify(VK_PRIOR);
        caseStringify(VK_NEXT);
        caseStringify(VK_END);
        caseStringify(VK_HOME);
        caseStringify(VK_LEFT);
        caseStringify(VK_UP);
        caseStringify(VK_RIGHT);
        caseStringify(VK_DOWN);
        caseStringify(VK_SELECT);
        caseStringify(VK_PRINT);
        caseStringify(VK_EXECUTE);
        caseStringify(VK_SNAPSHOT);
        caseStringify(VK_INSERT);
        caseStringify(VK_DELETE);
        caseStringify(VK_HELP);
        caseStringify(VK_LWIN);
        caseStringify(VK_RWIN);
        caseStringify(VK_APPS);
        caseStringify(VK_SLEEP);
        caseStringify(VK_NUMPAD0);
        caseStringify(VK_NUMPAD1);
        caseStringify(VK_NUMPAD2);
        caseStringify(VK_NUMPAD3);
        caseStringify(VK_NUMPAD4);
        caseStringify(VK_NUMPAD5);
        caseStringify(VK_NUMPAD6);
        caseStringify(VK_NUMPAD7);
        caseStringify(VK_NUMPAD8);
        caseStringify(VK_NUMPAD9);
        caseStringify(VK_MULTIPLY);
        caseStringify(VK_ADD);
        caseStringify(VK_SEPARATOR);
        caseStringify(VK_SUBTRACT);
        caseStringify(VK_DECIMAL);
        caseStringify(VK_DIVIDE);
        caseStringify(VK_F1);
        caseStringify(VK_F2);
        caseStringify(VK_F3);
        caseStringify(VK_F4);
        caseStringify(VK_F5);
        caseStringify(VK_F6);
        caseStringify(VK_F7);
        caseStringify(VK_F8);
        caseStringify(VK_F9);
        caseStringify(VK_F10);
        caseStringify(VK_F11);
        caseStringify(VK_F12);
        caseStringify(VK_F13);
        caseStringify(VK_F14);
        caseStringify(VK_F15);
        caseStringify(VK_F16);
        caseStringify(VK_F17);
        caseStringify(VK_F18);
        caseStringify(VK_F19);
        caseStringify(VK_F20);
        caseStringify(VK_F21);
        caseStringify(VK_F22);
        caseStringify(VK_F23);
        caseStringify(VK_F24);
        caseStringify(VK_NUMLOCK);
        caseStringify(VK_SCROLL);
        caseStringify(VK_OEM_NEC_EQUAL);  // '=' key on numpad
        caseStringify(VK_OEM_FJ_MASSHOU); // 'Unregister word' key
        caseStringify(VK_OEM_FJ_TOUROKU); // 'Register word' key
        caseStringify(VK_OEM_FJ_LOYA);    // 'Left OYAYUBI' key
        caseStringify(VK_OEM_FJ_ROYA);    // 'Right OYAYUBI' key
        caseStringify(VK_LSHIFT);
        caseStringify(VK_RSHIFT);
        caseStringify(VK_LCONTROL);
        caseStringify(VK_RCONTROL);
        caseStringify(VK_LMENU);
        caseStringify(VK_RMENU);
        caseStringify(VK_BROWSER_BACK);
        caseStringify(VK_BROWSER_FORWARD);
        caseStringify(VK_BROWSER_REFRESH);
        caseStringify(VK_BROWSER_STOP);
        caseStringify(VK_BROWSER_SEARCH);
        caseStringify(VK_BROWSER_FAVORITES);
        caseStringify(VK_BROWSER_HOME);
        caseStringify(VK_VOLUME_MUTE);
        caseStringify(VK_VOLUME_DOWN);
        caseStringify(VK_VOLUME_UP);
        caseStringify(VK_MEDIA_NEXT_TRACK);
        caseStringify(VK_MEDIA_PREV_TRACK);
        caseStringify(VK_MEDIA_STOP);
        caseStringify(VK_MEDIA_PLAY_PAUSE);
        caseStringify(VK_LAUNCH_MAIL);
        caseStringify(VK_LAUNCH_MEDIA_SELECT);
        caseStringify(VK_LAUNCH_APP1);
        caseStringify(VK_LAUNCH_APP2);
        caseStringify(VK_OEM_1);      // ';:' for US
        caseStringify(VK_OEM_PLUS);   // '+' any country
        caseStringify(VK_OEM_COMMA);  // ',' any country
        caseStringify(VK_OEM_MINUS);  // '-' any country
        caseStringify(VK_OEM_PERIOD); // '.' any country
        caseStringify(VK_OEM_2);  // '/?' for US
        caseStringify(VK_OEM_3);  // '`~' for US
        caseStringify(VK_OEM_4);  //  '[{' for US
        caseStringify(VK_OEM_5);  //  '\|' for US
        caseStringify(VK_OEM_6);  //  ']}' for US
        caseStringify(VK_OEM_7);  //  ''"' for US
        caseStringify(VK_OEM_8);
        caseStringify(VK_OEM_AX);   //  'AX' key on Japanese AX kbd
        caseStringify(VK_OEM_102);  //  "<>" or "\|" on RT 102-key kbd.
        caseStringify(VK_ICO_HELP); //  Help key on ICO
        caseStringify(VK_ICO_00);   //  00 key on ICO
        caseStringify(VK_PROCESSKEY);
        caseStringify(VK_ICO_CLEAR);
        caseStringify(VK_PACKET);
#undef caseStringify
    }
    assert("Invalid VK_CODE");
    return "Invalid VK_CODE";
}

struct format_thousands_separator {
    long long value;
};
template <>
struct std::formatter<format_thousands_separator> {
    std::formatter<std::string_view> base_;

    constexpr auto parse(std::format_parse_context& ctx)
    {
        return base_.parse(ctx);
    }

    auto format(format_thousands_separator num, std::format_context& ctx) const
    {
        char digits_buf[26]; // "-9,223,372,036,854,775,808"
        auto end = std::to_chars(digits_buf, digits_buf + sizeof(digits_buf), num.value).ptr;

        char* p = std::end(digits_buf);

        int group = 0;

        while (end != digits_buf) {
            char c = *--end;

            if (group == 3 && c != '-') {
                *--p = ',';
                group = 0;
            }

            *--p = c;
            ++group;
        }

        return base_.format(std::string_view{ p, static_cast<size_t>(std::end(digits_buf) - p) }, ctx);
    }
};
static void SetConsoleColor(int color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

template <class... _Types>
void printError(const std::format_string<_Types...> _Fmt, _Types&&... _Args) {
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::println(stderr, _Fmt, std::forward<_Types>(_Args)...);
    SetConsoleColor();
}

void listLocalIPsAndAdapters() {
    ULONG outBufLen = 16360;
    std::vector<BYTE> buffer(outBufLen);
    IP_ADAPTER_ADDRESSES* addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    ULONG res = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, addresses, &outBufLen);

    if (ERROR_BUFFER_OVERFLOW == res) {
        buffer.resize(outBufLen);
        res = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, addresses, &outBufLen);
    }

    if (NO_ERROR != res) {
        printError("Failed to get adapter addresses\n");
        return;
    }

    std::string str;
    str.reserve(2024);

    str += "=== Local network interfaces ===\n";

    char utf8buffer[2048]; utf8buffer[0] = '\0';
    for (auto* adapter = addresses; adapter; adapter = adapter->Next) {
        // 忽略未啟用或沒有 IP 的介面
        if (adapter->OperStatus != IfOperStatusUp) continue;
        if (!adapter->FirstUnicastAddress) continue;

        str += '[';
        boost::nowide::narrow(utf8buffer, sizeof(utf8buffer), adapter->FriendlyName);
        str += utf8buffer;
        str += "] ";
        boost::nowide::narrow(utf8buffer, sizeof(utf8buffer), adapter->Description);
        str += utf8buffer;
        str += '\n';

        for (auto* ua = adapter->FirstUnicastAddress; ua; ua = ua->Next) {
            sockaddr_in* sa_in = reinterpret_cast<sockaddr_in*>(ua->Address.lpSockaddr);

            inet_ntop(AF_INET, &(sa_in->sin_addr), utf8buffer, sizeof(utf8buffer));
            str += "\tIPv4: ";
            str += utf8buffer;
            str += '\n';
        }
        str += '\n';

    }
    std::print("{}", str);
}
//std::chrono::steady_clock::now().time_since_epoch() 的縮寫
std::chrono::nanoseconds time_since_epoch() noexcept {
    return std::chrono::steady_clock::now().time_since_epoch();
}
class NetStabilityMeter2 {
    struct Sample {
        long long serverSendTime;
        long long clientRevcTime; //Once the client receives the message, it will reply immediately; therefore, clientRevcTime == clientSendTime.
        long long serverRevcTime;
    };
    std::vector<Sample> rtsamples;
    long long estimate_offset = 0; //delay= serverRevcTime - clientSendTime - offset
    long long deviation_range = 0;
    static constexpr int max_sample_count = 512;
public:
    NetStabilityMeter2() {
        rtsamples.reserve(max_sample_count);
    }
    int AddSample(long long serverSendTime, long long clientRevcTime, long long serverRevcTime) {
        if (rtsamples.size() < max_sample_count) {
            rtsamples.push_back({ serverSendTime ,clientRevcTime,serverRevcTime });
        }
        else {
            EvaluateDelay();
            rtsamples.clear();
            return 0;
        }
        return 1;
    }
    void EvaluateOffset() {
        long long upper_bound_offset; {
            auto it = std::min_element(rtsamples.begin(), rtsamples.end(),
                [](const Sample& a, const Sample& b) {return a.serverRevcTime - a.clientRevcTime < b.serverRevcTime - b.clientRevcTime; });
            upper_bound_offset = it->serverRevcTime - it->clientRevcTime;
        }
        long long lower_bound_offset; {
            auto it = std::max_element(rtsamples.begin(), rtsamples.end(),
                [](const Sample& a, const Sample& b) {return a.serverSendTime - a.clientRevcTime < b.serverSendTime - b.clientRevcTime; });
            lower_bound_offset = it->serverSendTime - it->clientRevcTime;
        }
        long long oldoffset = estimate_offset;
        deviation_range = (upper_bound_offset - lower_bound_offset) / 2;
        estimate_offset = lower_bound_offset + deviation_range;
    }
    void EvaluateDelay() {
        EvaluateOffset();
        long long average = 0; double standard_deviation; long long maxdelay = 0; {
            double squares_sum = 0;
            for (const auto& i : rtsamples) {
                long long delay = i.serverRevcTime - i.clientRevcTime - estimate_offset;
                average += delay;
                maxdelay = (std::max)(maxdelay, delay);
                squares_sum += static_cast<double>(delay * delay);
            }
            average /= rtsamples.size();
            standard_deviation = std::sqrt((squares_sum / rtsamples.size()) - average * average);
        }
        std::println("In recent {} tests:\n"
            "Average latency:   {:>16}ns\n"
            "Maximum latency:   {:>16}ns\n"
            "Accuracy:          ±{:>15}ns + 1ms\n"
            "Standard deviation:{:>16}ns\n\n"
            , max_sample_count
            , format_thousands_separator(average)
            , format_thousands_separator(maxdelay)
            , format_thousands_separator(deviation_range)
            , format_thousands_separator(static_cast<long long>(standard_deviation)));
    }
};

void BusySleepUntil(std::chrono::steady_clock::time_point tp) {
    for (;;) {
        auto time_left = tp - std::chrono::steady_clock::now();
        if (time_left.count() <= 0) {
            return;
        }
        if (time_left.count() >= 8'000'000) {
            Sleep((time_left.count() / 1'000'000) - 7);
        }
        else {
            for (int i = 0; i < 8192; ++i) {
                _mm_pause();
            }
        }
    }
}
#endif // HELPERFUNCTIONANDCLASS_H