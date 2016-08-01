#ifndef TARGET_H
#define TARGET_H



class Target
{
  public:
    Target(QString label, QImage image);
    void setLabel(QString label);
    void setImage( QImage image);
    QString getLabel();
    QImage getImage();


    ~Target();

  private:
    QString m_Label;
    QImage m_TargetImg;

};

#endif // TARGET_H
