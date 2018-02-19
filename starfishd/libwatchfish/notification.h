/*
 *  libwatchfish - library with common functionality for SailfishOS smartwatch connector programs.
 *  Copyright (C) 2015 Javier S. Pedro <dev.git@javispedro.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WATCHFISH_NOTIFICATION_H
#define WATCHFISH_NOTIFICATION_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

namespace watchfish
{

class NotificationPrivate;

class Notification : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Notification)

	/** Notification ID */
	Q_PROPERTY(uint id READ id CONSTANT)
	/** Binary/harbour name of sending program */
	Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
	/** Human readable name of sender program */
	Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY appNameChanged)
	Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
	Q_PROPERTY(QString body READ body WRITE setBody NOTIFY bodyChanged)
	Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
	/** Icon file path */
	Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
	/** Category ID */
	Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
	Q_PROPERTY(int urgency READ urgency WRITE setUrgency NOTIFY urgencyChanged)
	Q_PROPERTY(bool transient READ transient WRITE setTransient NOTIFY transientChanged)

	/* Nemo stuff */
	Q_PROPERTY(QString previewSummary READ previewSummary WRITE setPreviewSummary NOTIFY previewSummaryChanged)
	Q_PROPERTY(QString previewBody READ previewBody WRITE setPreviewBody NOTIFY previewBodyChanged)

	Q_PROPERTY(QStringList actions READ actions NOTIFY actionsChanged)

	Q_ENUMS(CloseReason)

public:
	explicit Notification(uint id, QObject *parent = 0);
	~Notification();

	enum CloseReason {
		Expired = 1,
		DismissedByUser = 2,
		DismissedByProgram = 3,
		ClosedOther = 4
	};

	uint id() const;

	QString appId() const;
	void setAppId(const QString &appId);

	QString appName() const;
	void setAppName(const QString &appName);

	QString summary() const;
	void setSummary(const QString &summary);

	QString body() const;
	void setBody(const QString &body);

	QDateTime timestamp() const;
	void setTimestamp(const QDateTime &dt);

	QString icon() const;
	void setIcon(const QString &icon);

	QString category() const;
	void setCategory(const QString &category);

	int urgency() const;
	void setUrgency(int urgency);

	bool transient() const;
	void setTransient(bool transient);

	QString previewSummary() const;
	void setPreviewSummary(const QString &summary);

	QString previewBody() const;
	void setPreviewBody(const QString &body);

	QStringList actions() const;
	void addDBusAction(const QString &action, const QString &service, const QString &path, const QString &iface, const QString &method, const QStringList &args = QStringList());

public slots:
	void invokeAction(const QString &action);
	void close();

signals:
	void appIdChanged();
	void appNameChanged();
	void summaryChanged();
	void bodyChanged();
	void timestampChanged();
	void iconChanged();
	void categoryChanged();
	void urgencyChanged();
	void transientChanged();

	void previewSummaryChanged();
	void previewBodyChanged();

	void actionsChanged();

	void closed(CloseReason reason);

private:
	NotificationPrivate * const d_ptr;
};

}

#endif // WATCHFISH_NOTIFICATION_H
