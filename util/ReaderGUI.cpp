#include "ReaderGUI.h"
#include "ui_ReaderGUI.h"

ReaderGUI::ReaderGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderGUI)
{
    ui->setupUi(this);
    m_reader = new Reader();
    m_DS = new DataSet();
    m_dFlag = ui->DatasetType_comboBox->currentIndex();
    readSettings();
}

ReaderGUI::~ReaderGUI()
{
    delete ui;
    delete m_DS;
}


void ReaderGUI::countLabels()
{
    int avgW{};
    int avgH{};
    int counter{};
    int total = m_DS->labels.size();
    for (int i = 0; i < total; ++i){
        if(m_DS->map_dataPerLabel.find(m_DS->labels[i]) == m_DS->map_dataPerLabel.end())
            m_DS->map_dataPerLabel[m_DS->labels[i]] = 1;
        else
            ++m_DS->map_dataPerLabel[m_DS->labels[i]];

        if(m_DS->labels[i] == 0)
        {
            avgW += m_DS->images[i].cols;
            avgH += m_DS->images[i].rows;
            ++counter;
        }

    }

    avgW /= counter;
    avgH /= counter;
    qDebug() << "Average w/h for label  0: " << avgW << " " << avgH;
}

void ReaderGUI::load()
{
    double cpu_time;
    clock_t start = clock();


    delete m_DS;
    m_DS = new DataSet();

    switch (m_dFlag)
    {
    case Type_Standard:
    {
        m_dirStandard = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), m_dirStandard,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        QString pos_data = m_dirStandard + "/pos";
        QString neg_data = m_dirStandard + "/neg";
        auto img_N = 196;

        int tot_pos;
        int tot_neg;

        m_reader->readImages(pos_data, m_DS->images,m_dFlag);
        tot_pos = m_DS->images.size();
        for(auto i = 0; i < tot_pos; ++i)
            m_DS->labels.push_back(0);

//        // TODO: move resize to proper place
//        for(auto& img : m_DS->images)
//            cv::resize(img,img,cv::Size(196,196));

        std::vector<cv::Mat> tm_images;
        m_reader->readImages(neg_data, tm_images,m_dFlag);

        for(auto& img : tm_images)
        {
            if(img.rows > img_N && img.cols > img_N)
            {
                auto dx = img.cols - img_N;
                auto dy = img.rows - img_N;
                cv::Mat northWest(img,cv::Rect(0,0,img_N,img_N));
                cv::Mat center(img,cv::Rect(dx/2,dy/2,img_N,img_N));
                cv::Mat southEast(img,cv::Rect(dx,dy,img_N,img_N));

                m_DS->images.push_back(northWest);
                m_DS->images.push_back(center);
                m_DS->images.push_back(southEast);
            }
            else
                m_DS->images.push_back(img);
        }

//        m_reader->readImages(neg_data, m_DS->images,m_dFlag);
        tot_neg = m_DS->images.size() - tot_pos;
        for(auto i = 0; i < tot_neg; ++i)
            m_DS->labels.push_back(1);

        // TODO: move resize to proper place
        for(auto& img : m_DS->images)
            cv::resize(img,img, cv::Size(100,100));

        if (!m_DS->images.empty())
        {
            ui->LoadedDataset_label->setText(m_dirStandard);
            emit imagesLoaded(true);
        }
        break;
    }
    case Type_MNIST:
    {
        m_dirMNIST = QFileDialog::getOpenFileName(this, tr("Open Dataset File"), m_dirMNIST);
        if (!m_dirMNIST.isEmpty())
        {
            m_reader->readImages(m_dirMNIST, m_DS->images, m_dFlag);
            if (!m_DS->images.empty())
            {
                ui->LoadedDataset_label->setText(m_dirMNIST);
                emit imagesLoaded(true);
            }
            m_dirMNIST.replace(QRegExp("images.idx3"), "labels.idx1");
            m_reader->readLabels(m_dirMNIST, m_DS->labels, m_dFlag);
            if (!m_DS->labels.empty())
            {
                ui->labelFilepath_label->setText(m_dirMNIST);
                emit labelsLoaded(true);
            }
        }
        break;
    }
    default:
        break;
    }


    cpu_time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    qDebug() << "Load Time: " << cpu_time;
    countLabels();
}

void ReaderGUI::dataTypeChanged(int index)
{
    if(index == Type_MNIST)
        ui->loadLabels_checkBox->hide();
    else
        ui->loadLabels_checkBox->show();

    m_dFlag = index;
}

void ReaderGUI::writeSettings()
{
    QSettings settings("VVGLab", "ReaderGUI");
    settings.beginGroup("path");
    settings.setValue("m_dirMNIST", m_dirMNIST);
    settings.setValue("m_dirStandard", m_dirStandard);
    settings.endGroup();
}

void ReaderGUI::readSettings()
{
    QSettings settings("VVGLab", "RDFDialogGUI");
    settings.beginGroup("PARAMS");
    m_dirMNIST = settings.value("m_dirMNIST", "").toString();
    m_dirStandard = settings.value("m_dirStandard", "").toString();
    settings.endGroup();
}
