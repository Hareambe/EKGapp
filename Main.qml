import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 1400
    height: 600

    Column {
        spacing: 20
        anchors.centerIn: parent

        Text {
            text: "First Lead ECG Tracing"
            font.pointSize: 20
            horizontalAlignment: Text.AlignHCenter
        }

        Canvas {
            width: parent.width - 40
            height: 300

            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);

                ctx.strokeStyle = "blue";
                ctx.beginPath();

                if (graphData.length > 0) {
                    var scaleX = width / graphData.length;
                    var scaleY = height / 2000; // Adjust for voltage scaling

                    ctx.moveTo(0, height / 2 - graphData[0] * scaleY);
                    for (var i = 1; i < graphData.length; i++) {
                        ctx.lineTo(i * scaleX, height / 2 - graphData[i] * scaleY);
                    }
                }

                ctx.stroke();
            }
        }
    }
}
