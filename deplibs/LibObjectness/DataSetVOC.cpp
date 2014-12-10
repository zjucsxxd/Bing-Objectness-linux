#include "kyheader.h"
#include "DataSetVOC.h"
#include <unistd.h>


DataSetVOC::DataSetVOC(CStr &_wkDir)
{
    wkDir = _wkDir;
    resDir = wkDir + "/Results/";
    localDir = wkDir + "/Local/";
    imgPathW = wkDir + "/JPEGImages/%s.jpg";
    annoPathW = wkDir + "/Annotations/%s.yml";
    CmFile::MkDir(resDir);
    CmFile::MkDir(localDir);

    trainSet        = CmFile::loadStrList(wkDir + "/ImageSets/Main/train.txt");
    testSet          = CmFile::loadStrList(wkDir + "/ImageSets/Main/test.txt");
    classNames = CmFile::loadStrList(wkDir + "/ImageSets/Main/class.txt");

    // testSet.insert(testSet.end(), trainSet.begin(), trainSet.end());    
    // testSet.resize(min(1000, (int)testSet.size()));

    trainNum = trainSet.size();
    testNum = testSet.size();
}


Vec4i getMaskRange(CMat &mask1u, int ext = 0)
{
    int maxX = INT_MIN, maxY = INT_MIN, minX = INT_MAX, minY = INT_MAX, rows = mask1u.rows, cols = mask1u.cols;
    for (int r = 0; r < rows; r++)    {
        const byte* data = mask1u.ptr<byte>(r);
        for (int c = 0; c < cols; c++)
            if (data[c] > 10) {
                maxX = max(maxX, c);
                minX = min(minX, c);
                maxY = max(maxY, r);
                minY = min(minY, r);
            }
    }

    maxX = maxX + ext + 1 < cols ? maxX + ext + 1 : cols;
    maxY = maxY + ext + 1 < rows ? maxY + ext + 1 : rows;
    minX = minX - ext > 0 ? minX - ext : 0;
    minY = minY - ext > 0 ? minY - ext : 0;

    return Vec4i(minX + 1, minY + 1, maxX, maxY); // Rect(minX, minY, maxX - minX, maxY - minY);
}


DataSetVOC::~DataSetVOC(void)
{
}

void DataSetVOC::loadAnnotations()
{
    gtTrainBoxes.resize(trainNum);//vector< vector<Vec4i> > 
    gtTrainClsIdx.resize(trainNum);
    clog<<"Loading training sets.........."<<endl;
    for (int i = 0; i < trainNum; i++)
        if (!loadBBoxes(trainSet[i], gtTrainBoxes[i], gtTrainClsIdx[i]))
            return;

    gtTestBoxes.resize(testNum);//vector< vector<Vec4i> > 
    gtTestClsIdx.resize(testNum);
    clog<<"Loading test sets.........."<<endl;
    for (int i = 0; i < testNum; i++)
    {
     //   cout<<"test : "<<i<<endl;
        if(!loadBBoxes(testSet[i], gtTestBoxes[i], gtTestClsIdx[i]))
            return;
    }
            
    printf("Load annotations finished\n");
}

void DataSetVOC::loadDataGenericOverCls()
{
    vecS allSet = trainSet;
    allSet.insert(allSet.end(), testSet.begin(), testSet.end());
    int imgN = (int)allSet.size();
    trainSet.clear(), testSet.clear();
    trainSet.reserve(imgN), testSet.reserve(imgN);
    vector<vector<Vec4i>> gtBoxes(imgN);
    vector<vecI> gtClsIdx(imgN);
    for (int i = 0; i < imgN; i++){
        if (!loadBBoxes(allSet[i], gtBoxes[i], gtClsIdx[i]))
            return;
        vector<Vec4i> trainBoxes, testBoxes;
        vecI trainIdx, testIdx;
        for (size_t j = 0; j < gtBoxes[i].size(); j++)
            if (gtClsIdx[i][j] < 6){
                trainBoxes.push_back(gtBoxes[i][j]);
                trainIdx.push_back(gtClsIdx[i][j]);
            }
            else{
                testBoxes.push_back(gtBoxes[i][j]);
                testIdx.push_back(gtClsIdx[i][j]);
            }
        if (trainBoxes.size()){
            trainSet.push_back(allSet[i]);
            gtTrainBoxes.push_back(trainBoxes);
            gtTrainClsIdx.push_back(trainIdx);
        }
        else{
            testSet.push_back(allSet[i]);
            gtTestBoxes.push_back(testBoxes);
            gtTestClsIdx.push_back(testIdx);
        }
    }
    trainNum = trainSet.size();
    testNum = testSet.size();
    printf("Load annotations (generic over classes) finished\n");
}

void DataSetVOC::loadBox(const FileNode &fn, vector<Vec4i> &boxes, vecI &clsIdx)
{//Modified By Xu Xiduo, xuxiduo@zju.edu.cn
    string isDifficult;
    fn["difficult"]>>isDifficult;
    if (isDifficult == "1")     return; 
    string clsName;
    
    fn["name"]>>clsName;
    size_t idx=findFromList(clsName, classNames);
    if( idx < 0 )
    {
        cerr<<"Invalidate class name !"<<endl;
        return;
    }
    clsIdx.push_back(idx);    
    
    string strXmin, strYmin, strXmax, strYmax;
    fn["bndbox"]["xmin"] >> strXmin;
    fn["bndbox"]["ymin"] >> strYmin;
    fn["bndbox"]["xmax"] >> strXmax;
    fn["bndbox"]["ymax"] >> strYmax;
    boxes.push_back(Vec4i(atoi(_S(strXmin)), atoi(_S(strYmin)), atoi(_S(strXmax)), atoi(_S(strYmax))));
}

bool DataSetVOC::loadBBoxes(CStr &nameNE, vector<Vec4i> &boxes, vecI &clsIdx)
{
    string fName = format(_S(annoPathW), _S(nameNE));
    //---------------added By Xu Xiduo------------------
    //cout<<"fName="<<fName<<endl;
    if (access(fName.c_str(),F_OK) != 0 )
    {
        cerr<<"File [ "<<fName<<" ] not found !"<<endl;
        return false;
    }
    //-------------------added end-------------------------
    FileStorage fs(fName, FileStorage::READ);
    FileNode fn = fs["annotation"]["object"];
    boxes.clear();
    clsIdx.clear();
    if (fn.isSeq())
    {
        for (FileNodeIterator it = fn.begin(), it_end = fn.end(); it != it_end; it++)
        {
            loadBox(*it, boxes, clsIdx);
        }
    }
    else
    {
        loadBox(fn, boxes, clsIdx);
    }
    
    fs.release();
    
    return true;
}

// Needs to call yml.m in this solution before running this function.
bool DataSetVOC::cvt2OpenCVYml(CStr &annoDir)
{
    vecS namesNE;
    int imgNum = CmFile::GetNamesNE(annoDir + "*.yaml", namesNE);
    printf("Converting annotations to OpenCV yml format:\n");
    for (int i = 0; i < imgNum; i++){
        printf("%d/%d %s.yaml\r", i, imgNum, _S(namesNE[i]));    
        string fPath = annoDir + namesNE[i];
        cvt2OpenCVYml(fPath + ".yaml", fPath + ".yml");
    }
    return true;
}

// Needs to call yml.m in this solution before running this function.
bool DataSetVOC::cvt2OpenCVYml(CStr &yamlName, CStr &ymlName)
{    
    ifstream f(yamlName);    
    FILE *fO = fopen(_S(ymlName), "w");
    if (!f.is_open() && fO == NULL)
        return false;
    fprintf(fO, "%s\n", "%YAML:1.0\n");
    string line;

    int addIdent = 0;
    while(getline(f, line)){
        if (line.substr(0, 12) == "  filename: ")
            line = "  filename: \"" + line.substr(12) + "\"";            
        int tmp = line.find_first_of('-');
        if (tmp != string::npos){
            bool allSpace = true;
            for (int k = 0; k < tmp; k++)
                if (line[k] != ' ')
                    allSpace = false;
            if (allSpace)
                addIdent = tmp;
        }
        for (int k = 0; k < addIdent; k++)
            fprintf(fO, " ");
        fprintf(fO, "%s\n", _S(line));
    }
    fclose(fO);

    FileStorage fs(ymlName, FileStorage::READ);
    string tmp;
    fs["annotation"]["folder"]>>tmp;
    return true;
}


// Get training and testing for demonstrating the generative of the objectness over classes
void DataSetVOC::getTrainTest()
{
    const int TRAIN_CLS_NUM = 6;
    string trainCls[TRAIN_CLS_NUM] = {"bird", "car", "cat", "cow", "dog", "sheep"};

}

void DataSetVOC::getXmlStrVOC(CStr &fName, string &buf)
{
    ifstream fin(fName);
    string strLine;
    buf.clear();
    buf.reserve(100000);
    buf += "<?xml version=\"1.0\"?>\n<opencv_storage>\n";
    while (getline(fin, strLine) && strLine.size())    {
        int startP = strLine.find_first_of(">") + 1;
        int endP = strLine.find_last_of("<");
        if (endP > startP){
            string val = keepXmlChar(strLine.substr(startP, endP - startP));
            if (val.size() < endP - startP)
                strLine = strLine.substr(0, startP) + val + strLine.substr(endP);
        }
        buf += strLine + "\n";
    }
    buf += "</opencv_storage>\n";
    //FileStorage fs(buf, FileStorage::READ + FileStorage::MEMORY);
    ofstream fout("D:/t.xml");
    fout<< buf;
}
