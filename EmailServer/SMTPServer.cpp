/*
Explanation
1.	SMTP Server:
•	The SMTPServer class listens for incoming connections on a specified port.
•	It handles basic SMTP commands like HELO, MAIL FROM, RCPT TO, DATA, and QUIT.
2.	Client Handling:
•	Each client connection is handled in a separate thread to allow multiple clients to connect simultaneously.
3.	SMTP Commands:
•	HELO: Identifies the client to the server.
•	MAIL FROM: Specifies the sender's email address.
•	RCPT TO: Specifies the recipient's email address.
•	DATA: Sends the email content.
•	QUIT: Closes the connection.
4.	Email Data:
•	The email content is read until a line containing only . is received, which indicates the end of the email.
5.	Error Handling:
•	Exceptions are caught and logged to handle connection or protocol errors gracefully.
---

Key Features
1.	Basic SMTP Protocol:
•	Implements core SMTP commands for sending emails.
2.	Multithreading:
•	Handles multiple clients simultaneously using threads.
3.	Error Handling:
•	Catches and logs exceptions for robust operation.
---
Enhancements
1.	Authentication:
•	Add support for SMTP authentication (e.g., AUTH LOGIN).
2.	TLS/SSL Support:
•	Use Boost.Asio's SSL features to secure the connection.
3.	Email Storage:
•	Store received emails in a database or file system.
4.	Advanced Features:
•	Add support for additional SMTP commands and error codes.
---
Dependencies
•	Boost Library:
•	Install Boost (https://www.boost.org/) to use Boost.Asio


Example Input/Output
Input (Client Commands):
HELO example.com
MAIL FROM:<sender@example.com>
RCPT TO:<recipient@example.com>
DATA
Subject: Test Email

This is a test email.
.
QUIT

Output (Server Responses):
S: 220 Simple SMTP Server
C: HELO example.com
S: 250 Hello example.com
C: MAIL FROM:<sender@example.com>
S: 250 OK
C: RCPT TO:<recipient@example.com>
S: 250 OK
C: DATA
S: 354 End data with <CR><LF>.<CR><LF>
Email Data:
Subject: Test Email

This is a test email.

S: 250 OK: Message accepted
C: QUIT
S: 221 Bye

*/

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

class SMTPServer {
private:
    io_context ioContext;
    tcp::acceptor acceptor;

    // Function to handle a single client connection
    void handleClient(tcp::socket socket) {
        try {
            // Send initial SMTP greeting
            string greeting = "220 Simple SMTP Server\r\n";
            socket.send(buffer(greeting));
            cout << "S: " << greeting;

            boost::asio::streambuf buffer;
            istream inputStream(&buffer);

            while (true) {
                // Read client command
                read_until(socket, buffer, "\r\n");
                string command;
                getline(inputStream, command);
                if (!command.empty() && command.back() == '\r') {
                    command.pop_back(); // Remove trailing '\r'
                }
                cout << "C: " << command << endl;

                // Process client command
                if (command.substr(0, 4) == "HELO") {
                    string response = "250 Hello " + command.substr(5) + "\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                } else if (command.substr(0, 4) == "MAIL") {
                    string response = "250 OK\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                } else if (command.substr(0, 4) == "RCPT") {
                    string response = "250 OK\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                } else if (command.substr(0, 4) == "DATA") {
                    string response = "354 End data with <CR><LF>.<CR><LF>\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;

                    // Read email data
                    string emailData;
                    while (true) {
                        read_until(socket, buffer, "\r\n");
                        string line;
                        getline(inputStream, line);
                        if (!line.empty() && line.back() == '\r') {
                            line.pop_back(); // Remove trailing '\r'
                        }
                        if (line == ".") break; // End of email data
                        emailData += line + "\n";
                    }
                    cout << "Email Data:\n" << emailData;

                    response = "250 OK: Message accepted\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                } else if (command == "QUIT") {
                    string response = "221 Bye\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                    break;
                } else {
                    string response = "500 Command not recognized\r\n";
                    socket.send(buffer(response));
                    cout << "S: " << response;
                }
            }
        } catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

public:
    SMTPServer(short port)
        : acceptor(ioContext, tcp::endpoint(tcp::v4(), port)) {}

    void start() {
        cout << "SMTP Server is running on port " << acceptor.local_endpoint().port() << endl;

        while (true) {
            tcp::socket socket(ioContext);
            acceptor.accept(socket);
            cout << "New client connected." << endl;

            // Handle client in a separate thread
            thread(&SMTPServer::handleClient, this, move(socket)).detach();
        }
    }
};

int main() {
    try {
        short port = 2525; // Use port 2525 for testing (standard SMTP port is 25)
        SMTPServer server(port);
        server.start();
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
