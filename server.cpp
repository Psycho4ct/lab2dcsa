#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <map>

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

std::map<std::string, std::pair<std::string, bool>> users = {
    {"admin", {"admin_password", true}},
    {"user", {"user_password", false}}
};

void handle_request(http::request<http::string_body> req, tcp::socket& socket) {
    std::string auth_header = req.base()["Authorization"];
    if (auth_header.empty()) {
        http::response<http::string_body> res{http::status::unauthorized, req.version()};
        res.set(http::field::server, "Server");
        res.set(http::field::www_authenticate, "Basic realm=\"Protected\"");
        res.keep_alive(req.keep_alive());
        http::write(socket, res);
        return;
    }

    std::string username_password = boost::beast::buffers_to_string(
        boost::beast::decode_base64(boost::asio::buffer(auth_header.begin() + 6, auth_header.end())));
    size_t pos = username_password.find(":");
    std::string username = username_password.substr(0, pos);
    std::string password = username_password.substr(pos + 1);

    if (users.find(username) == users.end() || users[username].first != password) {
        http::response<http::string_body> res{http::status::unauthorized, req.version()};
        res.set(http::field::server, "Server");
        res.set(http::field::www_authenticate, "Basic realm=\"Protected\"");
        res.keep_alive(req.keep_alive());
        http::write(socket, res);
        return;
    }

    // Обработка запроса после успешной аутентификации
    bool is_admin = users[username].second;
    if (is_admin) {
        // Действия, доступные только администратору
    } else {
        // Действия, доступные только обычному пользователю
    }

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, "Server");
    res.set(http::field::content_type, "text/plain");
    res.body() = "Hello, " + username + "!";
    res.keep_alive(req.keep_alive());
    http::write(socket, res);
}

int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context ioc;
        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            boost::beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);
            handle_request(std::move(req), socket);
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
