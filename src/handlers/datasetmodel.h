#ifndef DATASETMODEL_H
#define DATASETMODEL_H

#include <QAbstractListModel>
#include <vector>

class DatasetModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit DatasetModel(QObject *parent = nullptr);

    // Set the dataset
    void setDataset(const std::vector<std::vector<double>>& data);

    // Required overrides for QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<std::vector<double>> m_data; // Store the dataset
};

#endif // DATASETMODEL_H
