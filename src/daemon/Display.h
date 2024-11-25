/***************************************************************************
* Copyright (c) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
* Copyright (c) 2014 Martin Bříza <mbriza@redhat.com>
* Copyright (c) 2013 Abdurrahman AVCI <abdurrahmanavci@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the
* Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/

#ifndef SDDM_DISPLAY_H
#define SDDM_DISPLAY_H

#include <QObject>
#include <QPointer>
#include <QDir>

#include "Auth.h"
#include "Session.h"

class QLocalSocket;

namespace SDDM {
    class Authenticator;
    class DisplayServer;
    class Seat;
    class SocketServer;
    class Greeter;

    class Display : public QObject {
        Q_OBJECT
        Q_DISABLE_COPY(Display)
    public:
        enum DisplayServerType {
            X11DisplayServerType,
            X11UserDisplayServerType,
            WaylandDisplayServerType
        };
        Q_ENUM(DisplayServerType)

        static DisplayServerType defaultDisplayServerType();
        explicit Display(Seat *parent, DisplayServerType serverType);
        ~Display();

        DisplayServerType displayServerType() const;
        DisplayServer *displayServer() const;

        int terminalId() const;

        const QString &name() const;

        QString sessionType() const;
        QString reuseSessionId() const { return m_reuseSessionId; }

        Seat *seat() const;

    public slots:
        bool start();
        void stop();

        void login(QLocalSocket *socket,
                   const QString &user, const QString &password,
                   const Session &session);
        void setPamResponse(const QString &password);
        void cancelPamConv();
        void displayServerStarted();

    signals:
        void stopped();
        void displayServerFailed();

        void loginSucceeded(QLocalSocket *socket);
        void loginFailed(QLocalSocket *socket, const QString &message, int result);
        void pamConvMsg(QLocalSocket *socket, const QString &message, int result);
        void pamRequest(QLocalSocket *socket, const AuthRequest * const request);

    private:
        QString findGreeterTheme() const;
        bool findSessionEntry(const QStringList &dirPaths, const QString &name) const;

        bool startAuth(const QString &user, const QString &password,
                       const Session &session);

        void startSocketServerAndGreeter();
        bool handleAutologinFailure();

        DisplayServerType m_displayServerType = X11DisplayServerType;

        bool m_started { false };
        bool m_failed { false };

        int m_terminalId = -1;
        int m_sessionTerminalId = 0;

        QString m_passPhrase;
        QString m_sessionName;
        QString m_reuseSessionId;

        Session m_autologinSession;

        Auth *m_auth { nullptr };
        DisplayServer *m_displayServer { nullptr };
        Seat *m_seat { nullptr };
        SocketServer *m_socketServer { nullptr };
        QPointer<QLocalSocket> m_socket;
        Greeter *m_greeter { nullptr };

    private slots:
        void slotRequestChanged();
        void slotAuthenticationFinished(const QString &user, bool success);
        void slotSessionStarted(bool success);
        void slotHelperFinished(AuthEnums::HelperExitStatus status);
        void slotAuthInfo(const QString &message, AuthEnums::Info info, int result);
        void slotAuthError(const QString &message, AuthEnums::Error error, int result);
        void slotGreeterStopped();
    };
}

#endif // SDDM_DISPLAY_H
