#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include <QString>
#include <QStringList>
#include <QDebug>

class CommandParser
{
public:
    enum Commands
    {
        CMD_UNKNOWN = 0,
        CMD_REGISTER,
        CMD_LOGIN,
        CMD_AUTH,
        CMD_FORGOT_PASSWORD,
        CMD_HELP,
        CMD_CALCULATE,
    };

    Commands stringToCommand(const QString& cmd) {
        if (cmd == "register" || cmd == "reg") return CMD_REGISTER;
        if (cmd == "login") return CMD_LOGIN;
        if (cmd == "auth") return CMD_AUTH;
        if (cmd == "forgot" || cmd == "forgot_password") return CMD_FORGOT_PASSWORD;
        if (cmd == "help" || cmd == "?") return CMD_HELP;
        if (cmd == "calculate") return CMD_CALCULATE;
        return CMD_UNKNOWN;
    }


    QString commandToString(Commands cmd) {
        switch(cmd) {
        case CMD_REGISTER:          return "REGISTER";
        case CMD_LOGIN:             return "LOGIN";
        case CMD_AUTH:              return "AUTH";
        case CMD_FORGOT_PASSWORD:   return "FORGOT_PASSWORD";
        case CMD_HELP:              return "HELP";
        case CMD_CALCULATE:         return "CALCULATE";
        default:                    return "UNKNOWN";
        }
    }

    QString cmdSequenceToString(const QString& sequence) {
        QString result;

        for (int i=0; i<sequence.length()   ; i++) {

            QChar c = sequence[i];

            if (c == '\b') {
                if (!result.isEmpty()) {
                    result.chop(1);
                }
            } else {
                result.append(c);
            }
        }


        return result;
    }

    struct ParsedCommand
    {
        Commands command;
        QStringList params;

        bool is_valid;
        QString error;

        ParsedCommand() : command(CMD_UNKNOWN), is_valid(false) {}
    };

    ParsedCommand parse(const QString& input)
    {
        ParsedCommand result;

        QString data = cmdSequenceToString(input).trimmed();
        if (data.isEmpty())
        {
            result.error = "Empty message";
            return result;
        }

        QStringList parts = data.split("||");
        if (parts.isEmpty())
        {
            result.error = "No data";
            return result;
        }

        QString cmdStr = parts[0].toLower();
        result.command = stringToCommand(cmdStr);

        if (result.command == CMD_UNKNOWN)
        {
            result.error = "Unknown command: " + cmdStr;
            return result;
        }

        for (int i = 1; i < parts.size(); i++)
        {
            result.params.append(parts[i]);
        }

        if (!validateParams(result))
        {
            return result;
        }

        result.is_valid = true;
        return result;
    }

    QString getHelp()
    {
        return "Available commands:\r\n"
               "  register||login||password||email - register new user\r\n"
               "  login||login||password     - login\r\n"
               "  auth||login||password      - authenticate\r\n"
               "  forgot||email||newpassword - reset password by email\r\n"
               "  calculate||a||b||c         - calculate function with given arguments\r\n"
               "  help                       - show this help\r\n";
    }

private:
    bool validateParams(ParsedCommand& result)
    {
        switch (result.command)
        {
        case CMD_REGISTER:
        {
            if (result.params.size() != 3)
            {
                result.error = "Registration needs 3 params: login, password, email";
                return false;
            }
            if (result.params[0].length() < 3)
            {
                result.error = "Login too short (min 3 characters)";
                return false;
            }
            if (result.params[1].length() < 4)
            {
                result.error = "Password too short (min 4 characters)";
                return false;
            }
            if (!result.params[2].contains("@"))
            {
                result.error = "Invalid email";
                return false;
            }
        }
        break;

        case CMD_LOGIN:
        case CMD_AUTH:
        {
            if (result.params.size() != 2)
            {
                result.error = "Login/auth needs: login and password";
                return false;
            }
            if (result.params[0].isEmpty())
            {
                result.error = "Login cannot be empty";
                return false;
            }
            if (result.params[1].isEmpty())
            {
                result.error = "Password cannot be empty";
                return false;
            }
        }
        break;

        case CMD_FORGOT_PASSWORD:
        {
            if (result.params.size() != 2)
            {
                result.error = "Forgot password needs 2 params: email and new password";
                return false;
            }
            if (!result.params[0].contains("@"))
            {
                result.error = "Valid email is required";
                return false;
            }
            if (result.params[1].length() < 4)
            {
                result.error = "New password too short (min 4 characters)";
                return false;
            }
        }
        break;

        case CMD_HELP:
            break;

        case CMD_CALCULATE:
            if (result.params.size() != 3)
            {
                result.error = "Calculate function requires exactly 3 function arguments";
                return false;
            };

        case CMD_UNKNOWN:
            result.error = "Unknown command";
            return false;
        }
        return true;
    }
};

#endif // MESSAGE_PARSER_H
