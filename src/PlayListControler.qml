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
                player.play()
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
                guard: modelData.trackCount > 0 && playListPosition < modelData.trackCount - 1
            }

            SignalTransition {
                targetState: pause
                signal: player.stopped
                guard: playListPosition === modelData.trackCount - 1
            }
        }

        State {
            id: trackFinished

            onEntered: {
                console.log('entering track finished state')
                playListPosition = playListPosition + 1
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

