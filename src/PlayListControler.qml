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

            onEntered: {
                console.log('entering empty state')
            }

            SignalTransition {
                targetState: pause
                signal: modelData.trackCountChanged
                guard: modelData.trackCount > 0
            }
        }

        State {
            id: pause

            onEntered: {
                console.log('entering pause state')
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
                targetState: playing
                signal: playControl.play
                guard: modelData.trackCount > 0
            }
        }

        State {
            id: playing

            onEntered: {
                console.log('entering playing state')
                player.source = playListModel.model.getUrl(playListModel.modelIndex(playListPosition))
                playListModel.model.startPlaying(playListModel.modelIndex(playListPosition))
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
        }

        State {
            id: trackFinished

            onEntered: {
                console.log('entering track finished state')
                playListModel.model.finishedPlaying(playListModel.modelIndex(playListPosition))
                playListPosition = playListPosition + 1
                playControl.isPlaying = false
            }

            SignalTransition {
                targetState: playing
                signal: trackFinished.entered
                guard: playListPosition < modelData.trackCount
            }

            SignalTransition {
                targetState: pause
                signal: trackFinished.entered
                guard: playListPosition === modelData.trackCount
            }
        }
    }
}

