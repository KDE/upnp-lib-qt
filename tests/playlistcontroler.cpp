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

#include "playlistcontroler.h"

#include <QtCore/QDebug>

PlayListControler::PlayListControler(QObject *parent)
    : QObject(parent), mPlayListModel(nullptr), mCurrentTrack(), mUrlRole(Qt::DisplayRole),
      mIsPlayingRole(Qt::DisplayRole), mPlayerState(PlayListControler::PlayerState::Stopped),
      mAudioVolume(1.0), mAudioPosition(0), mPlayControlVolume(1.0), mPlayControlPosition(0)
{
}

QUrl PlayListControler::playerSource() const
{
    if (!mPlayListModel) {
        return QUrl();
    }

    return mPlayListModel->data(mCurrentTrack, mUrlRole).toUrl();
}

bool PlayListControler::playControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return mPlayListModel->rowCount() > 0;
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) > 0;
}

bool PlayListControler::skipBackwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mCurrentTrack.row() > 0;
}

bool PlayListControler::skipForwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mCurrentTrack.row() < mPlayListModel->rowCount(mCurrentTrack.parent()) - 1;
}

bool PlayListControler::musicPlaying() const
{
    return mPlayerState == PlayListControler::PlayerState::Playing;
}

void PlayListControler::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel);
    }

    mPlayListModel = aPlayListModel;

    connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &PlayListControler::tracksInserted);
    connect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &PlayListControler::tracksMoved);
    connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &PlayListControler::tracksRemoved);
    connect(mPlayListModel, &QAbstractItemModel::modelReset, this, &PlayListControler::playListReset);
    connect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &PlayListControler::playListLayoutChanged);

    Q_EMIT playListModelChanged();
    playListReset();
}

QAbstractItemModel *PlayListControler::playListModel() const
{
    return mPlayListModel;
}

void PlayListControler::setUrlRole(int value)
{
    mUrlRole = value;
    Q_EMIT urlRoleChanged();
}

int PlayListControler::urlRole() const
{
    return mUrlRole;
}

void PlayListControler::setIsPlayingRole(int value)
{
    mIsPlayingRole = value;
    Q_EMIT isPlayingRoleChanged();
}

int PlayListControler::isPlayingRole() const
{
    return mIsPlayingRole;
}

void PlayListControler::setAudioVolume(double value)
{
    mAudioVolume = value;
    Q_EMIT audioVolumeChanged();
}

double PlayListControler::audioVolume() const
{
    return mAudioVolume;
}

void PlayListControler::setAudioPosition(int value)
{
    mAudioPosition = value;
    Q_EMIT audioPositionChanged();
}

int PlayListControler::audioPosition() const
{
    return mAudioPosition;
}

void PlayListControler::setAudioDuration(int value)
{
    mAudioDuration = value;
    Q_EMIT audioDurationChanged();
}

int PlayListControler::audioDuration() const
{
    return mAudioDuration;
}

void PlayListControler::setPlayControlVolume(double value)
{
    mPlayControlVolume = value;
    Q_EMIT playControlVolumeChanged();
}

double PlayListControler::playControlVolume() const
{
    return mPlayControlVolume;
}

void PlayListControler::setPlayControlPosition(int value)
{
    mPlayControlPosition = value;
    Q_EMIT playControlPositionChanged();
}

int PlayListControler::playControlPosition() const
{
    return mPlayControlPosition;
}

void PlayListControler::playListReset()
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
        Q_EMIT playerSourceChanged();
    }

    Q_EMIT playControlEnabledChanged();
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
}

void PlayListControler::tracksInserted(const QModelIndex &parent, int first, int last)
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
        Q_EMIT playerSourceChanged();
    }

    Q_EMIT playControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
}

void PlayListControler::tracksRemoved(const QModelIndex &parent, int first, int last)
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
        Q_EMIT playerSourceChanged();
    }

    Q_EMIT playControlEnabledChanged();
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::playerPaused()
{
    mPlayerState = PlayListControler::PlayerState::Paused;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::playerPlaying()
{
    mPlayerState = PlayListControler::PlayerState::Playing;
    mPlayListModel->setData(mCurrentTrack, true, mIsPlayingRole);
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::playerStopped()
{
    mPlayListModel->setData(mCurrentTrack, false, mIsPlayingRole);
    mPlayerState = PlayListControler::PlayerState::Stopped;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::skipNextTrack()
{
    stopPlayer();
    gotoNextTrack();
}

void PlayListControler::skipPreviousTrack()
{
    stopPlayer();

    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() <= 0) {
        stopPlayer();
    }

    mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() - 1, mCurrentTrack.column(), mCurrentTrack.parent());
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
    Q_EMIT playerSourceChanged();
    startPlayer();
}

void PlayListControler::playPause()
{
    switch(mPlayerState)
    {
    case PlayerState::Stopped:
        startPlayer();
        break;
    case PlayerState::Playing:
        pausePlayer();
        break;
    case PlayerState::Paused:
        startPlayer();
        break;
    }
}

void PlayListControler::playerSeek(int position)
{
    mAudioPosition = position;
    Q_EMIT audioPositionChanged();
    mPlayControlPosition = position;
    Q_EMIT playControlPositionChanged();
}

void PlayListControler::audioPlayerPositionChanged(int position)
{
    mRealAudioPosition = position;
    mAudioPosition = position;
    mPlayControlPosition = mRealAudioPosition;
    Q_EMIT playControlPositionChanged();
}

void PlayListControler::audioPlayerFinished(bool finished)
{
    if (finished) {
        gotoNextTrack();
    }
}

void PlayListControler::startPlayer()
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
        Q_EMIT skipBackwardControlEnabledChanged();
        Q_EMIT skipForwardControlEnabledChanged();
        Q_EMIT playerSourceChanged();
    }

    Q_EMIT playMusic();
}

void PlayListControler::pausePlayer()
{
    Q_EMIT pauseMusic();
}

void PlayListControler::stopPlayer()
{
    Q_EMIT stopMusic();
}

void PlayListControler::gotoNextTrack()
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() >= mPlayListModel->rowCount(mCurrentTrack.parent()) - 1) {
        mCurrentTrack = mPlayListModel->index(0, mCurrentTrack.column(), mCurrentTrack.parent());
        Q_EMIT skipBackwardControlEnabledChanged();
        Q_EMIT skipForwardControlEnabledChanged();
        Q_EMIT playerSourceChanged();
        stopPlayer();
        return;
    }

    mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() + 1, mCurrentTrack.column(), mCurrentTrack.parent());
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
    Q_EMIT playerSourceChanged();
    startPlayer();
}


#include "moc_playlistcontroler.cpp"
