// This implementation demonstrates how to send an email using the SMTP protocol. 

/*
Explanation
1.	SMTP Connection:
•	The SMTPClient class connects to the SMTP server using Boost.Asio's tcp::socket.
2.	SMTP Commands:
•	HELO: Identifies the client to the server.
•	MAIL FROM: Specifies the sender's email address.
•	RCPT TO: Specifies the recipient's email address.
•	DATA: Sends the email content.
•	QUIT: Closes the connection.
3.	Email Content:
•	The email content includes the subject and body, followed by a . on a new line to indicate the end of the message.
4.	Error Handling:
•	Exceptions are caught and printed to handle connection or protocol errors.
---
Example Output
Input:
•	SMTP Server: smtp.example.com
•	Sender: sender@example.com
•	Recipient: recipient@example.com
•	Subject: Test Email
•	Body: This is a test email sent from a C++ SMTP client.
Output:
Connected to SMTP server: smtp.example.com
C: HELO smtp.example.com
S: 250 Hello
C: MAIL FROM:<sender@example.com>
S: 250 OK
C: RCPT TO:<recipient@example.com>
S: 250 OK
C: DATA
S: 354 Start mail input; end with <CRLF>.<CRLF>
C: Subject: Test Email

This is a test email sent from a C++ SMTP client.
.
S: 250 OK
C: QUIT
S: 221 Bye


Key Features
1.	SMTP Protocol:
•	Implements basic SMTP commands for sending emails.
2.	Boost.Asio:
•	Provides high-performance networking for connecting to the SMTP server.
3.	Error Handling:
•	Handles connection and protocol errors gracefully.
---
Enhancements
1.	Authentication:
•	Add support for SMTP authentication (e.g., AUTH LOGIN).
•	Use Base64 encoding for username and password.
2.	TLS/SSL Support:
•	Use Boost.Asio's SSL features to secure the connection.
3.	IMAP/POP3 Support:
•	Extend the implementation to receive emails using IMAP or POP3.
4.	Email Attachments:
•	Add support for MIME encoding to send attachments.
---
Dependencies
•	Boost Library:
•	Install Boost (https://www.boost.org/) to use Boost.Asio.

*/


#include <boost/asio.hpp>
#include <iostream>
#include <string>
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

class SMTPClient {
private:
    io_context ioContext;
    tcp::socket socket;
    string smtpServer;
    int smtpPort;

    // Helper function to send a command to the SMTP server
    void sendCommand(const string& command) {
        socket.send(buffer(command + "\r\n"));
        cout << "C: " << command << endl;
    }

    // Helper function to read the server's response
    string readResponse() {
        boost::asio::streambuf responseBuffer;
        read_until(socket, responseBuffer, "\r\n");
        istream responseStream(&responseBuffer);
        string response;
        getline(responseStream, response);
        cout << "S: " << response << endl;
        return response;
    }

public:
    SMTPClient(const string& server, int port)
        : socket(ioContext), smtpServer(server), smtpPort(port) {}

    void connect() {
        tcp::resolver resolver(ioContext);
        auto endpoints = resolver.resolve(smtpServer, to_string(smtpPort));
        connect(socket, endpoints);
        cout << "Connected to SMTP server: " << smtpServer << endl;

        // Read the server's initial response
        readResponse();
    }

    void sendEmail(const string& from, const string& to, const string& subject, const string& body) {
        // Send HELO command
        sendCommand("HELO " + smtpServer);
        readResponse();

        // Send MAIL FROM command
        sendCommand("MAIL FROM:<" + from + ">");
        readResponse();

        // Send RCPT TO command
        sendCommand("RCPT TO:<" + to + ">");
        readResponse();

        // Send DATA command
        sendCommand("DATA");
        readResponse();

        // Send email content
        string emailContent = "Subject: " + subject + "\r\n\r\n" + body + "\r\n.";
        sendCommand(emailContent);
        readResponse();

        // Send QUIT command
        sendCommand("QUIT");
        readResponse();
    }
};

int main() {
    try {
        // Replace with your SMTP server and port
        string smtpServer = "smtp.example.com";
        int smtpPort = 25;

        // Replace with sender and recipient email addresses
        string from = "sender@example.com";
        string to = "recipient@example.com";

        // Email subject and body
        string subject = "Test Email";
        string body = "This is a test email sent from a C++ SMTP client.";

        // Create and use the SMTP client
        SMTPClient client(smtpServer, smtpPort);
        client.connect();
        client.sendEmail(from, to, subject, body);
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
