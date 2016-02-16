#include "include/Reader.h"

void Reader::readFromTo(string filename, vector<QString> &imgName)
{
    struct dirent *ent;

    // check for valid directory
    if ((m_dir = opendir(filename.c_str())) != NULL) {

        QString prev = "abdd";

        // read from folder untill it is empty
        while((ent = readdir(m_dir)) != NULL)
        {
            // get filename and convert it to string
            string str = (string)ent->d_name;
            size_t pos = str.find(".");
            if (pos == string::npos || pos == 0)
                continue;

//            cout<<str.substr(0,pos)<<endl;
            QString file = QString::fromStdString(str.substr(0,pos));

            if(prev != file){
                imgName.push_back(file);
                prev = file;
            }
        }
        closedir(m_dir);
    }
    else
        cerr<<"Couldn't open directory!"<<endl;
}
