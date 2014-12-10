#include "kyheader.h"


#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <Commdlg.h>
#include <ShellAPI.h>
#else
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

string& trim(string& s) 
{//added by Xu Xiduo, xuxiduo@zju.edu.cn 
  if (s.empty()) {
    return s;
  }
 
  string::iterator c;
  // Erase whitespace before the string
  for (c = s.begin(); c != s.end() && iswspace(*c++););
    s.erase(s.begin(), --c);

  // Erase whitespace after the string
  for (c = s.end(); c != s.begin() && iswspace(*--c););
    s.erase(++c, s.end());

  return s;
}

// Get image names from a wildcard. Eg: GetNames("D:\\*.jpg", imgNames);
int CmFile::GetNames(CStr &_nameW, vecS &_names)
{
    string _dir = GetFolder(_nameW);
    _names.clear();

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(_dir.c_str()))!=NULL){
        //print all the files and directories within directory
        while((ent = readdir(dir))!=NULL){
            if(ent->d_name[0] == '.')
                continue;
            if(ent->d_type ==4)
                continue;
            _names.push_back(ent->d_name);
        }
        closedir(dir);
    } else {
        perror("");
        return EXIT_FAILURE;
    }
    return (int)_names.size();
}
int CmFile::GetNames(CStr &_nameW, vecS &_names, string &_dir)
{
    _dir = GetFolder(_nameW);
    _names.clear();

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(_dir.c_str()))!=NULL){
        //print all the files and directories within directory
        while((ent = readdir(dir))!=NULL){
            if(ent->d_name[0] == '.')
                continue;
            if(ent->d_type ==4)
                continue;
            _names.push_back(ent->d_name);
        }
        closedir(dir);
    } else {
        perror("");
        return EXIT_FAILURE;
    }
    return (int)_names.size();
}

int CmFile::GetSubFolders(CStr &folder, vecS &subFolders)
{
    subFolders.clear();
    string nameWC = GetFolder(folder);//folder + "/*";

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(nameWC.c_str()))!=NULL){
        while((ent = readdir(dir))!=NULL){
            if(ent->d_name[0] == '.')
                continue;
            if(ent->d_type == 4){
                subFolders.push_back(ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("");
        return EXIT_FAILURE;
    }
    return (int)subFolders.size();
}

int CmFile::GetNames(CStr& rootFolder, CStr &fileW, vecS &names)
{
    GetNames(rootFolder + fileW, names);
    vecS subFolders, tmpNames;
    int subNum = CmFile::GetSubFolders(rootFolder, subFolders);//
    for (int i = 0; i < subNum; i++){
        subFolders[i] += "/";
        int subNum = GetNames(rootFolder + subFolders[i], fileW, tmpNames);
        for (int j = 0; j < subNum; j++)
            names.push_back(subFolders[i] + tmpNames[j]);
    }
    return (int)names.size();
}

int CmFile::GetNamesNE(CStr& nameWC, vecS &names)
{
    string dir = string();
    string ext = string();
    int fNum = GetNames(nameWC, names, dir);
    ext = GetExtention(nameWC);
    for (int i = 0; i < fNum; i++)
        names[i] = GetNameNE(names[i]);
    return fNum;
}

int CmFile::GetNamesNE(CStr& nameWC, vecS &names, string &dir, string &ext)
{
    int fNum = GetNames(nameWC, names, dir);
    ext = GetExtention(nameWC);
    for (int i = 0; i < fNum; i++)
        names[i] = GetNameNE(names[i]);
    return fNum;
}

int CmFile::GetNamesNE(CStr& rootFolder, CStr &fileW, vecS &names)
{
    int fNum = GetNames(rootFolder, fileW, names);
    int extS = GetExtention(fileW).size();
    for (int i = 0; i < fNum; i++)
        names[i].resize(names[i].size() - extS);
    return fNum;
}

bool CmFile::MkDir(CStr &_path)
{
    if(_path.size() == 0)
        return false;
    static char buffer[1024];
    strcpy(buffer, _S(_path));
#ifdef _WIN32
    for (int i = 0; buffer[i] != 0; i ++) {
        if (buffer[i] == '\\' || buffer[i] == '/') {
            buffer[i] = '\0';
            CreateDirectoryA(buffer, 0);
            buffer[i] = '/';
        }
    }
    return CreateDirectoryA(_S(_path), 0);
#else
    for (int i = 0; buffer[i] != 0; i ++) {
        if (buffer[i] == '\\' || buffer[i] == '/') {
            buffer[i] = '\0';
            mkdir(buffer, 0755);
            buffer[i] = '/';
        }
    }
    return mkdir(_S(_path), 0755);
#endif
}

vecS CmFile::loadStrList(CStr &fName)
{//modified By Xu Xiduo, xuxiduo@zju.edu.cn
    ifstream fIn(fName);
    string line;
    vecS strs;
    while(getline(fIn, line) && line.size())
    {
		line=trim(line);
        //unsigned sz = line.size();
       //  cout<<" line="<<line<<endl;
        //line.resize(sz - 1); //Please use script to convert the VOC format data into the OpenCV format data
        //line.resize(sz); //Please use script to convert the VOC format data into the OpenCV format data
        //cout<<fName<<" line="<<line<<endl;
        //line.resize(sz);
        strs.push_back(line);
    }
    fIn.close();
    return strs;
}

bool CmFile::writeStrList(CStr &fName, const vecS &strs)
{//modified By Xu Xiduo, xuxiduo@zju.edu.cn
    FILE *f = fopen(_S(fName), "w");
    
    if (f == NULL)
    {
		cerr<<"Open [ "<<fName<<" ] for writing failed !"<<endl;
        return false;
     }
     
    for (size_t i = 0; i < strs.size(); i++)
        fprintf(f, "%s\n", _S(strs[i]));
        
    fclose(f);
    
    return true;
}
