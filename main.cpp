#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QStandardPaths>
#include <QVariantList>
#include <H5File.hpp>
#include <limits>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Path to the HDF5 file
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/first_patient_data.hdf5";
    qDebug() << "[DEBUG] File path:" << filePath;

    QVariantList graphData; // To hold the first lead data for QML
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    try {
        qDebug() << "[DEBUG] Attempting to open HDF5 file.";
        HighFive::File file(filePath.toStdString(), HighFive::File::ReadOnly);
        qDebug() << "[DEBUG] Successfully opened HDF5 file.";

        // List all groups/datasets in the file
        auto groups = file.listObjectNames();
        qDebug() << "[DEBUG] Groups/Datasets in the file:";
        for (const auto& group : groups) {
            qDebug() << "  -" << QString::fromStdString(group);
        }

        // Check for the "first_patient" dataset
        if (!file.exist("first_patient")) {
            qDebug() << "[ERROR] Dataset 'first_patient' not found in the file.";
            return -1;
        }

        HighFive::DataSet dataset = file.getDataSet("first_patient");
        qDebug() << "[DEBUG] Successfully accessed 'first_patient' dataset.";

        // Get dataset dimensions
        std::vector<size_t> dims = dataset.getDimensions();
        if (dims.size() != 2 || dims[1] != 12) {
            qDebug() << "[ERROR] Unexpected dataset dimensions. Expected [4096, 12] but got:" << dims;
            return -1;
        }
        qDebug() << "[DEBUG] Dataset dimensions:" << dims[0] << "x" << dims[1];

        // Read the entire dataset (2D: 4096 samples × 12 leads)
        std::vector<std::vector<double>> fullData(dims[0], std::vector<double>(dims[1]));
        dataset.read(fullData);

        qDebug() << "[DEBUG] Successfully read full dataset.";

        // Extract the first lead (column 0) and keep track of min and max
        for (size_t i = 0; i < dims[0]; ++i) {
            double valueInMillivolts = fullData[i][0] * 1000.0; // Convert to millivolts
            graphData.append(valueInMillivolts);

            // Update min and max values
            if (valueInMillivolts < minValue) {
                minValue = valueInMillivolts;
            }
            if (valueInMillivolts > maxValue) {
                maxValue = valueInMillivolts;
            }
        }

        qDebug() << "[DEBUG] Successfully processed first lead data.";
        qDebug() << "[DEBUG] Graph data size:" << graphData.size();
        qDebug() << "[DEBUG] Minimum value (mV):" << minValue;
        qDebug() << "[DEBUG] Maximum value (mV):" << maxValue;

    } catch (const HighFive::Exception& e) {
        qDebug() << "[ERROR] HighFive Exception:" << e.what();
        return -1;
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Standard Exception:" << e.what();
        return -1;
    }

    // Expose graphData to QML
    engine.rootContext()->setContextProperty("graphData", graphData);

    // Load QML interface
    qDebug() << "[DEBUG] Loading QML interface.";
    engine.loadFromModule("demos.calqlatr", "Main");

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "[ERROR] Failed to load QML interface.";
        return -1;
    }

    qDebug() << "[DEBUG] Application is running.";
    return app.exec();
}
