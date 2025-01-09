#include "DatasetModel.h"

DatasetModel::DatasetModel(QObject *parent)
    : QAbstractListModel(parent) {}

void DatasetModel::setDataset(const std::vector<std::vector<double>> &data) {
    beginResetModel();
    m_data = data;
    endResetModel();
}

int DatasetModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_data.size()); // Number of rows
}

QVariant DatasetModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    int row = index.row();
    QVariantList rowData;
    for (const auto& value : m_data[row]) {
        rowData.append(value * 1000.0); // Convert to millivolts
    }
    return rowData; // Return a row of data
}
