import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
Item
{
    readonly property int thisPageNO: 120
    id:ptConfig
    anchors.fill:parent

    TableView {
        id: tableView
        x: 0
        y: 231
        width: 640
        height: 249
        TableViewColumn {
                 role: "title"
                 title: "Title"
                 width: 100
             }
             TableViewColumn {
                 role: "author"
                 title: "Author"
                 width: 200
             }
             model: libraryModel
             ListModel {
                   id: libraryModel
                   ListElement {
                       title: "A Masterpiece"
                       author: "Gabriel"
                   }
                   ListElement {
                       title: "Brilliance"
                       author: "Jens"
                   }
                   ListElement {
                       title: "Outstanding"
                       author: "Frederik"
                   }
               }
    }

}
