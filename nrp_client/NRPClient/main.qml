import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0

Window {
	id: window
	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")


	ToolButton {
		id: toolButton
		x: 0
		y: 0
		text: qsTr("Tool Button")
	}

	ToolBar {
		id: toolBar
		x: 0
		y: 0
		width: 640
		height: 40

		ComboBox {
			id: comboBox
			x: 106
			y: 0
			enabled: true
			textRole: qsTr("")

			ListView {
				id: listView
				anchors.left: parent.left
				anchors.leftMargin: 0
				anchors.top: parent.bottom
				anchors.topMargin: 0
			}
		}
	}
}
