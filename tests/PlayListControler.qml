import QtQuick 2.0
import QtQml.Models 2.1
import QtQml.StateMachine 1.0
import QtMultimedia 5.4
import org.mgallien.QmlExtension 1.0

Item {
    property int playListPosition
    property DelegateModel playListModel
    property var modelData
    property Audio player
    property var playControl

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
                targetState: currentTrackFinished
                signal: player.stopped
            }

            SignalTransition {
                targetState: nextTrack
                signal: playControl.playNext
            }

            SignalTransition {
                targetState: previousTrack
                signal: playControl.playPrevious
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
                guard: playListPosition === modelData.trackCount && playListPosition === -1
            }
        }

        State {
            id: currentTrackFinished

            onEntered: {
                playListModel.model.finishedPlaying(playListModel.modelIndex(playListPosition))
                playListPosition = playListPosition + 1
            }

            SignalTransition {
                targetState: trackFinished
                signal: currentTrackFinished.entered
            }
        }

        State {
            id: nextTrack

            onEntered: {
                player.stop()
                playListModel.model.finishedPlaying(playListModel.modelIndex(playListPosition))
                playListPosition = playListPosition + 1
            }

            SignalTransition {
                targetState: trackFinished
                signal: player.stopped
                guard: playListPosition !== modelData.trackCount
            }

            SignalTransition {
                targetState: ready
                signal: player.stopped
                guard: playListPosition === modelData.trackCount
            }
        }

        State {
            id: previousTrack

            onEntered: {
                player.stop()
                playListModel.model.finishedPlaying(playListModel.modelIndex(playListPosition))
                playListPosition = playListPosition - 1
            }

            SignalTransition {
                targetState: trackFinished
                signal: player.stopped
                guard: playListPosition !== -1
            }

            SignalTransition {
                targetState: ready
                signal: player.stopped
                guard: playListPosition === -1
            }
        }
    }
}

