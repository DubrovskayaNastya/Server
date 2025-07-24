#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <windows.h>
#include <locale>
#include <fcntl.h>
#include <io.h>
#include <iomanip>

using namespace boost::asio;
using ip::tcp;
using namespace std;

wstring utf8_to_wstring(const string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
    wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

string wstring_to_utf8(const wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) return "";
    string str(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

void chat_server(tcp::socket&socket) 
{
    wcout << "Client connected!\n";
    while (true) {
        char data[512];
        boost::system::error_code error;
        size_t length = socket.read_some(buffer(data), error);
        if (error) break;
        wcout << right << setw(100) << utf8_to_wstring(string(data, length)) << "\n";
    }
}
void chat_client(tcp::socket& socket)
{
    while (true) {
        wstring message;
        getline(wcin, message);
        socket.write_some(buffer(wstring_to_utf8(message)));
        this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 5500));
    tcp::socket socket(io);
    acceptor.accept(socket); 

    thread chat1(chat_server,ref(socket));
    thread chat2(chat_client,ref(socket));

    chat1.join();
    chat2.join();

    return 0;
}
