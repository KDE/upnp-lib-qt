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
    : QObject(parent), mPlayListModel(nullptr), mCurrentTrack(), mUrlRole(Qt::DisplayRole), mPlayerState(PlayListControler::PlayerState::Stopped)
{
}

QUrl PlayListControler::playerSource() const
{
    if (!mPlayListModel) {
        return QUrl();
    }

    qDebug() << "PlayListControler::playerSource" << mCurrentTrack << mUrlRole << mPlayListModel->data(mCurrentTrack, mUrlRole).toUrl();

    return mPlayListModel->data(mCurrentTrack, mUrlRole).toUrl();
}

bool PlayListControler::playControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        mPlayListModel->rowCount();
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) > 0;
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

void PlayListControler::playListReset()
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
    }

    Q_EMIT playControlEnabledChanged();
}

void PlayListControler::playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
}

void PlayListControler::tracksInserted(const QModelIndex &parent, int first, int last)
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
    }

    Q_EMIT playControlEnabledChanged();
}

void PlayListControler::tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
}

void PlayListControler::tracksRemoved(const QModelIndex &parent, int first, int last)
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
    }

    Q_EMIT playControlEnabledChanged();
}

void PlayListControler::playerPaused()
{
    mPlayerState = PlayListControler::PlayerState::Paused;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::playerPlaying()
{
    mPlayerState = PlayListControler::PlayerState::Playing;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::playerStopped()
{
    mPlayerState = PlayListControler::PlayerState::Stopped;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::skipNextTrack()
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() >= mPlayListModel->rowCount(mCurrentTrack.parent())) {
        stopPlayer();
    }

    mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() + 1, mCurrentTrack.column(), mCurrentTrack.parent());
    currentTrackChanged();
}

void PlayListControler::skipPreviousTrack()
{
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
    currentTrackChanged();
}

void PlayListControler::playPause()
{
    qDebug() << "PlayListControler::playPause" << mPlayerState;
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

void PlayListControler::startPlayer()
{
    if (!mCurrentTrack.isValid()) {
        mCurrentTrack = mPlayListModel->index(0, 0);
    }

    Q_EMIT playerSourceChanged();
    Q_EMIT playMusic();
}

void PlayListControler::pausePlayer()
{
    Q_EMIT pauseMusic();
}

void PlayListControler::stopPlayer()
{
    Q_EMIT pauseMusic();
}

void PlayListControler::currentTrackChanged()
{
    Q_EMIT playMusic();
    Q_EMIT playerSourceChanged();
}

#include "moc_playlistcontroler.cpp"
