#include "hdf5_handler.h"
#include <stdexcept>
#include <QDebug>

HDF5Handler::HDF5Handler(const std::string &filename, QObject* parent)
    : QObject(parent), file(H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT)) {
    if (file < 0) {
        throw std::runtime_error("Failed to open HDF5 file");
    }
}

HDF5Handler::~HDF5Handler() {
    if (file >= 0) {
        H5Fclose(file);
    }
}

QStringList HDF5Handler::listGroups() {
    QStringList groups;

    H5G_info_t groupInfo;
    if (H5Gget_info(file, &groupInfo) >= 0) {
        for (hsize_t i = 0; i < groupInfo.nlinks; i++) {
            char groupName[256];
            ssize_t nameSize = H5Lget_name_by_idx(file, ".", H5_INDEX_NAME, H5_ITER_INC, i, groupName, sizeof(groupName), H5P_DEFAULT);
            if (nameSize >= 0) {
                groups << QString(groupName);
            }
        }
    } else {
        throw std::runtime_error("Failed to retrieve group information");
    }

    return groups;
}

size_t HDF5Handler::getDatasetSize(const QString &datasetName) {
    hid_t dataset = H5Dopen(file, datasetName.toStdString().c_str(), H5P_DEFAULT);
    if (dataset < 0) {
        throw std::runtime_error("Failed to open dataset");
    }

    hid_t space = H5Dget_space(dataset);
    if (space < 0) {
        H5Dclose(dataset);
        throw std::runtime_error("Failed to get dataset space");
    }

    // Retrieve dataset dimensions
    hsize_t dims[1];
    int ndims = H5Sget_simple_extent_dims(space, dims, nullptr);
    if (ndims != 1) { // Adjust this for multidimensional datasets if needed
        H5Sclose(space);
        H5Dclose(dataset);
        throw std::runtime_error("Unexpected dataset dimensions");
    }

    size_t size = static_cast<size_t>(dims[0]);

    H5Sclose(space);
    H5Dclose(dataset);

    return size;
}

QVector<QVector<double>> HDF5Handler::readDataset(const QString &datasetName) {
    hid_t dataset = H5Dopen(file, datasetName.toStdString().c_str(), H5P_DEFAULT);
    if (dataset < 0) {
        throw std::runtime_error("Failed to open dataset");
    }

    hid_t space = H5Dget_space(dataset);
    if (space < 0) {
        H5Dclose(dataset);
        throw std::runtime_error("Failed to get dataset space");
    }

    // Validate dataset dimensions
    hsize_t dims[2];
    int ndims = H5Sget_simple_extent_dims(space, dims, nullptr);
    if (ndims != 2) {
        H5Sclose(space);
        H5Dclose(dataset);
        throw std::runtime_error("Unexpected dataset dimensions");
    }

    QVector<QVector<double>> data(dims[0], QVector<double>(dims[1]));

    // Read the dataset
    std::vector<double> buffer(dims[0] * dims[1]);
    if (H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer.data()) < 0) {
        H5Sclose(space);
        H5Dclose(dataset);
        throw std::runtime_error("Failed to read dataset");
    }

    // Scale and populate QVector
    for (hsize_t i = 0; i < dims[0]; i++) {
        for (hsize_t j = 0; j < dims[1]; j++) {
            data[i][j] = buffer[i * dims[1] + j] * 1000.0; // Convert to millivolts
        }
    }

    H5Sclose(space);
    H5Dclose(dataset);

    return data;
}
