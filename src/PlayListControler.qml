import QtQuick 2.0
import QtQml.Models 2.1
import QtQml.StateMachine 1.0
import QtMultimedia 5.4
import org.mgallien.QmlExtension 1.0

Item {
    property int playListPosition
    property DelegateModel playListModel
    property MediaPlayList modelData
    property Audio player
    property MediaPlayerControl playControl

    StateMachine {
        id: controlerState
        initialState: empty
        running: true

        State {
            id: empty

            SignalTransition {
                targetState: ready
                signal: modelData.trackCountChanged
                guard: modelData.trackCount > 0
            }
        }

        State {
            id: ready

            onEntered: {
                playListPosition = 0
                player.pause()
                playControl.isPlaying = false
            }

            SignalTransition {
                targetState: empty
                signal: modelData.trackCountChanged
                guard: modelData.trackCount === 0 || playListPosition === modelData.trackCount
            }

            SignalTransition {
                targetState: initializePlay
                signal: playControl.play
                guard: modelData.trackCount > 0
            }
        }

        State {
            id: initializePlay

            onEntered: {
                player.source = playListModel.model.getUrl(playListModel.modelIndex(playListPosition))
                playListModel.model.startPlaying(playListModel.modelIndex(playListPosition))
            }

            SignalTransition {
                targetState: playing
                signal: initializePlay.entered
            }
        }

        State {
            id: playing

            onEntered: {
                player.play()
                playControl.isPlaying = true
            }

            SignalTransition {
                targetState: empty
                signal: modelData.trackCountChanged
                guard: modelData.trackCount === 0 || playListPosition === modelData.trackCount
            }

            SignalTransition {
                targetState: pause
                signal: playControl.pause
                guard: modelData.trackCount > 0
            }

            SignalTransition {
                targetState: trackFinished
                signal: player.stopped
            }

            SignalTransition {
                targetState: nextTrack
                signal: playControl.playNext
            }
        }

        State {
            id: pause

            onEntered: {
                player.pause()
                playControl.isPlaying = false
            }

            SignalTransition {
                targetState: empty
                signal: modelData.trackCountChanged
                guard: modelData.trackCount === 0 || playListPosition === modelData.trackCount
            }

            SignalTransition {
                targetState: playing
                signal: playControl.play
                guard: modelData.trackCount > 0
            }
        }

        State {
            id: trackFinished

            onEntered: {
                playListModel.model.finishedPlaying(playListModel.modelIndex(playListPosition))
                playListPosition = playListPosition + 1
                playControl.isPlaying = false
            }

            SignalTransition {
                targetState: initializePlay
                signal: trackFinished.entered
                guard: playListPosition < modelData.trackCount
            }

            SignalTransition {
                targetState: ready
                signal: trackFinished.entered
                guard: playListPosition === modelData.trackCount
            }
        }

        State {
            id: nextTrack

            onEntered: {
                player.stop()
            }

            SignalTransition {
                targetState: trackFinished
                signal: player.stopped
            }
        }
    }
}

