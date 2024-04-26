#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        if (argc != 4 && argc != 5) {
            std::cerr << "Usage: http_client <method> <host> <port> [<HTTP version>]\n";
            return EXIT_FAILURE;
        }

        std::string method_str(argv[1]);
        std::transform(method_str.begin(), method_str.end(), method_str.begin(), ::toupper);
        http::verb method = http::string_to_verb(method_str);
        if (method == http::verb::invalid) {
            std::cerr << "Invalid HTTP method\n";
            return EXIT_FAILURE;
        }

        std::string host = argv[2];
        std::string port = argv[3];
        int version = argc == 5 ? std::stoi(argv[4]) : 11;

        boost::asio::io_context ioc;
        tcp::resolver resolver{ioc};
        auto const results = resolver.resolve(host, port);

        tcp::socket socket{ioc};
        boost::asio::connect(socket, results.begin(), results.end());

        http::request<http::string_body> req{method, "/", version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::authorization, "Basic " +
                boost::beast::base64_encode("admin:admin_password"));

        http::write(socket, req);

        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        std::cout << res << std::endl;

        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != boost::system::errc::not_connected) {
            throw boost::system::system_error{ec};
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
