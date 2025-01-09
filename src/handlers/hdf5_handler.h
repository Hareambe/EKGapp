#ifndef HDF5_HANDLER_H
#define HDF5_HANDLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <android/include/hdf5/hdf5.h>
#include <string>

class HDF5Handler : public QObject {
    Q_OBJECT
public:
    explicit HDF5Handler(const std::string& filename, QObject* parent = nullptr);
    ~HDF5Handler();

    Q_INVOKABLE QStringList listGroups();
    Q_INVOKABLE size_t getDatasetSize(const QString& datasetName);
    Q_INVOKABLE QVector<QVector<double>> readDataset(const QString& datasetName);

private:
    hid_t file;  // Handle to the HDF5 file
};

#endif // HDF5_HANDLER_H
