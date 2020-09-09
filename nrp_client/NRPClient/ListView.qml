import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0

ListView {
	id: listView
	width: 120
	height: 160
	model: ListModel {
		ListElement {
			name: "Grey"
			colorCode: "grey"
		}
		
		ListElement {
			name: "Red"
			colorCode: "red"
		}
		
		ListElement {
			name: "Blue"
			colorCode: "blue"
		}
		
		ListElement {
			name: "Green"
			colorCode: "green"
		}
	}
	delegate: Item {
		x: 0
		width: 72
		height: 20
		Row {
			id: row1
			spacing: 10
			
			Text {
				text: name
				anchors.right: parent.left
				anchors.rightMargin: -60
				anchors.top: parent.top
				anchors.topMargin: 0
				font.bold: true
			}
		}
	}
}
