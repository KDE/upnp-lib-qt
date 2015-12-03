/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PLAYLISTCONTROLER_H
#define PLAYLISTCONTROLER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QUrl>

class PlayListControler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl playerSource
               READ playerSource
               NOTIFY playerSourceChanged)

    Q_PROPERTY(bool playControlEnabled
               READ playControlEnabled
               NOTIFY playControlEnabledChanged)

    Q_PROPERTY(bool skipBackwardControlEnabled
               READ skipBackwardControlEnabled
               NOTIFY skipBackwardControlEnabledChanged)

    Q_PROPERTY(bool skipForwardControlEnabled
               READ skipForwardControlEnabled
               NOTIFY skipForwardControlEnabledChanged)

    Q_PROPERTY(bool musicPlaying
               READ musicPlaying
               NOTIFY musicPlayingChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(int urlRole
               READ urlRole
               WRITE setUrlRole
               NOTIFY urlRoleChanged)

    Q_PROPERTY(int isPlayingRole
               READ isPlayingRole
               WRITE setIsPlayingRole
               NOTIFY isPlayingRoleChanged)

    Q_PROPERTY(double audioVolume
               READ audioVolume
               WRITE setAudioVolume
               NOTIFY audioVolumeChanged)

    Q_PROPERTY(int audioPosition
               READ audioPosition
               WRITE setAudioPosition
               NOTIFY audioPositionChanged)

    Q_PROPERTY(int audioDuration
               READ audioDuration
               WRITE setAudioDuration
               NOTIFY audioDurationChanged)

    Q_PROPERTY(double playControlVolume
               READ playControlVolume
               WRITE setPlayControlVolume
               NOTIFY playControlVolumeChanged)

    Q_PROPERTY(int playControlPosition
               READ playControlPosition
               WRITE setPlayControlPosition
               NOTIFY playControlPositionChanged)

public:

    enum class PlayerState
    {
        Playing,
        Paused,
        Stopped,
    };

    Q_ENUM(PlayerState)

    explicit PlayListControler(QObject *parent = 0);

    QUrl playerSource() const;

    bool playControlEnabled() const;

    bool skipBackwardControlEnabled() const;

    bool skipForwardControlEnabled() const;

    bool musicPlaying() const;

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    QAbstractItemModel* playListModel() const;

    void setUrlRole(int value);

    int urlRole() const;

    void setIsPlayingRole(int value);

    int isPlayingRole() const;

    void setAudioVolume(double value);

    double audioVolume() const;

    void setAudioPosition(int value);

    int audioPosition() const;

    void setAudioDuration(int value);

    int audioDuration() const;

    void setPlayControlVolume(double value);

    double playControlVolume() const;

    void setPlayControlPosition(int value);

    int playControlPosition() const;

Q_SIGNALS:

    void playMusic();

    void pauseMusic();

    void stopMusic();

    void playerSourceChanged();

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void musicPlayingChanged();

    void playListModelChanged();

    void urlRoleChanged();

    void isPlayingRoleChanged();

    void audioVolumeChanged();

    void audioPositionChanged();

    void audioDurationChanged();

    void playControlVolumeChanged();

    void playControlPositionChanged();

public Q_SLOTS:

    void playListReset();

    void playListLayoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(),
                               QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);

    void tracksInserted(const QModelIndex &parent, int first, int last);

    void tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    void tracksRemoved(const QModelIndex & parent, int first, int last);

    void playerPaused();

    void playerPlaying();

    void playerStopped();

    void skipNextTrack();

    void skipPreviousTrack();

    void playPause();

    void playerSeek(int position);

    void audioPlayerPositionChanged(int position);

    void audioPlayerFinished(bool finished);

private:

    void startPlayer();

    void pausePlayer();

    void stopPlayer();

    void gotoNextTrack();

    QAbstractItemModel *mPlayListModel;

    QModelIndex mCurrentTrack;

    int mUrlRole;

    int mIsPlayingRole;

    PlayerState mPlayerState;

    double mAudioVolume;

    int mAudioPosition;

    int mAudioDuration;

    int mRealAudioPosition;

    double mPlayControlVolume;

    int mPlayControlPosition;

};

#endif // PLAYLISTCONTROLER_H