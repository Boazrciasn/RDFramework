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

void ReaderGUI::loadFrames(QString dir, std::vector<cv::Mat> &images, QVector<QVector<QRect>> &frameRects)
{
    QFile inputFile(dir + "/LOG_Video_Stream_2.txt");
    int max_img = ui->max_img_load_spinBox->value();
    int skip_img = ui->skip_img_load_spinBox->value();
    auto drawRect = ui->draw_rect_checkBox->isChecked();
    auto load_color = ui->Load_color_comboBox->currentIndex();

    auto img_size = 416;
    auto img_row_ratio = 1.0f;
    auto img_col_ratio = 1.0f;
    auto max_w_h = 100;

    auto skip_counter = 0;
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       QString line;
       while (!in.atEnd())
       {
          line = in.readLine();
          if(line.contains("frame_"))
          {
            if(++skip_counter <= skip_img)
                continue;
            QStringList file_ = line.trimmed().split(" ");
            QString img_file = dir + "/frames/" + file_[0] + file_[1] +".png";
            cv::Mat img = cv::imread(img_file.toStdString(),load_color);
            img_row_ratio = (float)img_size/img.rows;
            img_col_ratio = (float)img_size/img.cols;
            cv::resize(img,img,cv::Size(img_size,img_size));

            QVector<QRect> rects;
            line = in.readLine();
            auto count = 0;
            while (!in.atEnd() && !line.isEmpty())
            {
                QStringList rect = line.split(" ");
                if(count%2 == 1 && rect[2].toInt()*img_col_ratio < max_w_h && rect[3].toInt()*img_row_ratio < max_w_h)
                {
                    rects.push_back(QRect(rect[0].toInt()*img_col_ratio,rect[1].toInt()*img_row_ratio,rect[2].toInt()*img_col_ratio,rect[3].toInt()*img_row_ratio));
                    if(drawRect)
                    {
                        auto rect = rects.last();
                        cv::rectangle(img,cv::Rect(rect.x(),rect.y(),rect.width(),rect.height()), cv::Scalar(200,0,0),2);
                    }
                }

                line = in.readLine();
                count++;
            }


            images.push_back(img);
            frameRects.push_back(rects);
            if(max_img != 0 && images.size() >= max_img)
                break;
          }
       }
       inputFile.close();
    }

    if(ui->lab_checkBox->isChecked())
        for(auto& img : images)
            cv::cvtColor(img,img,CV_BGR2Lab);
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
        m_fileDir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), m_fileDir,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        QString pos_data = m_fileDir + "/pos";
        QString neg_data = m_fileDir + "/neg";
        auto img_N = 196;

        int tot_pos;
        int tot_neg;

        // Load Positive
        m_reader->readImages(pos_data, m_DS->images,m_dFlag);
        tot_pos = m_DS->images.size();
        for(auto i = 0; i < tot_pos; ++i)
            m_DS->labels.push_back(0);

        // Load Negative
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



////  Save Loaded Data
//        QString pos_ = m_dirStandard + "/dataSaved/pos/pos_";
//        QString neg_ = m_dirStandard + "/dataSaved/neg/neg_";


//        for(auto i = 0; i < tot_pos; ++i)
//        {
//            std::string file_pos = (pos_ + QString::number(i) + ".jpg").toStdString();
//            cv::imwrite(file_pos, m_DS->images[i]);
//        }

//        for(auto i = tot_pos; i < tot_pos + tot_neg; ++i)
//        {
//            std::string file_neg = (neg_ + QString::number(i-tot_pos) + ".jpg").toStdString();
//            cv::imwrite(file_neg, m_DS->images[i]);
//        }


        // TODO: move resize to proper place
        for(auto& img : m_DS->images)
            cv::resize(img,img, cv::Size(50,50));

        break;
    }
    case Type_MNIST:
    {
        m_fileDir = QFileDialog::getOpenFileName(this, tr("Open Dataset File"), m_fileDir);
        if (!m_fileDir.isEmpty())
        {
            m_reader->readImages(m_fileDir, m_DS->images, m_dFlag);
            m_fileDir.replace(QRegExp("images.idx3"), "labels.idx1");
            m_reader->readLabels(m_fileDir, m_DS->labels, m_dFlag);
        }
        break;
    }
    case Type_STD_ONE:
    {
        m_fileDir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"), m_fileDir,
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        loadFrames(m_fileDir, m_DS->images, m_DS->frameRects);
        break;
    }
    default:
        break;
    }


    cpu_time = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    qDebug() << "Load Time: " << cpu_time;
//    countLabels();


    // Emit signals
    if (!m_DS->images.empty())
    {
        ui->LoadedDataset_label->setText(m_fileDir);
        emit imagesLoaded(true);
    }

    if (!m_DS->labels.empty())
    {
        ui->labelFilepath_label->setText(m_fileDir);
        emit labelsLoaded(true);
    }
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
    settings.setValue("m_fileDir", m_fileDir);
    settings.endGroup();
}

void ReaderGUI::readSettings()
{
    QSettings settings("VVGLab", "RDFDialogGUI");
    settings.beginGroup("PARAMS");
    m_fileDir = settings.value("m_fileDir", "").toString();
    settings.endGroup();
}
